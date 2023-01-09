#include "VertexStream.h"

NS_JYE_BEGIN

IMPLEMENT_RTTI(VertexStream, Object);
BEGIN_ADD_PROPERTY(VertexStream, Object);
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(VertexStream)
IMPLEMENT_INITIAL_END

VertexStream::VertexStream(void)
	:m_nReserveVertexCount(0)
	, m_nVertexCount(0)
	, m_nChunkStride(0)
	, m_Flag(0)
{
}
	
VertexStream::VertexStream(const VertexStream& rhs)
	:m_nReserveVertexCount(0)
	, m_nVertexCount(0)
	, m_nChunkStride(0)
	, m_Flag(0)
{
	_Copy(rhs);
}
	
VertexStream::~VertexStream(void)
{
	_Reset();
}
	
void VertexStream::ReserveBuffer(int size)
{
	if (m_nReserveVertexCount < size)
	{
		m_nReserveVertexCount = size;
		m_Buffer.resize(size * m_nChunkStride);
	}
}
	
void VertexStream::_Reset()
{
	m_nChunkStride = 0;
	m_nReserveVertexCount = 0;
	m_nVertexCount = 0;
	m_FlushInterval = Math::IntVec2(-1, -1);
	m_Buffer.clear();
	m_Layout.Clear();
	for(auto& pusher : m_VertexPusherVector)
	{
		delete (pusher);
	}
	m_IndexMapping.clear();
	m_VertexPusherVector.clear();
}
	
const VertexStream& VertexStream::_Copy(const VertexStream& rhs)
{
	if (this != &rhs)
	{
		_Reset();
		m_Layout = rhs.m_Layout;
		m_IndexMapping = rhs.m_IndexMapping;
		m_FlushInterval = rhs.m_FlushInterval;
		m_nReserveVertexCount = rhs.m_nReserveVertexCount;
		m_nVertexCount = rhs.m_nVertexCount;
		m_nChunkStride = rhs.m_nChunkStride;
		m_Buffer = rhs.m_Buffer;
		m_Flag = rhs.m_Flag;
		for (auto ivp : rhs.m_VertexPusherVector)
		{
			VertexPusher* nvp = new VertexPusher(*ivp);
			m_VertexPusherVector.push_back(nvp);
		}
	}
	return *this;
}

