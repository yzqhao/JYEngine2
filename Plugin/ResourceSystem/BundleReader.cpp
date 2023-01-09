
#include "BundleReader.h"

NS_JYE_BEGIN

BundleReader::BundleReader()
{
    m_buffer = nullptr;
    m_position = 0;
    m_length = 0;
};

BundleReader::~BundleReader()
{
    
};

void BundleReader::init(char* buffer, uint length)
{
    m_position = 0;
    m_buffer  = buffer;
    m_length = length;
}

uint BundleReader::read(void* ptr, uint size, uint count)
{
    if (!m_buffer || eof())
    {
        return 0;
    }

    uint validCount;
    uint validLength = m_length - m_position;
    uint needLength = size*count;
    char* ptr1 = (char*)ptr;
    if(validLength < needLength)
    {
        validCount = validLength/size;
        uint readLength = size*validCount;
        memcpy(ptr1,(char*)m_buffer+m_position,readLength);
        ptr1 += readLength;
        m_position += readLength;
        readLength = validLength - readLength;
        if(readLength>0)
        {
            memcpy(ptr1,(char*)m_buffer+m_position,readLength);
            m_position += readLength;
			validCount += 1;
		}
    }
    else
    {
        memcpy(ptr1,(char*)m_buffer+m_position,needLength);
        m_position += needLength;
        validCount = count;
    }

    return validCount;
}

char* BundleReader::readLine(int num,char* line)
{
    if (!m_buffer)
        return nullptr;

    char* buffer = (char*)m_buffer+m_position;
    char* p = line;
    char c;
    uint readNum = 0;
    while((c=*buffer) != 10 && readNum < (uint)num && m_position < m_length)
    {
        *p = c;
        p++;
        buffer++;
        m_position++;
        readNum++;
    }
    *p = '\0';

    return line;
}

bool BundleReader::eof()
{
    if (!m_buffer)
        return true;
    
    return ((uint)tell()) >= length();
}

uint BundleReader::length()
{
    return m_length;
}

uint BundleReader::tell()
{
    if (!m_buffer)
        return -1;
    return m_position;
}

bool BundleReader::seek(long int offset, int origin)
{
    if (!m_buffer)
        return false;

    if(origin == SEEK_CUR)
    {
        m_position += offset;
    }
    else if(origin == SEEK_SET)
    {
        m_position = offset;
    }
    else if(origin == SEEK_END)
    {
        m_position = m_length+offset;
    }
    else
        return false;

    return true;
}

bool BundleReader::rewind()
{
    if (m_buffer != nullptr)
    {
        m_position = 0;
        return true;
    }
    return false;
}

std::string BundleReader::readString()
{
    unsigned int length;
    if(read(&length, 4, 1) != 1)
    {
        return std::string();
    }

    std::string str;
    
    uint validLength = m_length - m_position;
    if (length > 0 && static_cast<uint>(length) <= validLength)
    {
        str.resize(length);
        if (read(&str[0], 1, length) != length)
        {
            return std::string();
        }
    }
    
    return str;
}

bool BundleReader::readMatrix(float* m)
{
    return (read(m, sizeof(float), 16) == 16);
}

NS_JYE_END