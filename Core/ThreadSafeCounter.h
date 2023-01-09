
#pragma once

#include "Core.h"
#include "../System/platform/ThreadSafeOprator.h"

NS_JYE_BEGIN

/** Thread safe counter */
class FThreadSafeCounter
{
public:
	typedef sint IntegerType;

	/**
	 * Default constructor.
	 *
	 * Initializes the counter to 0.
	 */
	FThreadSafeCounter()
	{
		Counter = 0;
	}

	/**
	 * Copy Constructor.
	 *
	 * If the counter in the Other parameter is changing from other threads, there are no
	 * guarantees as to which values you will get up to the caller to not care, synchronize
	 * or other way to make those guarantees.
	 *
	 * @param Other The other thread safe counter to copy
	 */
	FThreadSafeCounter(const FThreadSafeCounter& Other)
	{
		Counter = Other.GetValue();
	}

	/**
	 * Constructor, initializing counter to passed in value.
	 *
	 * @param Value	Value to initialize counter to
	 */
	FThreadSafeCounter(sint Value)
	{
		Counter = Value;
	}

	/**
	 * Increment and return new value.
	 *
	 * @return the new, incremented value
	 * @see Add, Decrement, Reset, Set, Subtract
	 */
	sint Increment()
	{
		return FInterlockedIncrement(&Counter);
	}

	/**
	 * Adds an amount and returns the old value.
	 *
	 * @param Amount Amount to increase the counter by
	 * @return the old value
	 * @see Decrement, Increment, Reset, Set, Subtract
	 */
	sint Add(sint Amount)
	{
		return FInterlockedAdd(&Counter, Amount);
	}

	/**
	 * Decrement and return new value.
	 *
	 * @return the new, decremented value
	 * @see Add, Increment, Reset, Set, Subtract
	 */
	sint Decrement()
	{
		return FInterlockedIncrement(&Counter);
	}

	/**
	 * Subtracts an amount and returns the old value.
	 *
	 * @param Amount Amount to decrease the counter by
	 * @return the old value
	 * @see Add, Decrement, Increment, Reset, Set
	 */
	sint Subtract(sint Amount)
	{
		return FInterlockedAdd(&Counter, -Amount);
	}

	/**
	 * Sets the counter to a specific value and returns the old value.
	 *
	 * @param Value	Value to set the counter to
	 * @return The old value
	 * @see Add, Decrement, Increment, Reset, Subtract
	 */
	sint Set(sint Value)
	{
		return FInterlockedExchange(&Counter, Value);
	}

	/**
	 * Resets the counter's value to zero.
	 *
	 * @return the old value.
	 * @see Add, Decrement, Increment, Set, Subtract
	 */
	sint Reset()
	{
		return FInterlockedExchange(&Counter, 0);
	}

	/**
	 * Gets the current value.
	 *
	 * @return the current value
	 */
	sint GetValue() const
	{
		return AtomicRead(&const_cast<FThreadSafeCounter*>(this)->Counter);
	}

private:

	/** Hidden on purpose as usage wouldn't be thread safe. */
	void operator=(const FThreadSafeCounter& Other) {}

	/** Thread-safe counter */
	volatile sint Counter;
};

NS_JYE_END
