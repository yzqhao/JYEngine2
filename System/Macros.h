
#pragma once

#include "platform/PlatformConfig.h"

#include <cassert>

#pragma warning(disable: 4251)
#pragma warning(disable: 4275)

#define NS_JYE_BEGIN                     namespace JYE {
#define NS_JYE_END                       }
#define USING_JYE_CC                     using namespace JYE;

NS_JYE_BEGIN

/**
 * utility template for a class that should not be copyable.
 * Derive from this class to make your class non-copyable
 */
class FNoncopyable
{
protected:
	// ensure the class cannot be constructed directly
	FNoncopyable() {}
	// the class should not be used polymorphically
	~FNoncopyable() {}
private:
	FNoncopyable(const FNoncopyable&);
	FNoncopyable& operator=(const FNoncopyable&);
};

struct LogCategory 
{
	LogCategory(const std::string& name) { sTagName = (name); }
	std::string sTagName;
};

#define DECLARE_LOG_TAG(CategoryName) \
	extern struct FLogCategory##CategoryName : public LogCategory \
	{ \
		FLogCategory##CategoryName() : LogCategory((#CategoryName)) {} \
	} CategoryName;

#define DEFINE_LOG_TAG(CategoryName) FLogCategory##CategoryName CategoryName;

#ifdef _DEBUG
#define JY_ASSERT(cond)    assert(cond)
#define JY_CHECK(cond, msg) \
	{\
		if (!cond) {\
			 JYERROR(msg); assert(0); \
		}\
	}
#else
#define JY_ASSERT(cond)     cond
#define JY_CHECK(cond, msg) cond
#endif

#ifdef PLATFORM_WIN32
#define FORCEINLINE __forceinline
#else
#define FORCEINLINE __attribute__((always_inline)) inline
#endif

#define SAFE_DELETE(p) if(p){delete p; p = NULL;}
#define SAFE_DELETE_ARRAY(p) if(p){delete[] p; p = NULL;}

#define _NEW new
#define _DELETE delete

#ifndef NOMINMAX	// max and std::max conflict
   #define NOMINMAX
#endif

typedef unsigned short		uint16;
typedef int					int32;
typedef unsigned int		uint;
typedef unsigned long		ulong;
typedef unsigned char		byte;
typedef unsigned char		uint8;
typedef unsigned int		bitmask;
typedef int					sint;
typedef unsigned int		handle;
#ifdef _WIN32
typedef __int64				int64;
typedef unsigned __int64	uint64;
#else
typedef long long			int64;
typedef unsigned long long	uint64;
#endif

#define MIN_uint8		((byte)	0x00)
#define	MIN_uint16		((ushort)	0x0000)
#define	MIN_uint32		((uint)	0x00000000)
#define MIN_uint64		((uint64)	0x0000000000000000)
#define MIN_int8		((char)		-128)
#define MIN_int16		((short)	-32768)
#define MIN_int32		((sint)	0x80000000)
#define MIN_int64		((int64)	0x8000000000000000)

#define MAX_uint8		((byte)	0xff)
#define MAX_uint16		((ushort)	0xffff)
#define MAX_uint32		((uint)	0xffffffff)
#define MAX_uint64		((uint64)	0xffffffffffffffff)
#define MAX_int8		((char)		0x7f)
#define MAX_int16		((short)	0x7fff)
#define MAX_int32		((sint)	0x7fffffff)
#define MAX_int64		((int64)	0x7fffffffffffffff)

#define MIN_flt			(1.175494351e-38F)			/* min positive value */
#define MAX_flt			(3.402823466e+38F)
#define MIN_dbl			(2.2250738585072014e-308)	/* min positive value */
#define MAX_dbl			(1.7976931348623158e+308)	

#define nullhandle ((uint)-1)
#define nullid ((uint64)-1)

enum { INDEX_NONE = -1 };
enum { UNICODE_BOM = 0xfeff };

#ifdef __clang__
template <typename T>
auto ArrayCountHelper(T& t) -> typename TEnableIf<__is_array(T), char(&)[sizeof(t) / sizeof(t[0]) + 1]>::Type;
#else
template <typename T, uint N>
char(&ArrayCountHelper(const T(&)[N]))[N + 1];
#endif

// Number of elements in an array.
#define JY_ARRAY_COUNT( array ) (sizeof(ArrayCountHelper(array)) - 1)

enum EForceInit
{
	ForceInit,
	ForceInitToZero
};
enum ENoInit { NoInit };
enum EInPlace { InPlace };

#if PLATFORM_WIN32
#if _WIN64
#define SYSTEM_BIT_WIDTH 64
#define USIZE_TYPE unsigned long long
#define SIZE_TYPE long long
#else
#define SYSTEM_BIT_WIDTH 32
#define USIZE_TYPE unsigned int
#define SIZE_TYPE int
#endif
#else
#define SYSTEM_BIT_WIDTH 64
#define USIZE_TYPE unsigned long long
#define SIZE_TYPE long long
#endif

template< class T > inline T Align(const T Ptr, USIZE_TYPE Alignment)
{
	return (T)(((USIZE_TYPE)Ptr + Alignment - 1) & ~(Alignment - 1));
}
template< class T > inline T Align1(const T Ptr, USIZE_TYPE Alignment)
{
	return (T)((USIZE_TYPE)Ptr + Alignment - (Ptr & (Alignment - 1)));
}

NS_JYE_END