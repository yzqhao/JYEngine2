
#pragma once

#include "Macros.h"

NS_JYE_BEGIN

/**
 * Combines two hash values to get a third.
 * Note - this function is not commutative.
 */
	inline uint HashCombine(uint A, uint C)
{
	uint B = 0x9e3779b9;
	A += B;

	A -= B; A -= C; A ^= (C >> 13);
	B -= C; B -= A; B ^= (A << 8);
	C -= A; C -= B; C ^= (B >> 13);
	A -= B; A -= C; A ^= (C >> 12);
	B -= C; B -= A; B ^= (A << 16);
	C -= A; C -= B; C ^= (B >> 5);
	A -= B; A -= C; A ^= (C >> 3);
	B -= C; B -= A; B ^= (A << 10);
	C -= A; C -= B; C ^= (B >> 15);

	return C;
}


inline uint PointerHash(const void* Key, uint C = 0)
{
	// Avoid LHS stalls on PS3 and Xbox 360
#if PLATFORM_64BITS
	// Ignoring the lower 4 bits since they are likely zero anyway.
	// Higher bits are more significant in 64 bit builds.
	auto PtrInt = reinterpret_cast<USIZE_TYPE>(Key) >> 4;
#else
	auto PtrInt = reinterpret_cast<USIZE_TYPE>(Key);
#endif

	return HashCombine((uint)PtrInt, C);
}


//
// Hash functions for common types.
//

inline uint GetTypeHash(const byte A)
{
	return A;
}

inline uint GetTypeHash(const char A)
{
	return A;
}

inline uint GetTypeHash(const uint16 A)
{
	return A;
}

inline uint GetTypeHash(const short A)
{
	return A;
}

inline uint GetTypeHash(const sint A)
{
	return A;
}

inline uint GetTypeHash(const uint A)
{
	return A;
}

inline uint GetTypeHash(const uint64 A)
{
	return (uint)A + ((uint)(A >> 32) * 23);
}

inline uint GetTypeHash(const int64 A)
{
	return (uint)A + ((uint)(A >> 32) * 23);
}

#if PLATFORM_MAC
inline uint GetTypeHash(const __uint128_t A)
{
	uint64 Low = (uint64)A;
	uint64 High = (uint64)(A >> 64);
	return GetTypeHash(Low) ^ GetTypeHash(High);
}
#endif

#if (PLATFORM_ANDROID && PLATFORM_64BITS) || defined(UINT64_T_IS_UNSIGNED_LONG)
//On Android 64bit, uint64_t is unsigned long, not unsigned long long (aka uint64). These types can't be automatically converted.
inline uint GetTypeHash(uint64_t A)
{
	return GetTypeHash((uint64)A);
}
#endif

inline uint GetTypeHash(float Value)
{
	return *(uint*)&Value;
}

inline uint GetTypeHash(double Value)
{
	return GetTypeHash(*(uint64*)&Value);
}

inline uint GetTypeHash(const void* A)
{
	return PointerHash(A);
}

inline uint GetTypeHash(void* A)
{
	return PointerHash(A);
}

NS_JYE_END
