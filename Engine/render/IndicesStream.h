#pragma once

#include "RHI/RHIDefine.h"
#include "../private/Define.h"
#include "Core/Object.h"

NS_JYE_BEGIN

class ENGINE_API IndicesStream : public Object
{
	DECLARE_RTTI
	DECLARE_INITIAL
private:
	enum Constant
	{
		IC_RESERVE_BUFFER_SIZE = 512,
	};
private:
	Vector<byte>					m_DataVector;
	RHIDefine::IndicesType	m_eType;
	int								m_ReserveIndicesCount;
	int								m_IndicesCount;
	int								m_CopyBytes;
public:
	enum
	{
		I_QUAD_NUM = 6,  // 四边形索引数目
	};
	IndicesStream(void);
	IndicesStream(const IndicesStream& rhs);
	~IndicesStream(void);
private:
	void _Reset();
	const IndicesStream& _Copy(const IndicesStream& rhs);
public:
	const IndicesStream& operator=(const IndicesStream& rhs);
	void SetIndicesType(RHIDefine::IndicesType type);
	void ReserveBuffer(int size);
	void GetIndicesData(Vector<unsigned int>& indices) const;
	void FullMerge(IndicesStream& is, int startIdx, int mergeCount);
	void CopyIndicesBuffer(int indicecount, int indicesize, const byte* buffer);
public:
	inline void PushIndicesData(int data);
	inline void PushIndicesDataFast(int data);
	inline void SetIndicesCount(int count);
	inline RHIDefine::IndicesType GetIndicesType() const;
	inline const void* GetBuffer() const;
	inline int GetIndicesCount() const;
	inline int GetByteSize() const;
	inline int GetIndicesStride() const;
	inline void Clear();
};

inline void IndicesStream::PushIndicesData(int data)
{
	if (m_IndicesCount >= m_ReserveIndicesCount)
	{
		ReserveBuffer(m_ReserveIndicesCount + IC_RESERVE_BUFFER_SIZE);
	}
	PushIndicesDataFast(data);
}
	
inline void IndicesStream::PushIndicesDataFast(int data)
{
	memcpy(m_DataVector.data() + m_IndicesCount * m_CopyBytes, &data, m_CopyBytes);
	++m_IndicesCount;
}
	
inline void IndicesStream::SetIndicesCount(int count)  
{
	m_IndicesCount = count;
}
	
inline RHIDefine::IndicesType IndicesStream::GetIndicesType() const
{
	return m_eType;
}
	
inline const void* IndicesStream::GetBuffer() const
{
	return m_DataVector.data();
}
	
inline int IndicesStream::GetIndicesCount() const
{
	return m_IndicesCount;
}
	
inline int IndicesStream::GetByteSize() const
{
	return m_CopyBytes * m_IndicesCount;
}
	
inline int IndicesStream::GetIndicesStride() const
{
	return m_CopyBytes;
}
	
inline void IndicesStream::Clear()
{
	m_IndicesCount = 0;
	m_ReserveIndicesCount = 0;
	m_DataVector.clear();
}

NS_JYE_END
