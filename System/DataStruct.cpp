
#include "DataStruct.h"

NS_JYE_BEGIN

String NAME_None = "";

template<class T>
bool IsVectorIndexValid(const Vector<T>& vec, int idx)
{
	return (idx >= 0 && idx < vec.size());
}

static String VFormat(const char* format, va_list ap)
{
	va_list zp;
	va_copy(zp, ap);
	char* buffer = new char[1024 * 1024];
	vsnprintf(buffer, 1024 * 1024, format, zp);
	va_end(zp);
	String res(buffer);
	delete[] buffer;
	return res;
}

String StringFormat(const char* format, ...)
{
	va_list va;
	va_start(va, format);
	std::string formatted = VFormat(format, va);
	va_end(va);
	return formatted;
}

NS_JYE_END
