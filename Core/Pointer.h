
#pragma once

#include "Core.h"

NS_JYE_BEGIN

template <class T>
class Pointer
{
	friend class Stream;
public:
	Pointer (T* pObject = 0);
	Pointer (const Pointer& rPointer);
	~Pointer ();
	
	T& operator* () const { return *m_pObject; }
	T* operator-> () const { return m_pObject; }
	
	Pointer& operator= (T* pObject);
	Pointer& operator= (const Pointer& rReference);
	
	bool operator== (T* pObject) const { return (m_pObject == pObject); }
	bool operator!= (T* pObject) const { return (m_pObject != pObject); }
	bool operator== (const Pointer& rPointer) const { return (m_pObject == rPointer.m_pObject); }
	bool operator!= (const Pointer& rPointer) const { return (m_pObject != rPointer.m_pObject); }

	bool operator > (T* pObject) const { return (ComparePointer(m_pObject, pObject) > 0); }
	bool operator < (T* pObject) const { return (ComparePointer(m_pObject, pObject) < 0); }
	bool operator > (const Pointer& rPointer) const { return (ComparePointer(m_pObject, rPointer.m_pObject) > 0); }
	bool operator < (const Pointer& rPointer) const { return (ComparePointer(m_pObject, rPointer.m_pObject) < 0); }

	operator T*() const { return m_pObject; }
	T* GetEObject() const { return m_pObject; }
	T** GetObjectAddress() { return &m_pObject; }
	void SafeRelease() { *this = nullptr; }

	friend bool IsValidRef(const Pointer& InReference) { return InReference.m_pObject != nullptr; }

private:
	T* m_pObject;
	
	inline void SetObject(T* pObject) { m_pObject = pObject; }
};

template <class T>
Pointer<T>::Pointer(T* pObject)
{
	m_pObject = pObject;
	if (m_pObject)
	{
		m_pObject->AddRef();
	}
}

template <class T>
Pointer<T>::Pointer(const Pointer& rPointer)
{
	m_pObject = rPointer.m_pObject;
	if (m_pObject)
	{
		m_pObject->AddRef();
	}
}

template <class T>
Pointer<T>::~Pointer()
{
	if (m_pObject)
	{
		m_pObject->Release();
		m_pObject = nullptr;
	}
}

template <class T>
Pointer<T>& Pointer<T>::operator= (T* pObject)
{
	if (m_pObject != pObject)
	{
		if (pObject)
		{
			pObject->AddRef();
		}

		if (m_pObject)
		{
			m_pObject->Release();
		}

		m_pObject = pObject;
	}
	return *this;
}

template <class T>
Pointer<T>& Pointer<T>::operator= (const Pointer& rPointer)
{
	if (m_pObject != rPointer.m_pObject)
	{
		if (rPointer.m_pObject)
		{
			rPointer.m_pObject->AddRef();
		}

		if (m_pObject)
		{
			m_pObject->Release();
		}

		m_pObject = rPointer.m_pObject;
	}
	return *this;
}

#define DECLARE_Ptr(ClassName)\
	class ClassName;\
	typedef Pointer<ClassName> ClassName##Ptr;

NS_JYE_END