void VertexStream::_SetVertexType(RHIDefine::ShaderAttribute sa, RHIDefine::DataType source, RHIDefine::DataType dest, int count)
{
	// dx不支持halfrgb16类型，android平台也没有使用half类型，注释掉这个宏了
	switch (dest)
	{
	case RHIDefine::DT_FLOAT:
	case RHIDefine::DT_HALF_FLOAT:
	case RHIDefine::DT_INT_2_10_10_10:
	case RHIDefine::DT_UINT_10_10_10_2: dest = RHIDefine::DT_FLOAT;
	}

	int i = _GetAttributeIndex(sa);
	if (m_VertexPusherVector[i])
	{
		JYLOG("vertex slot %d is exist", sa);
	}
	else
	{
		assert(0 == GetVertexCount());
		byte layoutCount = 0;
		m_VertexPusherVector[i] = new VertexPusher;
		switch (source)
		{
		case RHIDefine::DT_FLOAT:
		{
			switch (dest)
			{
			case RHIDefine::DT_FLOAT: m_VertexPusherVector[i]->CompressPtr = &VertexCompress::SimpleCopy; layoutCount = count; break;
			case RHIDefine::DT_HALF_FLOAT: m_VertexPusherVector[i]->CompressPtr = &VertexCompress::Float_HalfFloat; layoutCount = count; break;
			case RHIDefine::DT_INT_8_8_8_8: m_VertexPusherVector[i]->CompressPtr = &VertexCompress::Float_INT8888; layoutCount = 4; break;
			case RHIDefine::DT_UINT_8_8_8_8: m_VertexPusherVector[i]->CompressPtr = &VertexCompress::Float_UINT8888; layoutCount = 4; break;
			case RHIDefine::DT_INT_2_10_10_10: m_VertexPusherVector[i]->CompressPtr = &VertexCompress::Float_INT101010; layoutCount = 4; break;
			case RHIDefine::DT_UINT_10_10_10_2: m_VertexPusherVector[i]->CompressPtr = &VertexCompress::Float_UINT101010; layoutCount = 4; break;
			default: JYLOG("Unkown vertex data type !");
			}
		}break;
		case RHIDefine::DT_HALF_FLOAT:
		{
			assert(source == dest);
			layoutCount = count;
			m_VertexPusherVector[i]->CompressPtr = &VertexCompress::SimpleCopy;
		}break;
		case RHIDefine::DT_INT_8_8_8_8:
		case RHIDefine::DT_UINT_8_8_8_8:
		case RHIDefine::DT_INT_2_10_10_10:
		case RHIDefine::DT_UINT_10_10_10_2:
		{
			assert(source == dest);
			layoutCount = 4;
			m_VertexPusherVector[i]->CompressPtr = &VertexCompress::SimpleCopy;
		}break;
		default: JYLOG("Unkown vertex data type !");
		}
		m_Layout.SetVertexLayout(sa, dest, layoutCount);
		m_VertexPusherVector[i]->PointerStride = m_nChunkStride;
		m_VertexPusherVector[i]->Count = count;
		m_VertexPusherVector[i]->Bytesize = m_Layout.GetVertexLayout(sa)->ByteSize();
		m_nChunkStride += m_Layout.GetVertexLayout(sa)->Stride();
	}
	if (0 != m_nReserveVertexCount)
	{
		m_Buffer.resize(m_nReserveVertexCount * m_nChunkStride);
	}
}

const VertexStream& VertexStream::operator=(const VertexStream& rhs)
{
	return _Copy(rhs);
}

void VertexStream::Clear()
{
	m_nReserveVertexCount = 0;
	m_nVertexCount = 0;
	m_FlushInterval = Math::IntVec2(-1, -1);
	m_Buffer.clear();
	for (int i = 0; i < m_VertexPusherVector.size(); i++)
	{
		m_VertexPusherVector[i]->ResetIndices();
	}
}

void VertexStream::GetVerticesData(Vector<Math::Vec3>& vertices) const
{
	const VertexStreamLayout::Layout* vertexLayout = m_Layout.GetVertexLayout(RHIDefine::PS_ATTRIBUTE_POSITION);
	byte offset = vertexLayout->Offset();
	int vertexSize = GetVertexSize();
	RHIDefine::DataType dt = vertexLayout->DataType();
	byte* bufferDat = (byte*)GetBufferData();
	int vertexCnt = GetVertexCount();

	vertices.reserve(vertexCnt);
	for (int i = 0; i < vertexCnt; i++)
	{
		byte* tmpBuf = bufferDat + i * vertexSize + offset;
		if (dt == RHIDefine::DT_FLOAT)
		{
			float* vertexBuf = (float*)tmpBuf;
			Math::Vec3 vertex;
			vertex.x = *vertexBuf; vertexBuf++;
			vertex.y = *vertexBuf; vertexBuf++;
			vertex.z = *vertexBuf; vertexBuf++;
			vertices.push_back(vertex);
		}
		else
		{
			assert(false);
		}
	}
}

void VertexStream::CopyVertexBuffer(uint vertexcount, uint vertexsize, const byte* buffer)
{
	if (vertexsize == m_nChunkStride)
	{
		ReserveBuffer(vertexcount);
		m_nVertexCount = vertexcount;
		memcpy(m_Buffer.data(), buffer, vertexcount * vertexsize);
	}
	else
	{
		JYERROR("vertex copy error: size of each vertex is miss match");
	}
}

NS_JYE_END