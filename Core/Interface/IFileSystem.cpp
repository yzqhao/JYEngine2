
#include "IFileSystem.h"
#include "IFile.h"
#include "../../System/Utility.hpp"
#include "../Configure.h"
#include "IApplication.h"

NS_JYE_BEGIN

#define FILE_PATH_FOURCC_ROOT		'root'
#define FILE_PATH_FOURCC_SAVE		'save'
#define FILE_PATH_FOURCC_DOCUMENT	'docs'
#define FILE_PATH_FOURCC_SCRIPT		'scrs'
#define FILE_PATH_FOURCC_COMMON		'comm'
#define FILE_PATH_FOURCC_EDITOR_PROJECT	'proj'
#define FILE_PATH_FOURCC_EDITOR_SCRIPT	'edsc'
#define FILE_PATH_FOURCC_EDITOR_DOC	'eddc'

SYSTEM_SINGLETON_IMPLEMENT(IFileSystem);

IFileSystem::IFileSystem()
{
	SYSTEM_SINGLETON_INITIALIZE;

	const std::string& root = IApplication::Instance()->GetRootpath();
	const std::string& save = IApplication::Instance()->GetSavepath();
	_root = PathAssembly(root);
	_save = PathAssembly(save);
}

IFileSystem::~IFileSystem()
{
	SYSTEM_SINGLETON_DESTROY;
}

IFile* IFileSystem::FileFactory(const String& path)
{
	return _DoFileFactory(PathAssembly(path));
}

bool IFileSystem::isFileExist(const String& filePath)
{
	return _DoIsFileExist(PathAssembly(filePath));
}

IFileSystem::FileType IFileSystem::GetFileType(const std::string& path)
{
	return _DoGetFileType(path);
}

void IFileSystem::MakeFolder(const std::string& path)
{
	_DoMakeFolder(PathAssembly(path));
}

bool IFileSystem::DeleteFile(const std::string& path)
{
	return _DoDeleteFile(path);
}

void IFileSystem::SetRootPath(const String& path)
{
	_root = PathAssembly(path);
}

void IFileSystem::SetSavePath(const String& path)
{
	_save = PathAssembly(path);
}

void IFileSystem::SetDocmPath(const String& path)
{
	_docm = PathAssembly(path);
}

void IFileSystem::SetScrsPath(const String& path)
{
	_scrs = PathAssembly(path);
}

void IFileSystem::SetCommPath(const String& path)
{
	_comm = PathAssembly(path);
}

#ifdef _EDITOR
void IFileSystem::SetEdscPath(const String& path)
{
	m_edscPath = PathAssembly(path);
}

void IFileSystem::SetEddcPath(const String& path)
{
	m_eddcPath = PathAssembly(path);
}

void IFileSystem::SetAsstPath(const String& path)
{
	m_asstPath = PathAssembly(path);
}

void IFileSystem::SetProjPath(const String& path)
{
	m_projPath = PathAssembly(path);
}

#endif

const String& IFileSystem::GetRootPath() const
{
	return (_root);
}

const String& IFileSystem::GetSavePath() const
{
	return (_save);
}

const String& IFileSystem::GetDocmPath() const
{
	return (_docm);
}

const String& IFileSystem::GetScrsPath() const
{
	return (_scrs);
}

const String& IFileSystem::GetCommPath() const
{
	return (_comm);
}

#ifdef _EDITOR
const String& IFileSystem::GetEdscPath() const
{
	return (m_edscPath);
}

const String& IFileSystem::GetEddcPath() const
{
	return (m_eddcPath);
}
const String& IFileSystem::GetAsstPath() const
{
	return (m_asstPath);
}

const String& IFileSystem::GetProjPath() const
{
	return (m_projPath);
}
#endif

String IFileSystem::PathAssembly(const String& path)
{
	String res = path;
	if (path.length() > 4)
	{
		uint id = static_cast<int>(path[0]) << 24
			| static_cast<int>(path[1]) << 16
			| static_cast<int>(path[2]) << 8
			| static_cast<int>(path[3]);
		switch (id)
		{
		case FILE_PATH_FOURCC_ROOT: res = _root + path.substr(IO_SUBPATH_POS, path.length()); break;
		case FILE_PATH_FOURCC_SAVE: res = _save + path.substr(IO_SUBPATH_POS, path.length()); break;
		case FILE_PATH_FOURCC_DOCUMENT: res = _docm + path.substr(IO_SUBPATH_POS, path.length()); break;
		case FILE_PATH_FOURCC_SCRIPT: res = _scrs + path.substr(IO_SUBPATH_POS, path.length()); break;
		case FILE_PATH_FOURCC_COMMON: res = _comm + path.substr(IO_SUBPATH_POS, path.length()); break;
#ifdef _EDITOR
		case FILE_PATH_FOURCC_EDITOR_PROJECT: return m_projPath + path.substr(IO_SUBPATH_POS);
		case FILE_PATH_FOURCC_EDITOR_SCRIPT: return m_edscPath + path.substr(IO_SUBPATH_POS);
		case FILE_PATH_FOURCC_EDITOR_DOC: return m_eddcPath + path.substr(IO_SUBPATH_POS);
#endif
		}
	}
	Utility::ReplaceAllString(res, "//", "/");
	return res;
}

const std::vector<char>& IFileSystem::GetDictionaryFile()
{
	if (m_pDictionaryFile.empty())
	{
		IFile* dictFile = _DoFileFactory(PathAssembly(Configure::DICTIONARY_PATH));
		if (dictFile != NULL)
		{
			dictFile->OpenFile(IFile::AT_READ);
			uint size = dictFile->GetSize();
			m_pDictionaryFile.resize(size);
			dictFile->ReadFile(m_pDictionaryFile.data(), size);
			dictFile->CloseFile();
			RecycleBin(dictFile);
		}
	}
	return m_pDictionaryFile;
}

NS_JYE_END