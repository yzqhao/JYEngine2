#include "BaseFile.h"

NS_JYE_BEGIN

BaseFile::BaseFile(const std::string& url)
	:m_FileUrl(url),
	m_ActionType(AT_NONE),
	m_isOpen(false),
	m_FileSize(0),
	m_PosIndex(0),
	m_BufferSize(0),
	m_ReadedBufferSize(0),
	m_pDatas(NULL)
{
}

BaseFile::~BaseFile(void)
{
	JY_ASSERT(false == m_isOpen);
}

bool BaseFile::OpenFile(ActionType at)
{
	JY_ASSERT(m_isOpen == false);
	m_isOpen = _DoOpenFile(at);
	if (m_isOpen)
	{
		switch (at)
		{
		case AT_READ:
		{
			m_BufferSize = m_FileSize > FC_FILEBUFFER ? FC_FILEBUFFER : m_FileSize;
			m_pDatas = _NEW byte[m_BufferSize];
			m_ReadedBufferSize += m_BufferSize;
			_DoReadFile(m_pDatas, m_BufferSize);//重新填充
		}break;
		case AT_WRITE:
		{
			m_BufferSize = FC_FILEBUFFER;
			m_pDatas = _NEW byte[m_BufferSize];
		}break;
		default:JY_ASSERT(false);
		}
	}
	m_ActionType = m_isOpen ? at : AT_NONE;
	return m_isOpen;
}

bool BaseFile::CloseFile()
{
	if (m_isOpen)
	{
		if (m_PosIndex != 0 && AT_WRITE == m_ActionType)
		{
			_DoWriteFile(m_pDatas, m_PosIndex);
		}
		m_isOpen = !_DoCloseFile();
		m_PosIndex = 0;
		SAFE_DELETE_ARRAY(m_pDatas);
	}
	return !m_isOpen;
}

int BaseFile::Seek(int to, int whence)
{
	int result = 0;
	JY_ASSERT(m_isOpen == true);
	switch (m_ActionType)
	{
	case AT_READ:
	{
		switch (whence)
		{
		case SEEK_SET:
		{
			result = _DoSeekFile(to, whence);
			m_ReadedBufferSize = to;
			m_BufferSize = m_FileSize - m_ReadedBufferSize > FC_FILEBUFFER ? FC_FILEBUFFER : m_FileSize - m_ReadedBufferSize;
		}break;
		case SEEK_CUR:
		{
			int realto = to - m_BufferSize + m_PosIndex;
			m_ReadedBufferSize += realto;
			result = _DoSeekFile(realto, whence);
			m_BufferSize = m_FileSize - m_ReadedBufferSize > FC_FILEBUFFER ? FC_FILEBUFFER : m_FileSize - m_ReadedBufferSize;
		}break;
		case SEEK_END:
		{
			result = _DoSeekFile(to, whence);
			m_ReadedBufferSize = m_FileSize - to;
			m_BufferSize = to > FC_FILEBUFFER ? FC_FILEBUFFER : to;
		}break;
		default: JY_ASSERT(false);
		}
		m_PosIndex = m_BufferSize;//制空当先buffer数据堆
	}break;
	case AT_WRITE:
	{
		if (0 != m_PosIndex)
		{
			_DoWriteFile(m_pDatas, m_PosIndex);//把已有数据写入
			m_PosIndex = 0;
		}
		result = _DoSeekFile(to, whence);
	}break;
	default:
	{
		JY_ASSERT(false);
	}break;
	}
	return result;
}

int BaseFile::Tell()
{
	JY_ASSERT(m_isOpen == true);
	switch (m_ActionType)
	{
	case AT_READ: return _DoTell() - m_BufferSize + m_PosIndex;
	case AT_WRITE:return _DoTell() + m_PosIndex;
	default: JY_ASSERT(false); break;
	}
	return 0;
}

int BaseFile::ReadFile(void* data, int bitesize)
{
	if (!m_isOpen)
	{
		OpenFile(AT_READ);
	}
	JY_ASSERT(AT_READ == m_ActionType);
	if (bitesize > m_BufferSize || bitesize > (m_BufferSize - m_PosIndex))
	{
		int readsize = m_BufferSize - m_PosIndex;	//buffer中的剩余数据加载出来先
		memcpy(data, m_pDatas + m_PosIndex, readsize);
		int remainsize = bitesize - readsize;//剩余需要读取的
		if (remainsize > FC_FILEBUFFER)	//当剩余的需要读取的大于buffer
		{
			int res = _DoReadFile(((byte*)data) + readsize, remainsize);
			readsize += res;
			m_ReadedBufferSize += res;

			//重新填满buffer
			m_BufferSize = std::min(m_FileSize - m_ReadedBufferSize, (int)FC_FILEBUFFER);
			res = _DoReadFile(m_pDatas, m_BufferSize);
			m_ReadedBufferSize += res;
			m_PosIndex = 0;

			return readsize;
		}
		else
		{
			m_BufferSize = std::min(m_FileSize - m_ReadedBufferSize, (int)FC_FILEBUFFER);
			int res = _DoReadFile(m_pDatas, m_BufferSize);
			JY_ASSERT(res == m_BufferSize);
			m_PosIndex = 0;
			m_ReadedBufferSize += m_BufferSize;

			res = std::min(remainsize, m_BufferSize);
			memcpy(((byte*)data) + readsize, m_pDatas, res);

			readsize += res;
			m_PosIndex = res;

			return readsize;
		}
	}
	else
	{
		memcpy(data, m_pDatas + m_PosIndex, bitesize);
		m_PosIndex += bitesize;
		return bitesize;
	}
	return 0;
}

int BaseFile::WriteFile(const void* data, int bitesize)
{
	if (!m_isOpen)
	{
		OpenFile(AT_WRITE);
	}
	JY_ASSERT(AT_WRITE == m_ActionType);

	if (AT_WRITE != m_ActionType)
	{
		return 0;
	}

	if (bitesize > m_BufferSize || bitesize > (m_BufferSize - m_PosIndex))
	{
		Flush();
		m_FileSize += bitesize;
		return _DoWriteFile(data, bitesize);
	}
	else
	{
		memcpy(m_pDatas + m_PosIndex, data, bitesize);
		m_PosIndex += bitesize;
		m_FileSize += bitesize;
	}
	return bitesize;
}

bool BaseFile::Flush()
{
	bool res = false;
	if (m_PosIndex > 0)
	{
		int writed(0);
		writed = _DoWriteFile(m_pDatas, m_PosIndex);
		res = writed == m_PosIndex;
		res &= _DoFlush();
		JY_ASSERT(m_PosIndex >= writed);
		m_PosIndex -= writed;
	}
	return res;
}

const std::string& BaseFile::GetFilePath()
{
	return m_FileUrl;
}

uint BaseFile::GetSize()
{
	return _GetFileSize();
}

bool BaseFile::isOpen()
{
	return m_isOpen;
}

NS_JYE_END
