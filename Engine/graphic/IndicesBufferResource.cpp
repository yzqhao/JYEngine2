#include "IndicesBufferResource.h"
#include "RHI/DynamicRHI.h"
#include "Engine/render/IndicesStream.h"

NS_JYE_BEGIN

IndicesBufferResource::IndicesBufferResource(void)
	: ISharedResource(ResourceProperty::SRT_INDICESBUFFER)
	, m_eMemoryUseage((RHIDefine::MemoryUseage)-1)
	, m_GPUResourceHandle(nullhandle)
	, m_uIndicesCount(0)
	, m_TypeSize(-1)
	, m_isKeepSource(false)
	, m_IndexStream(_NEW IndicesStream())
{
}

IndicesBufferResource::~IndicesBufferResource(void)
{
	SAFE_DELETE(m_IndexStream);
}

bool IndicesBufferResource::_DeprecatedFilter(bool isd)
{
	if (isd)
	{
		m_GPUResourceHandle = nullhandle;
		if (!m_IndexStream)
		{
			m_IndexStream = _NEW IndicesStream();
		}
	}
	return isd;
}

void IndicesBufferResource::ReleaseResource()
{
	if (nullhandle != m_GPUResourceHandle)
	{
		RHIDestroyIndiceBuffer(m_GPUResourceHandle);
		m_GPUResourceHandle = nullhandle;
	}
}

bool IndicesBufferResource::_FlushIndicesBuffer(uint offset, uint indicesCount, uint typesize, const void* buffer)
{
	switch (typesize)
	{
	case 2: m_eIndicesType = RHIDefine::IT_UINT16; break;
	case 4: m_eIndicesType = RHIDefine::IT_UINT32; break;
	default: JYERROR("Unkown buffer type!");
	}
	if (nullhandle == m_GPUResourceHandle//初始化
		|| (-1 == m_TypeSize || typesize != m_TypeSize))
	{
		JY_ASSERT(0 == offset);//这种重新分配的方法必须保证传入的指针必须是从头开始的，不然我们没有办法增量添加
		m_TypeSize = typesize;
		m_uIndicesCount = indicesCount;
		m_GPUResourceHandle = RHICreateIndiceBuffer(
			indicesCount,
			m_TypeSize,
			buffer,
			m_eMemoryUseage);
	}
	else if (m_uIndicesCount < offset + indicesCount)//原来的不够了
	{
		JY_ASSERT(0 == offset);//这种重新分配的方法必须保证传入的指针必须是从头开始的，不然我们没有办法增量添加
		ReleaseResource();
		m_uIndicesCount = offset + indicesCount;
		m_GPUResourceHandle = RHICreateIndiceBuffer(
			offset + indicesCount,
			m_TypeSize,
			buffer,
			m_eMemoryUseage);
	}
	else if (buffer != NULL)
	{
		if (RHIDefine::MU_STATIC != m_eMemoryUseage)
		{
			m_uIndicesCount = indicesCount;
			RHIUpdateIndiceBuffer(m_GPUResourceHandle,
				offset,
				indicesCount,
				m_TypeSize,
				buffer);
		}
		else
		{
			JYERROR("static indices buffer can't be flush");
			return false;
		}
	}
	return true;
}

bool IndicesBufferResource::FlushIndicesBuffer(const IndicesStream& stream, bool isKeepSource)
{
	m_isKeepSource = isKeepSource;
	if (nullhandle == m_GPUResourceHandle
		|| (-1 == m_TypeSize || stream.GetIndicesStride() != m_TypeSize)
		|| m_uIndicesCount < stream.GetIndicesCount()
		|| RHIDefine::MU_STATIC != m_eMemoryUseage)
	{
		if (isKeepSource || RHIDefine::MU_STATIC != m_eMemoryUseage)
		{
			*m_IndexStream = stream;
		}
		_FlushIndicesBuffer(0, stream.GetIndicesCount(), stream.GetIndicesStride(), stream.GetBuffer());
		return true;
	}
	JYERROR("vertex buffer can't be flush");
	return false;
}

bool IndicesBufferResource::ChangeIndicesBuffer(const IndicesStream& stream)
{
	if (RHIDefine::MU_STATIC != m_eMemoryUseage)
	{
		*m_IndexStream = stream;
		_FlushIndicesBuffer(0, stream.GetIndicesCount(), stream.GetIndicesStride(), stream.GetBuffer());
		return true;
	}
	JYERROR("static indices buffer can't be change");
	return false;
}

void IndicesBufferResource::UnloadSource()
{
	m_isKeepSource = false;
	if (RHIDefine::MU_STATIC == m_eMemoryUseage)
	{
		SAFE_DELETE(m_IndexStream);
	}
}

IndicesStream* IndicesBufferResource::GetIndexStream()
{
	return m_IndexStream;
}

NS_JYE_END