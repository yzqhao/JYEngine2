#pragma once

#include "VertexStreamLayout.h"

#include <algorithm>
#include "Math/IntVec2.h"
#include "Math/Vec3.h"
#include "Core/Object.h"

NS_JYE_BEGIN

namespace VertexCompress
{
	typedef void (*VertexCompressFunction)(const void* input, byte* output, byte count, byte bytesize);
	static void SimpleCopy(const void* input, byte* output, byte count, byte bytesize)
	{
		memcpy(output, input, bytesize);
	}

	static void Float_HalfFloat(const void* input, byte* output, byte count, byte bytesize)
	{
		uint16* pdest = reinterpret_cast<uint16*>(output);
		for (int i = 0; i < count; i++)
		{
			uint src = *(unsigned long*)(&((float*)input)[i]);
			uint sign = src >> 31;
			// Extract mantissa
			uint mantissa = src & (1 << 23) - 1;
			// extract & convert exp bits
			long exp = (long)(src >> 23 & 0xFF) - 127;
			if (exp > 16)
			{
				// largest possible number...
				exp = 16;
				mantissa = (1 << 23) - 1;
			}
			else if (exp <= -15)
			{
				// handle wraparound of shifts (done mod 64 on PPC)
				// All float32 denorm/0 values map to float16 0
				if (exp <= -14 - 24)
					mantissa = 0;
				else
					mantissa = (mantissa | 1 << 23) >> (-exp - 14);
				exp = -15;
			}
			pdest[i] = (unsigned short)(sign << 15) | (unsigned short)((exp + 15) << 10) | (unsigned short)(mantissa >> 13);
		}
	}

	static void HalfFloat_Float(float* __restrict out, const uint16_t in) {
		uint32_t t1;
		uint32_t t2;
		uint32_t t3;

		t1 = in & 0x7fff;                       // Non-sign bits
		t2 = in & 0x8000;                       // Sign bit
		t3 = in & 0x7c00;                       // Exponent

		t1 <<= 13;                              // Align mantissa on MSB
		t2 <<= 16;                              // Shift sign bit into position

		t1 += 0x38000000;                       // Adjust bias

		t1 = (t3 == 0 ? 0 : t1);                // Denormals-as-zero

		t1 |= t2;                               // Re-insert sign bit

		*((uint32_t*)out) = t1;
	};

	static void Float_INT101010(const void* input, byte* output, byte count, byte bytesize)
	{
		ulong* pdest = reinterpret_cast<ulong*>(output);
		*pdest = 0;
		for (int i = 0; i < count; i++)
		{
			*pdest |= (((unsigned long)(((float*)input)[i] * 511.0f)) & 0x000003ff) << (i * 10);
		}
	}

	static void Float_UINT101010(const void* input, byte* output, byte count, byte bytesize)
	{
		ulong* pdest = reinterpret_cast<ulong*>(output);
		*pdest = 0;
		for (int i = 0; i < count; i++)
		{
			*pdest |= (((unsigned long)(((float*)input)[i] * 1023.0f)) & 0x000003ff) << (i * 10);
		}
	}

	static void Float_INT8888(const void* input, byte* output, byte count, byte bytesize)
	{
		ulong* pdest = reinterpret_cast<ulong*>(output);
		*pdest = 0;
		for (int i = 0; i < count; i++)
		{
			*pdest |= (((unsigned long)(((float*)input)[i] * 127.0f)) & 0x000000ff) << (i * 8);
		}
	}

	static void Float_UINT8888(const void* input, byte* output, byte count, byte bytesize)
	{
		ulong* pdest = reinterpret_cast<ulong*>(output);
		*pdest = 0;
		for (int i = 0; i < count; i++)
		{
			*pdest |= (((unsigned long)(((float*)input)[i] * 255.0f)) & 0x000000ff) << (i * 8);
		}
	}
}

class ENGINE_API VertexStream : public Object
{
	DECLARE_RTTI
	DECLARE_INITIAL
private:
	struct VertexPusher//数据压榨结构体
	{
		VertexCompress::VertexCompressFunction		CompressPtr;//压缩函数指针
		int											IndicesCount;//索引计数
		byte										PointerStride;//地址偏移的byte位
		byte										Count;
		byte										Bytesize;//每一个数据所含的byte个数
		VertexPusher() :CompressPtr(NULL), PointerStride(0), Count(0), Bytesize(0), IndicesCount(0) {}
		void ResetIndices()
		{
			IndicesCount = 0;
		}
	};

