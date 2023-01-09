
#pragma once

#include "../System/System.h"

#define NS_JYE_MATH_BEGIN                     namespace JYE { namespace Math {
#define NS_JYE_MATH_END                       }}

#ifdef _PLATFORM_WINDOWS
#ifdef MATH_EXPORTS
    #define MATH_API     __declspec(dllexport)
#else
    #define MATH_API     __declspec(dllimport)
#endif
#else
    #define MATH_API
#endif

NS_JYE_MATH_BEGIN

static const float PI = 3.14159265358979323846264338327950288f;
static const float M_HALF_PI = PI * 0.5f;
static const int M_MIN_INT = 0x80000000;
static const int M_MAX_INT = 0x7fffffff;
static const unsigned M_MIN_UNSIGNED = 0x00000000;
static const unsigned M_MAX_UNSIGNED = 0xffffffff;
static const float M_INFINITY = (float)HUGE_VAL;
static const float M_EPSILON = 0.000001f;
static const float M_LARGE_EPSILON = 0.00005f;

#define SQUARE(x) (x*x)
#define MATH_DEG_TO_RAD(x)          ((x) * 0.0174532925f)
#define MATH_RAD_TO_DEG(x)          ((x)* 57.29577951f)
#define BIT(i) (1 << i)

template <class T>
static FORCEINLINE bool Equals(T lhs, T rhs) { return lhs + std::numeric_limits<T>::epsilon() >= rhs && lhs - std::numeric_limits<T>::epsilon() <= rhs; }

template <class T>
static FORCEINLINE T ABS(T t) { return t < 0 ? -t : t; }

template <class T, class U>
static FORCEINLINE T Lerp(T lhs, T rhs, U t) { return lhs * (1.0 - t) + rhs * t; }

template <class T, class U>
static FORCEINLINE T Min(T lhs, U rhs) { return lhs < rhs ? lhs : rhs; }

template <class T, class U>
static FORCEINLINE T Max(T lhs, U rhs) { return lhs > rhs ? lhs : rhs; }

template <class T>
static FORCEINLINE T Square(T t) { return t * t; }

template <typename T>
FORCEINLINE T Sin(const T& x) { return sin(x); }

template <typename T>
FORCEINLINE T Cos(const T& x) { return cos(x); }

template<typename T>
FORCEINLINE void SinCos(const T& x, T o[2])
{
	o[0] = Sin(x);
	o[1] = Cos(x);
}

template <typename T>
FORCEINLINE T Tan(const T& x) { return tan(x); }

template <typename T>
FORCEINLINE T ASin(const T& x) { return asin(x); }

template <typename T>
FORCEINLINE T ACos(const T& x) { return acos(x); }

template <typename T>
FORCEINLINE T ATan(const T& x) { return atan(x); }

template <typename T>
FORCEINLINE T Sqrt(const T& x) { return sqrt(x); }

template <typename T>
FORCEINLINE T Invsqrt(const T& x) { return 1.0f / Sqrt(x); }

static FORCEINLINE int RoundToInt(float t) { return (int)(t + 0.5f); }

/** Returns highest of 3 values */
template< class T >
static FORCEINLINE T Max3(const T A, const T B, const T C) { return Max(Max(A, B), C); }

template< class T >
static FORCEINLINE T Min3(const T A, const T B, const T C) { return Min(Min(A, B), C); }

inline int Rand() { return rand(); }
inline void RandInit(int Seed) { srand(Seed); }
inline float FRand() { return Rand() / (float)RAND_MAX; }

template <class T>
static void Swap(T a, T b)
{
	T temp;  temp = a, a = b, b = temp;
}

template <class T>
static FORCEINLINE T Clamp(T value, T min, T max)
{
	if (value < min)
		return min;
	else if (value > max)
		return max;
	else
		return value;
}

/** Divides two integers and rounds up */
template <class T>
static FORCEINLINE T DivideAndRoundUp(T Dividend, T Divisor)
{
	return (Dividend + Divisor - 1) / Divisor;
}

/** Divides two integers and rounds down */
template <class T>
static FORCEINLINE T DivideAndRoundDown(T Dividend, T Divisor)
{
	return Dividend / Divisor;
}

/** Divides two integers and rounds to nearest */
template <class T>
static FORCEINLINE T DivideAndRoundNearest(T Dividend, T Divisor)
{
	return (Dividend >= 0)
		? (Dividend + Divisor / 2) / Divisor
		: (Dividend - Divisor / 2 + 1) / Divisor;
}

static FORCEINLINE bool IsTwoPower(unsigned int uiN) { return !(uiN & (uiN - 1)); }

static FORCEINLINE int FastLog2(int x)
{
	float fx;
	unsigned int ix, exp;
	fx = (float)x;
	ix = *(unsigned int*)&fx;
	exp = (ix >> 23) & 0xFF;
	return exp - 127;
}

inline unsigned int FloorLog2(unsigned int Value)
{
	unsigned int pos = 0;
	if (Value >= 1 << 16) { Value >>= 16; pos += 16; }
	if (Value >= 1 << 8) { Value >>= 8; pos += 8; }
	if (Value >= 1 << 4) { Value >>= 4; pos += 4; }
	if (Value >= 1 << 2) { Value >>= 2; pos += 2; }
	if (Value >= 1 << 1) { pos += 1; }
	return (Value == 0) ? 0 : pos;
}

inline unsigned int CountLeadingZeros(unsigned int Value)
{
	if (Value == 0) return 32;
	return 31 - FloorLog2(Value);
}

inline unsigned int CeilLogTwo(unsigned int Arg)
{
	unsigned int Bitmask = ((unsigned int)(CountLeadingZeros(Arg) << 26)) >> 31;
	return (32 - CountLeadingZeros(Arg - 1)) & (~Bitmask);
}

/** @return Rounds the given number up to the next highest power of two. */
static FORCEINLINE uint RoundUpToPowerOfTwo(uint Arg)
{
	return 1 << CeilLogTwo(Arg);
}

#define JY_FRONT		0
#define JY_BACK			1
#define JY_ON			2

#define JY_CLIPPED		3
#define JY_CULLED		4
#define JY_VISIBLE		5

#define JY_INTERSECT	3
#define JY_OUT			4
#define JY_IN			5

class MATH_API MathInstance
{
public:
	float GetFastSin(unsigned int i);
	float GetFastCos(unsigned int i);
	uint CRC32Compute(const void* pData, uint uiDataSize);
	uint CRC32Compute(const void* InData, int Length, uint CRC);
	static MathInstance& GetMathInstance()
	{
		static MathInstance g_MathInitial;
		return g_MathInitial;
	}
protected:
	MathInstance();

	void VSInitCRCTable();
	float FastSin[361];
	float FastCos[361];
	unsigned int CRCTable[256];
};

NS_JYE_MATH_END

