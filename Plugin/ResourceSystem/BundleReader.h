
#pragma once

#include "private/Define.h"

#include <string>
#include <vector>

NS_JYE_BEGIN

class BundleReader
{
public:

    BundleReader();
    ~BundleReader();

    void init(char* buffer, uint length);

    uint read(void* ptr, uint size, uint count);

    char* readLine(int num, char* line);

    bool eof();

    uint length();

    uint tell();

    bool seek(long int offset, int origin);

    bool rewind();

    template<typename T> bool read(T* ptr);
    template<typename T> bool readArray(unsigned int* length, std::vector<T>* values);

    std::string readString();

    bool readMatrix(float* m);

private:
    uint  m_position;
    uint  m_length;
    char* m_buffer;
};


template<typename T>
inline bool BundleReader::read(T *ptr)
{
    return (read(ptr, sizeof(T), 1) == 1);
}


template<typename T>
inline bool BundleReader::readArray(unsigned int *length, std::vector<T> *values)
{
    if (!read(length))
    {
        return false;
    }
    
    if (*length > 0 && values)
    {
        values->resize(*length);
        if (read(&(*values)[0], sizeof(T), *length) != *length)
        {
            return false;
        }
    }
    return true;
}

template<>
inline bool BundleReader::read<char>(char *ptr)
{
    if (read(ptr, sizeof(char), 1) == 1)
    {
        return true;
    }
    else
    {
        *ptr = -1;
        return false;
    }
}

template<>
inline bool BundleReader::read<std::string>(std::string* /*ptr*/)
{
    return false;
}

/**
* template function to read array of value.
*/
template<>
inline bool BundleReader::readArray<std::string>(unsigned int *length, std::vector<std::string> *values)
{
    if (!read(length))
    {
        return false;
    }
    values->clear();
    if (*length > 0 && values)
    {
        for (int i = 0; i < (int)*length; ++i)
        {
            values->push_back(readString());
        }
    }
    return true;
}

NS_JYE_END