	typedef Vector<byte> Buffer;
	typedef Vector<VertexPusher*> VertexPusherVector;
	typedef Map<RHIDefine::ShaderAttribute, int>	IndexMapping;
private:
	VertexStreamLayout			m_Layout;//数据构成
	IndexMapping			m_IndexMapping;
	VertexPusherVector		m_VertexPusherVector;
	mutable Math::IntVec2   m_FlushInterval;//需要填充更新的区间
	unsigned				m_nReserveVertexCount;//预分配的顶点内存个数
	unsigned				m_nVertexCount;//顶点个数
	byte					m_nChunkStride;//每一组数据的大小
	Buffer					m_Buffer;
	unsigned				m_Flag;

	void _Reset();
	const VertexStream& _Copy(const VertexStream& rhs);
	void _SetVertexType(RHIDefine::ShaderAttribute sa, RHIDefine::DataType source, RHIDefine::DataType dest, int count);

	inline int _GetAttributeIndex(RHIDefine::ShaderAttribute att);
	inline void _DoPushVertexData(VertexPusher* pusher, const void* data);
	inline void _TrySetReflushInterval(int index);
	inline void* _GetBufferData();
	inline void* _GetBufferData(int vertexIndex);
public:
	enum 
	{
		V_QUAD_NUM = 4,  // 四边形顶点数目
	};
	VertexStream(void);
	VertexStream(const VertexStream& rhs);
	~VertexStream(void);

	const VertexStream& operator=(const VertexStream& rhs);

	void ReserveBuffer(int size);
	void Clear();
	void GetVerticesData(Vector<Math::Vec3>& vertices) const;
	void CopyVertexBuffer(uint vertexcount, uint vertexsize, const byte* buffer);

	template<typename T>
	void SetVertexType(RHIDefine::ShaderAttribute sa, RHIDefine::DataType source, RHIDefine::DataType dest)//生成数据压缩函数，数据类型等数据，设置输入
	{
		_SetVertexType(sa, source, dest, T::Size());
	}

	inline void SetVertexType(RHIDefine::ShaderAttribute sa, int count);
	inline void SetVertexType(RHIDefine::ShaderAttribute sa, RHIDefine::DataType source, RHIDefine::DataType dest, int count);
	inline void PushVertexData(RHIDefine::ShaderAttribute sa, const float* data);
	inline void PushVertexDataFast(RHIDefine::ShaderAttribute sa, const float* data);
	inline void ChangeVertexData(RHIDefine::ShaderAttribute sa, int index, const float* data);
	inline void ChangeVertexDataWithAttributeFast(int a, int index, const float* data);//使用GetAttributeIndex的结果作为第一个参数、并且调用SetReflushInterval设置范围
	inline int GetAttributeIndex(RHIDefine::ShaderAttribute sa);
	inline void ResetFlushInterval() const;
	inline const void* GetBufferData() const;
	inline const void* GetBufferData(int vertexIndex) const;
	inline const void* GetReflushBufferData() const;
	inline int GetVertexStride() const;
	inline int GetVertexSize() const;
	inline int GetVertexCount() const;
	inline int GetByteSize() const;
	inline const VertexStreamLayout& GetLayout() const;
	inline bool hasVertex(RHIDefine::ShaderAttribute sa) const;
	inline void SetReflushInterval(int begin, int end);
	inline const Math::IntVec2& GetReflushInterval() const;
	inline bool isValid() const;
	inline unsigned GetFlag() const;
	inline void SetFlag(unsigned flag);
};

inline void VertexStream::SetVertexType(RHIDefine::ShaderAttribute sa, int count)
{
	_SetVertexType(sa, RHIDefine::DT_FLOAT, RHIDefine::DT_FLOAT, count);
}

inline void VertexStream::SetVertexType(RHIDefine::ShaderAttribute sa, RHIDefine::DataType source, RHIDefine::DataType dest, int count)
{
	_SetVertexType(sa, source, dest, count);
}

inline unsigned VertexStream::GetFlag() const
{
	return m_Flag;
}

inline void VertexStream::SetFlag(unsigned flag)
{
	m_Flag = flag;
}

inline int VertexStream::_GetAttributeIndex(RHIDefine::ShaderAttribute att)
{
	IndexMapping::iterator it = m_IndexMapping.find(att);
	if (m_IndexMapping.end() == it)
	{
		int index = m_VertexPusherVector.size();
		m_VertexPusherVector.push_back(NULL);
		m_IndexMapping.insert({ att, index });
		return index;
	}
	return it->second;
}

