
#include "FileSystem.h"
#include "System/Utility.hpp"
#include "CDirEntry.h"
#include "PackageFile.h"
#include "UsualFile.h"
#include "ZippedFile.h"
#include "Core/Configure.h"
#include "Core/Interface/ILogSystem.h"

#ifdef PLATFORM_WIN32
#include <direct.h>
#include <io.h>
#else
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#endif

NS_JYE_BEGIN

static const int IO_SUBPATH_POS = 5;

static bool _SplitZipFileName(const std::string& path, std::string& zipName, std::string& fileName)
{
	zipName = "";
	fileName = "";
	std::string oriPath = CDirEntry::normalize(path);
#ifdef PLATFORM_WIN32
	std::string parent = oriPath;
#else
	std::string parent = path;
#endif
	while (1)
	{
		parent = CDirEntry::dirName(parent);
		if (parent == "")
			break;
		auto suffix = CDirEntry::suffix(parent);
		if (suffix != std::string(Configure::archiveSuffix))
		{
#ifdef PLATFORM_WIN32
			if (0 == _access(parent.c_str(), 0))
#else
			if (0 == access(parent.c_str(), 0))
#endif
			{
				break;
			}
			zipName = parent + std::string(Configure::archiveSuffix);
		}
		else
		{
			zipName = parent;
		}
		int errorp = ZIP_ER_OK;
		zip_t* file = zip_open(zipName.c_str(), ZIP_RDONLY, &errorp);
		if (errorp != ZIP_ER_OK)
		{
			continue;
		}
		fileName = oriPath.substr(parent.length() + 1);
		const char* suffixes[3] = { "", Configure::compressionSuffix ,Configure::dictCompressionSuffix };
		for (int i = 0; i < 3; ++i)
		{
			if (zip_name_locate(file, (fileName + suffixes[i]).c_str(), ZIP_FL_ENC_RAW) != -1)
			{
				fileName += suffixes[i];
				zip_close(file);
				return true;
			}
		}
		zip_close(file);
	}

	zipName = "";
	fileName = "";
	return false;
}

FileSystem::FileSystem()
{

}

FileSystem::~FileSystem()
{
    
}

IFile* FileSystem::_DoFileFactory(const String& path)
{
	std::string zipName, subEntryName;
	if (_SplitZipFileName(path, zipName, subEntryName))
	{
		return _NEW ZippedFile(zipName, subEntryName);
	}
	if (CDirEntry::suffix(path) == std::string(Configure::archiveSuffix))
	{
		return _NEW PackageFile(path);
	}
	return _NEW UsualFile(path);
}

void FileSystem::RecycleBin(IFile* file)
{
    SAFE_DELETE(file);
}

std::string FileSystem::GetTimestamp(const std::string& path)
{
	String realpath = PathAssembly(path);
#ifdef PLATFORM_WIN32
	if (0 == _access(realpath.c_str(), 0))
#else
	if (0 == access(realpath.c_str(), 0))
#endif
	{
		struct stat attrib;
		stat(realpath.c_str(), &attrib);
		return ctime(&attrib.st_mtime);
	}
	else
	{
		String zipName, subEntryName;
		bool res = _SplitZipFileName(realpath, zipName, subEntryName);
		if (!res)
		{
			JYERROR("GetTimestamp failed, file %s does not exist.", path.c_str());
			return "";
		}
		struct stat attrib;
		stat(zipName.c_str(), &attrib);
		return ctime(&attrib.st_mtime);
	}
}

bool FileSystem::_DoIsFileExist(const String& filePath)
{
	if (0 == _access(filePath.c_str(), 0))//如果直接存在
	{
		return true;
	}

	std::string zipName, entryName;
	if (_SplitZipFileName(filePath, zipName, entryName))
	{
		return true;
	}

	return false;
}

IFileSystem::FileType FileSystem::_DoGetFileType(const std::string& path)
{
	struct stat s;
	if (stat(path.c_str(), &s) == 0)
	{
		if (s.st_mode & S_IFDIR)
		{
			return FT_DIR;
		}
		else if (s.st_mode & S_IFREG)
		{
			return FT_FILE;
		}
		else
		{
			return FT_STH_ELSE;
		}
	}
	else
	{
		std::string zipName, entryName;
		if (_SplitZipFileName(path, zipName, entryName))
		{
			return FT_ZIP;
		}
	}
	return FT_NO_EXIST;
}

void FileSystem::_DoMakeFolder(const std::string& path)
{
	if (_access(path.c_str(), 0) == -1) //如果文件夹不存在
	{
		JYWARNING("make folder %s\n", path.c_str());
		_mkdir(path.c_str());
	}
}

bool FileSystem::_DoDeleteFile(const std::string& strFileName)
{
	auto type = _DoGetFileType(strFileName);
	if (type != FT_NO_EXIST && type != FT_ZIP)
	{
		remove(strFileName.c_str());
	}
	return true;
}


NS_JYE_END