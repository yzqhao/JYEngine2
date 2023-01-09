#pragma once

#include "private/Define.h"
#include "Core/Interface/IFile.h"

NS_JYE_BEGIN

class BaseFile : public IFile
{
private:
	enum Constant
	{
		FC_FILEBUFFER = 1024 * 32,	//32kb缓存的buffer
	};
private:
	std::string				m_FileUrl;
	ActionType				m_ActionType;
	byte* m_pDatas;
	int						m_BufferSize;//缓存的大小，当文件小于FC_FILEBUFFER就需要计算缓存大小
	int						m_FileSize;
	int						m_PosIndex;
	int						m_ReadedBufferSize;//已经加载的buffer大小
	bool					m_isOpen;
protected:
	BaseFile(const std::string& url);
	virtual ~BaseFile(void);
protected:
	virtual bool _DoOpenFile(IFile::ActionType at) = 0;
	virtual bool _DoCloseFile() = 0;
	virtual int _DoSeekFile(int to, int whence) = 0;
	virtual int _DoTell() = 0;
	virtual int _DoReadFile(void* data, uint bitesize) = 0; //实际的读取函数
	virtual int _DoWriteFile(const void* data, uint bitesize) = 0; //实际的写入函数
	virtual bool _DoFlush() = 0;
public:
	virtual bool OpenFile(IFile::ActionType at);
	virtual bool isOpen();
	virtual bool CloseFile();
	virtual int Seek(int to, int whence);
	virtual int Tell();
	virtual bool Flush();
	virtual int ReadFile(void* data, int bitesize);
	virtual int WriteFile(const void* data, int bitesize);
	virtual const std::string& GetFilePath();
	virtual uint GetSize();
protected:
	FORCEINLINE uint _GetFileSize();
	FORCEINLINE void _SetFileSize(uint size);
};


FORCEINLINE uint BaseFile::_GetFileSize()
{
	if (!m_isOpen)//当没有打开文件就读取大小肯定是读文件
	{
		OpenFile(AT_READ);
	}
	return m_FileSize;
}

FORCEINLINE void BaseFile::_SetFileSize(uint size)
{
	m_FileSize = size;
}

NS_JYE_END