inline void VertexStream::_DoPushVertexData(VertexPusher* pusher, const void* data)
{
	pusher->CompressPtr(data, &m_Buffer[pusher->IndicesCount * m_nChunkStride + pusher->PointerStride], pusher->Count, pusher->Bytesize);
	++pusher->IndicesCount;
	//m_nVertexCount = aem::Math::GetMax( m_nVertexCount, pusher->IndicesCount );//找到最大的数值，这个数值是当前顶点数量
	m_nVertexCount = pusher->IndicesCount;//非安全做法，可能面临数据丢失和缺失，可以在push的时候检测数据安全性
	m_FlushInterval.x = 0;
	m_FlushInterval.y = m_nVertexCount - 1;
}

inline void* VertexStream::_GetBufferData()
{
	return &m_Buffer[0];
}

inline void* VertexStream::_GetBufferData(int vertexIndex)
{
	return &m_Buffer[m_nChunkStride * vertexIndex];
}

inline void VertexStream::PushVertexData(RHIDefine::ShaderAttribute sa, const float* data)
{
	int i = _GetAttributeIndex(sa);
	VertexPusher* pusher = m_VertexPusherVector[i];
	if (pusher->IndicesCount >= m_nReserveVertexCount)
	{
		ReserveBuffer(m_nReserveVertexCount + 256);
	}
	_DoPushVertexData(pusher, data);
}
	
inline void VertexStream::PushVertexDataFast(RHIDefine::ShaderAttribute sa, const float* data)
{
	int i = _GetAttributeIndex(sa);
	VertexPusher* pusher = m_VertexPusherVector[i];
	_DoPushVertexData(pusher, data);
}

inline void VertexStream::ChangeVertexData(RHIDefine::ShaderAttribute sa, int index, const float* data)
{
	int i = _GetAttributeIndex(sa);
	_TrySetReflushInterval(index);
	VertexPusher* pusher = m_VertexPusherVector[i];
	pusher->CompressPtr(data, &m_Buffer[index * m_nChunkStride + pusher->PointerStride], pusher->Count, pusher->Bytesize);
}
	
inline int VertexStream::GetAttributeIndex(RHIDefine::ShaderAttribute sa)
{
	return _GetAttributeIndex(sa);
}
	
inline void VertexStream::ChangeVertexDataWithAttributeFast(int a, int index, const float* data)
{
	VertexPusher* pusher = m_VertexPusherVector[a];
	pusher->CompressPtr(data, &m_Buffer[index * m_nChunkStride + pusher->PointerStride], pusher->Count, pusher->Bytesize);
}
	
inline void VertexStream::ResetFlushInterval() const
{
	m_FlushInterval = Math::IntVec2(-1, -1);
}
	
inline const void* VertexStream::GetBufferData() const
{
	return m_Buffer.data();
}
	
inline const void* VertexStream::GetBufferData(int vertexIndex) const
{
	return &m_Buffer[m_nChunkStride * vertexIndex];
}
	
inline const void* VertexStream::GetReflushBufferData() const
{
	return &m_Buffer[m_nChunkStride * m_FlushInterval.x];
}
	
inline int VertexStream::GetVertexStride() const
{
	return m_nChunkStride;
}
	
inline int VertexStream::GetVertexSize() const
{
	return m_nChunkStride;
}
	
inline int VertexStream::GetVertexCount() const
{
	return m_nVertexCount;
}
	
inline int VertexStream::GetByteSize() const
{
	return m_nVertexCount * m_nChunkStride;
}
	
inline const VertexStreamLayout& VertexStream::GetLayout() const
{
	return m_Layout;
}
	
inline bool VertexStream::hasVertex(RHIDefine::ShaderAttribute sa) const
{
	return m_IndexMapping.end() != m_IndexMapping.find(sa);
}
	
inline void VertexStream::SetReflushInterval(int begin, int end)
{
	m_FlushInterval.x = begin;
	m_FlushInterval.y = end;
	m_nVertexCount = (std::max)((int)m_nVertexCount, (int)m_FlushInterval.y + 1);
}
	
inline const Math::IntVec2& VertexStream::GetReflushInterval() const
{
	return m_FlushInterval;
}
	
inline bool VertexStream::isValid() const
{
	return m_FlushInterval.x != m_FlushInterval.y;
}

inline void VertexStream::_TrySetReflushInterval(int index)
{
	m_FlushInterval.x = m_FlushInterval.x > index || -1 == m_FlushInterval.x ? index : m_FlushInterval.x;
	m_FlushInterval.y = m_FlushInterval.y < index || -1 == m_FlushInterval.y ? index : m_FlushInterval.y;
}

NS_JYE_END