#include "UsualFile.h"
#include "CDirEntry.h"
#include "Core/Interface/ILogSystem.h"

NS_JYE_BEGIN

UsualFile::UsualFile(const std::string& url)
	:BaseFile(url),
	m_pFile(NULL)
{
}

UsualFile::~UsualFile(void)
{
}

bool UsualFile::_DoOpenFile(IFile::ActionType at)
{
	switch (at)
	{
	case IFile::AT_WRITE:
	{
		CDirEntry::createDir(CDirEntry::dirName(GetFilePath()));
		fopen_s(&m_pFile, GetFilePath().c_str(), "wb");
	}break;
	case IFile::AT_READ:
	{
		fopen_s(&m_pFile, GetFilePath().c_str(), "rb");
	}break;
	default: JYERROR("can't open usual file. invalid action type");
	}
	if (NULL == m_pFile)
	{
		JYERROR("File %s Can not Open! \n", GetFilePath().c_str());
	}
	else
	{
		uint nCurrentPos = ftell(m_pFile);//读文件大小
		fseek(m_pFile, 0, SEEK_END);
		uint size = ftell(m_pFile) + nCurrentPos;
		fseek(m_pFile, nCurrentPos, SEEK_SET);
		_SetFileSize(size);
	}
	return m_pFile != NULL ? true : false;
}

bool UsualFile::_DoCloseFile()
{
	return fclose(m_pFile) == -1 ? false : true;
}

int UsualFile::_DoReadFile(void* data, uint bitesize)
{
	return fread(data, 1, bitesize, m_pFile);
}

int UsualFile::_DoSeekFile(int to, int whence)
{
	return fseek(m_pFile, to, whence);
}

int UsualFile::_DoTell()
{
	return ftell(m_pFile);
}

bool UsualFile::_DoFlush()
{
	return 0 == fflush(m_pFile);
}

int UsualFile::_DoWriteFile(const void* data, uint bitesize)
{
	int size = fwrite(data, 1, bitesize, m_pFile);
	return size;
}

NS_JYE_END
