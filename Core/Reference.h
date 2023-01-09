
#pragma once

#include "Core.h"
#include "System/platform/ThreadSafeOprator.h"

NS_JYE_BEGIN

class IRefCountedObject
{
public:
	virtual ~IRefCountedObject() { }
	virtual void AddRef() const = 0;
	virtual void Release() const = 0;
	virtual uint GetRefCount() const = 0;
};

class CORE_API Reference
{
	template <class T>
	friend class Pointer;

public:
	Reference() {}
	virtual ~Reference() {}

	uint GetRef() const { return m_iReference; }

protected:

	void AddRef()
	{
		Increment((int*)&m_iReference);
	}

	void Release()
	{
		Decrement((int*)&m_iReference);
		if (!m_iReference)
			delete (this);
	}

	uint m_iReference = 0;
};

/**
 * The base class of reference counted objects.
 *
 * This class should not be used for new code as it does not use atomic operations to update
 * the reference count.
 *
 */
class CORE_API FRefCountedObject
{
public:
	FRefCountedObject() : NumRefs(0) {}
	virtual ~FRefCountedObject() { JY_ASSERT(!NumRefs); }
	uint AddRef() const
	{
		return uint(++NumRefs);
	}
	uint Release() const
	{
		uint Refs = uint(--NumRefs);
		if (Refs == 0)
		{
			delete this;
		}
		return Refs;
	}
	uint GetRefCount() const
	{
		return uint(NumRefs);
	}
private:
	mutable int32 NumRefs;
};

NS_JYE_END
