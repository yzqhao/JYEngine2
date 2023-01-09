#include "VertexBufferResource.h"
#include "RHI/DynamicRHI.h"
#include "Engine/render/VertexStream.h"

NS_JYE_BEGIN

VertexBufferResource::VertexBufferResource(void)
	: ISharedResource(ResourceProperty::SRT_VERTEXBUFFER)
	, m_eMemoryUseage((RHIDefine::MemoryUseage)-1)
	, m_handle(nullhandle)
	, m_isKeepSource(false)
	, m_uVertexCount(0)
	, m_uVertexSize(0)
	, m_pVertexStream(_NEW VertexStream())
{
}

VertexBufferResource::~VertexBufferResource(void)
{
	SAFE_DELETE(m_pVertexStream);
}

bool VertexBufferResource::_DeprecatedFilter(bool isd)
{
	if (isd)
	{
		m_handle = nullhandle;
		if (!m_pVertexStream)
		{
			m_pVertexStream = _NEW VertexStream();
		}
	}
	return isd;
}

void VertexBufferResource::ReleaseResource()
{
	if (-1 != m_handle)
	{
		RHIDestoryVertexBuffer(m_handle);
		m_handle = -1;
	}
	m_VertexLayout.Clear();
}

bool VertexBufferResource::FlushVertexBuffer(const VertexStream& stream, bool isKeepSource)
{
	return _FlushVertexBuffer(stream, isKeepSource, true);
}

#define BGFX_BUFFER_COMPUTE_READ            UINT16_C(0x0100) //!< Buffer will be read by shader.
#define BGFX_BUFFER_COMPUTE_WRITE           UINT16_C(0x0200) //!< Buffer will be used for writing.
#define BGFX_BUFFER_COMPUTE_READ_WRITE (0 \
	| BGFX_BUFFER_COMPUTE_READ \
	| BGFX_BUFFER_COMPUTE_WRITE \
	)

uint16_t MemoryUsage2Bgfx(RHIDefine::MemoryUseage usage)
{
	uint16_t mu = 0;
	switch (usage)
	{
	case RHIDefine::MemoryUseage::MU_READ:
		mu = BGFX_BUFFER_COMPUTE_READ;
		break;
	case RHIDefine::MemoryUseage::MU_WRITE:
		mu = BGFX_BUFFER_COMPUTE_WRITE;
		break;
	case RHIDefine::MemoryUseage::MU_READWRITE:
		mu = BGFX_BUFFER_COMPUTE_READ_WRITE;
		break;
	default:
		break;
	}
	return mu;
}


bool VertexBufferResource::_FlushVertexBuffer(const VertexStream& stream, bool isKeepSource, bool all)
{
	m_isKeepSource = isKeepSource;
	bool bNeedCreate = ((m_handle == -1) || m_uVertexCount < stream.GetVertexCount());
	bool isDynamic = (RHIDefine::MU_STATIC != m_eMemoryUseage);
	const void* vertexData = stream.GetBufferData();
	uint bufferSize = stream.GetVertexCount() * stream.GetVertexStride();
	bool updateOK = true;

	if (bNeedCreate)
	{
		ReleaseResource();
		m_VertexLayout = stream.GetLayout();
		if (stream.GetFlag() == 1)
		{
			uint16 _flag = MemoryUsage2Bgfx(m_eMemoryUseage);
			m_handle = RHICreateVertexBuffer(vertexData, bufferSize, m_VertexLayout, m_eMemoryUseage, BGFX_BUFFER_COMPUTE_READ);
		}
		else
		{
			m_handle = RHICreateVertexBuffer(vertexData, bufferSize, m_VertexLayout, m_eMemoryUseage, 0ULL);
		}

	}
	else
	{
		//update data only in dynamic buffer
		if (isDynamic)
		{
			if (all)
			{
				RHIUpdateVertexBuffer(m_handle, 0, vertexData, bufferSize);
			}
			else
			{
				const Math::IntVec2& interval = stream.GetReflushInterval();

				uint startVertex = interval.x;
				uint endVertex = interval.y;

				if (endVertex > startVertex)
				{
					uint memSize = (endVertex - startVertex + 1) * stream.GetVertexStride();

					vertexData = stream.GetBufferData(startVertex);
					RHIUpdateVertexBuffer(m_handle, startVertex, vertexData, memSize);
				}
				else if (startVertex > endVertex)
				{
					// 刷新后半部分数据
					uint startVertex0 = endVertex;
					uint endVertex0 = stream.GetVertexCount() - 1;
					uint memSize = (endVertex0 - startVertex0 + 1) * stream.GetVertexStride();
					vertexData = stream.GetBufferData(startVertex0);
					RHIUpdateVertexBuffer(m_handle, startVertex0, vertexData, memSize);

					// 刷新前半部分数据
					uint startVertex1 = 0;
					uint endVertex1 = interval.y;
					memSize = (endVertex1 - startVertex1 + 1) * stream.GetVertexStride();
					vertexData = stream.GetBufferData();
					RHIUpdateVertexBuffer(m_handle, startVertex, vertexData, memSize);
				}
			}
		}
		else
		{
			JYERROR("static vertex buffer can't be change");
			updateOK = false;
		}
	}

	if (updateOK)
	{
		// keep source
		if (isKeepSource || isDynamic)
		{
			*m_pVertexStream = stream;
		}

		m_uVertexSize = stream.GetVertexSize();
		m_uVertexCount = stream.GetVertexCount();
		stream.ResetFlushInterval();
	}

	return updateOK;
}

bool VertexBufferResource::ChangeVertexBuffer(const VertexStream& stream)
{
	return _FlushVertexBuffer(stream, m_isKeepSource, false);
}

void VertexBufferResource::UnloadSource()
{
	m_isKeepSource = false;
	if (RHIDefine::MU_STATIC == m_eMemoryUseage)
	{
		SAFE_DELETE(m_pVertexStream);
	}
}

VertexStream* VertexBufferResource::GetVertexStream()
{
	return m_pVertexStream;
}

NS_JYE_END