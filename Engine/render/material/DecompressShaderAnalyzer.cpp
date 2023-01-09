#include "DecompressShaderAnalyzer.h"
#include "Core/Interface/IFile.h"
#include "Core/Interface/IFileSystem.h"
#include "Core/Interface/ILogSystem.h"

#include <cstdlib>

using namespace std;

NS_JYE_BEGIN

DecompressShaderAnalyzer DecompressShaderAnalyzer::instance;

void DecompressShaderAnalyzer::DecompressShaderFromFile(handle filehandle,
	int blockIndex, int blockOffset, int readSize, std::string& filecontext)
{
	std::lock_guard<std::mutex> mut(m_mutex);
	if (filehandle >= MAX_HANDLES || m_fileList[filehandle].m_file == NULL)
	{
		JYERROR("DecompressShaderAnalyzer: uncompress before loading file %s.", m_fileList[filehandle].m_name.c_str());
		return ;
	}
	IFile* filecont = m_fileList[filehandle].m_file;
		
	IFile* subFile = filecont->GetFileFromPackage(to_string(blockIndex) + ".lzd");
	if (subFile == NULL)
	{
		JYERROR("DecompressShaderAnalyzer: Unable to find subfile %d in %s.",
					blockIndex, m_fileList[filehandle].m_name.c_str());
		return ;
	}
	vector<char> filebuffer(subFile->GetSize());
	subFile->ReadFile(filebuffer.data(), subFile->GetSize());
	subFile->CloseFile();
	filecont->RecycleFileInPackage(subFile);
	filecontext = string(filebuffer.data() + blockOffset, filebuffer.data() + blockOffset + readSize);
}

IFile* DecompressShaderAnalyzer::ReadFile(const std::string& filename)
{
	if (!IFileSystem::Instance()->isFileExist(filename))
	{
		JYERROR("DecompressShaderAnalyzer: compressed shader file %s not exists", filename.c_str());
		return nullptr;
	}
	IFile* readfile = IFileSystem::Instance()->FileFactory(filename);
	readfile->OpenFile(IFile::AT_READ);
	return readfile;
}

handle DecompressShaderAnalyzer::LoadFile(const std::string& filename)
{
	handle filehandle;
	std::lock_guard<std::mutex> mut(m_mutex);
	while (m_fileList[m_handleCnt].m_file != NULL)
	{
		m_handleCnt++;
	}
	filehandle = m_handleCnt++;
	m_fileList[filehandle] = { ReadFile(filename), filename };
	return filehandle;
}

void DecompressShaderAnalyzer::UnloadFile(handle filehandle)
{
	std::lock_guard<std::mutex> mut(m_mutex);
	if (filehandle >= MAX_HANDLES || m_fileList[filehandle].m_file == NULL)
	{
		JYERROR("DecompressShaderAnalyzer: unload failed, empty handle.");
	}
	else
	{
		IFile* file = m_fileList[filehandle].m_file;
		file->CloseFile();
		IFileSystem::Instance()->RecycleBin(file);
		m_fileList[filehandle] = { NULL, string("") };
	}
}

DecompressShaderAnalyzer::DecompressShaderAnalyzer()
{
	m_fileList.resize(MAX_HANDLES);
	m_handleCnt = 0u;
}

DecompressShaderAnalyzer::~DecompressShaderAnalyzer()
{
	for (int i = 0; i < MAX_HANDLES; ++i)
	{
		if (m_fileList[i].m_file != NULL)
		{
			IFile* file = m_fileList[i].m_file;
			file->CloseFile();
			IFileSystem::Instance()->RecycleBin(file);
		}
	}
}

NS_JYE_END
