#ifndef THREADSAFEOPRATOR
#error( "Do not include this file in other files" )
#endif

#include <xmmintrin.h>

namespace JYE
{

static int Increment(volatile int* var)
{
	return _InterlockedIncrement((volatile long*)var);
}

static int Decrement(volatile int* var)
{
	return _InterlockedDecrement((volatile long*)var);
}

static char FInterlockedIncrement(volatile char* Value)
{
	return (char)_InterlockedExchangeAdd8((char*)Value, 1) + 1;
}

static short FInterlockedIncrement(volatile short* Value)
{
	return (short)_InterlockedIncrement16((short*)Value);
}

static sint FInterlockedIncrement(volatile sint* Value)
{
	return (sint)_InterlockedIncrement((long*)Value);
}

static int64 FInterlockedIncrement(volatile int64* Value)
{
#if PLATFORM_64BITS
	return (int64)::_InterlockedIncrement64((long long*)Value);
#else
	// No explicit instruction for 64-bit atomic increment on 32-bit processors; has to be implemented in terms of CMPXCHG8B
	for (;;)
	{
		int64 OldValue = *Value;
		if (_InterlockedCompareExchange64(Value, OldValue + 1, OldValue) == OldValue)
		{
			return OldValue + 1;
		}
	}
#endif
}

static char InterlockedDecrement(volatile char* Value)
{
	return (char)::_InterlockedExchangeAdd8((char*)Value, -1) - 1;
}

static short InterlockedDecrement(volatile short* Value)
{
	return (short)::_InterlockedDecrement16((short*)Value);
}

static sint InterlockedDecrement(volatile sint* Value)
{
	return (sint)::_InterlockedDecrement((long*)Value);
}

static int64 InterlockedDecrement(volatile int64* Value)
{
#if PLATFORM_64BITS
	return (int64)::_InterlockedDecrement64((long long*)Value);
#else
	// No explicit instruction for 64-bit atomic decrement on 32-bit processors; has to be implemented in terms of CMPXCHG8B
	for (;;)
	{
		int64 OldValue = *Value;
		if (_InterlockedCompareExchange64(Value, OldValue - 1, OldValue) == OldValue)
		{
			return OldValue - 1;
		}
	}
#endif
}

static char InterlockedAdd(volatile char* Value, char Amount)
{
	return (char)::_InterlockedExchangeAdd8((char*)Value, (char)Amount);
}

static short InterlockedAdd(volatile short* Value, short Amount)
{
	return (short)::_InterlockedExchangeAdd16((short*)Value, (short)Amount);
}

static sint InterlockedAdd(volatile sint* Value, sint Amount)
{
	return (sint)::_InterlockedExchangeAdd((long*)Value, (long)Amount);
}

static int64 InterlockedAdd(volatile int64* Value, int64 Amount)
{
#if PLATFORM_64BITS
	return (int64)::_InterlockedExchangeAdd64((int64*)Value, (int64)Amount);
#else
	// No explicit instruction for 64-bit atomic add on 32-bit processors; has to be implemented in terms of CMPXCHG8B
	for (;;)
	{
		int64 OldValue = *Value;
		if (_InterlockedCompareExchange64(Value, OldValue + Amount, OldValue) == OldValue)
		{
			return OldValue;
		}
	}
#endif
}

static char FInterlockedExchange(volatile char* Value, char Exchange)
{
	return (char)::_InterlockedExchange8((char*)Value, (char)Exchange);
}

static short FInterlockedExchange(volatile short* Value, short Exchange)
{
	return (short)::_InterlockedExchange16((short*)Value, (short)Exchange);
}

static sint FInterlockedExchange(volatile sint* Value, sint Exchange)
{
	return (sint)::_InterlockedExchange((long*)Value, (long)Exchange);
}

static int64 FInterlockedExchange(volatile int64* Value, int64 Exchange)
{
#if PLATFORM_64BITS
	return (int64)::_InterlockedExchange64((long long*)Value, (long long)Exchange);
#else
	// No explicit instruction for 64-bit atomic exchange on 32-bit processors; has to be implemented in terms of CMPXCHG8B
	for (;;)
	{
		int64 OldValue = *Value;
		if (_InterlockedCompareExchange64(Value, Exchange, OldValue) == OldValue)
		{
			return OldValue;
		}
	}
#endif
}

static char FInterlockedCompareExchange(volatile char* Dest, char Exchange, char Comparand)
{
	return (char)::_InterlockedCompareExchange8((char*)Dest, (char)Exchange, (char)Comparand);
}

static short FInterlockedCompareExchange(volatile short* Dest, short Exchange, short Comparand)
{
	return (short)::_InterlockedCompareExchange16((short*)Dest, (short)Exchange, (short)Comparand);
}

static sint FInterlockedCompareExchange(volatile sint* Dest, sint Exchange, sint Comparand)
{
	return (sint)::_InterlockedCompareExchange((long*)Dest, (long)Exchange, (long)Comparand);
}

static int64 FInterlockedCompareExchange(volatile int64* Dest, int64 Exchange, int64 Comparand)
{
	return (int64)::_InterlockedCompareExchange64(Dest, Exchange, Comparand);
}

static int FInterlockedAdd(volatile int* Value, int Amount)
{
	int RetVal;
	do
	{
		RetVal = *Value;
	} while (FInterlockedCompareExchange((int*)Value, RetVal + Amount, RetVal) != RetVal);
	return RetVal;
}

static char InterlockedAnd(volatile char* Value, const char AndValue)
{
	return (char)::_InterlockedAnd8((volatile char*)Value, (char)AndValue);
}

static short InterlockedAnd(volatile short* Value, const short AndValue)
{
	return (short)::_InterlockedAnd16((volatile short*)Value, (short)AndValue);
}

static sint InterlockedAnd(volatile sint* Value, const sint AndValue)
{
	return (sint)::_InterlockedAnd((volatile long*)Value, (long)AndValue);
}

static int64 InterlockedAnd(volatile int64* Value, const int64 AndValue)
{
#if PLATFORM_64BITS
	return (int64)::_InterlockedAnd64((volatile long long*)Value, (long long)AndValue);
#else
	// No explicit instruction for 64-bit atomic and on 32-bit processors; has to be implemented in terms of CMPXCHG8B
	for (;;)
	{
		const int64 OldValue = *Value;
		if (_InterlockedCompareExchange64(Value, OldValue & AndValue, OldValue) == OldValue)
		{
			return OldValue;
		}
	}
#endif
}

static char InterlockedOr(volatile char* Value, const char OrValue)
{
	return (char)::_InterlockedOr8((volatile char*)Value, (char)OrValue);
}

static short InterlockedOr(volatile short* Value, const short OrValue)
{
	return (short)::_InterlockedOr16((volatile short*)Value, (short)OrValue);
}

static sint InterlockedOr(volatile sint* Value, const sint OrValue)
{
	return (sint)::_InterlockedOr((volatile long*)Value, (long)OrValue);
}

static int64 InterlockedOr(volatile int64* Value, const int64 OrValue)
{
#if PLATFORM_64BITS
	return (int64)::_InterlockedOr64((volatile long long*)Value, (long long)OrValue);
#else
	// No explicit instruction for 64-bit atomic or on 32-bit processors; has to be implemented in terms of CMPXCHG8B
	for (;;)
	{
		const int64 OldValue = *Value;
		if (_InterlockedCompareExchange64(Value, OldValue | OrValue, OldValue) == OldValue)
		{
			return OldValue;
		}
	}
#endif
}

static char InterlockedXor(volatile char* Value, const char XorValue)
{
	return (char)::_InterlockedXor8((volatile char*)Value, (char)XorValue);
}

static short InterlockedXor(volatile short* Value, const short XorValue)
{
	return (short)::_InterlockedXor16((volatile short*)Value, (short)XorValue);
}

static sint InterlockedXor(volatile sint* Value, const sint XorValue)
{
	return (sint)::_InterlockedXor((volatile long*)Value, (sint)XorValue);
}

static int64 InterlockedXor(volatile int64* Value, const int64 XorValue)
{
#if PLATFORM_64BITS
	return (int64)::_InterlockedXor64((volatile long long*)Value, (long long)XorValue);
#else
	// No explicit instruction for 64-bit atomic xor on 32-bit processors; has to be implemented in terms of CMPXCHG8B
	for (;;)
	{
		const int64 OldValue = *Value;
		if (_InterlockedCompareExchange64(Value, OldValue ^ XorValue, OldValue) == OldValue)
		{
			return OldValue;
		}
	}
#endif
}

static char AtomicRead(volatile const char* Src)
{
	return FInterlockedCompareExchange((char*)Src, 0, 0);
}

static short AtomicRead(volatile const short* Src)
{
	return FInterlockedCompareExchange((short*)Src, 0, 0);
}

static sint AtomicRead(volatile const sint* Src)
{
	return FInterlockedCompareExchange((sint*)Src, 0, 0);
}

static int64 AtomicRead(volatile const int64* Src)
{
	return FInterlockedCompareExchange((int64*)Src, 0, 0);
}

static char AtomicRead_Relaxed(volatile const char* Src)
{
	return *Src;
}

static short AtomicRead_Relaxed(volatile const short* Src)
{
	return *Src;
}

static sint AtomicRead_Relaxed(volatile const sint* Src)
{
	return *Src;
}

static int64 AtomicRead_Relaxed(volatile const int64* Src)
{
#if PLATFORM_64BITS
	return *Src;
#else
	return InterlockedCompareExchange((volatile int64*)Src, 0, 0);
#endif
}

static void AtomicStore(volatile char* Src, char Val)
{
	FInterlockedExchange(Src, Val);
}

static void AtomicStore(volatile short* Src, short Val)
{
	FInterlockedExchange(Src, Val);
}

static void AtomicStore(volatile sint* Src, sint Val)
{
	FInterlockedExchange(Src, Val);
}

static void AtomicStore(volatile int64* Src, int64 Val)
{
	FInterlockedExchange(Src, Val);
}

static void AtomicStore_Relaxed(volatile char* Src, char Val)
{
	*Src = Val;
}

static void AtomicStore_Relaxed(volatile short* Src, short Val)
{
	*Src = Val;
}

static void AtomicStore_Relaxed(volatile sint* Src, sint Val)
{
	*Src = Val;
}

static void AtomicStore_Relaxed(volatile int64* Src, int64 Val)
{
#if PLATFORM_64BITS
	* Src = Val;
#else
	FInterlockedExchange(Src, Val);
#endif
}

static void MemoryBarrier() { _mm_sfence(); }

}