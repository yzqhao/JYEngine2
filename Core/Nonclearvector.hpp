#pragma once

#include "Core.h"

#include <vector>

NS_JYE_BEGIN

template<typename T>
class Nonclearvector
{
private:
	typedef std::vector<T> ClientVector;
public:
	typedef typename ClientVector::iterator					iterator;
	typedef typename ClientVector::const_iterator			const_iterator;
private:
	uint				m_Index;
	ClientVector		m_ClientVector;
public:
	Nonclearvector() :m_Index(0){}
	~Nonclearvector(){}
public:
	
	FORCEINLINE void reserve(uint size)
	{
		m_ClientVector.reserve(size);
	}
	
	FORCEINLINE uint size() const
	{
		return m_Index;
	}
	
	FORCEINLINE void push_back(const T& v)
	{
		if (m_Index < m_ClientVector.size())
		{
			m_ClientVector[m_Index] = v;
		}
		else
		{
			m_ClientVector.push_back(v);
		}
		++m_Index;
	}
	
	FORCEINLINE void clear()
	{
		m_Index = 0;
	}
	FORCEINLINE void erase(typename ClientVector::iterator it)
	{
		--m_Index;
		m_ClientVector.erase(it);
	}
	
	FORCEINLINE typename ClientVector::iterator begin()
	{
		return m_ClientVector.begin();
	}
	
	FORCEINLINE typename ClientVector::const_iterator begin() const
	{
		return m_ClientVector.begin();
	}
	
	FORCEINLINE typename ClientVector::iterator end()
	{
		typename ClientVector::iterator res = m_Index < m_ClientVector.size()
			? &m_ClientVector[m_Index]
			: m_ClientVector.end();
		return res;
	}
	
	FORCEINLINE typename ClientVector::const_iterator end() const
	{
		typename ClientVector::const_iterator res = m_Index < m_ClientVector.size()
			? &m_ClientVector[m_Index]
			: m_ClientVector.end();
		return res;
	}
	
	FORCEINLINE typename ClientVector::const_pointer data() const
	{
		return m_ClientVector.data();
	}
	
	FORCEINLINE typename ClientVector::reference front()
	{
		return m_ClientVector.front();
	}
	
	FORCEINLINE typename ClientVector::const_reference front() const
	{
		return m_ClientVector.front();
	}
	
	FORCEINLINE typename ClientVector::reference back()
	{
		return m_ClientVector.back();
	}
	
	FORCEINLINE typename ClientVector::const_reference back() const
	{
		return m_ClientVector.back();
	}
	
	FORCEINLINE typename ClientVector::reference operator [](uint index)
	{
		return m_ClientVector[index];
	}
	
	FORCEINLINE typename ClientVector::const_reference operator [](uint index) const
	{
		return m_ClientVector[index];
	}
};

NS_JYE_END