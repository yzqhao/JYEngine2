#include "RenderObjectResource.h"
#include "RHI/DynamicRHI.h"

NS_JYE_BEGIN

RenderObjectResource::RenderObjectResource(void)
	: IMonopolyResource(ResourceProperty::MRT_RENDEROBJECT)
	, m_eRenderMode(RHIDefine::RM_TRIANGLES)
	, m_rpVertexBufferEntity(NULL)
	, m_rpIndicesBufferEntity(NULL)
	, m_DrawCount(nullhandle)
	, m_InstanceCount(0)
	, m_CommandIndex(0)
	, m_DefaultCommand()
	, m_hashValue(0)
{
}

RenderObjectResource::~RenderObjectResource(void)
{
}

void RenderObjectResource::ReleaseResource()
{
	for (SubMesh* rc : m_SubMeshs)
	{
		SAFE_DELETE(rc);
	}
}
//-----------------------------------------------------------------------------------------------------
void RenderObjectResource::UpdateDefaultCommand()
{
	m_DefaultCommand.SetDrawRange(0, 0);
	if (NULL != m_rpIndicesBufferEntity)
	{
		m_DefaultCommand.SetDrawRange(0, m_rpIndicesBufferEntity->GetIndicesCount());
	}
	else if (NULL != m_rpVertexBufferEntity)
	{
		m_DefaultCommand.SetDrawRange(0, m_rpVertexBufferEntity->GetVertexCount());
	}
	m_DefaultCommand.Enable();
}
//-----------------------------------------------------------------------------------------------------
void RenderObjectResource::SetupRenderObject(VertexBufferEntity* vbo, IndicesBufferEntity* ibo, std::vector<std::pair<int, int>>& offsetAndLength)
{
	if (vbo && vbo->isSuccess())
	{
		m_rpVertexBufferEntity = vbo;
	}
	if (ibo && ibo->isSuccess())
	{
		m_rpIndicesBufferEntity = ibo;
	}

	UpdateDefaultCommand();
	for (size_t i = 0; i < offsetAndLength.size(); ++i)
	{
		const std::pair<int, int>& offsetcount = offsetAndLength[i];
		SubMesh* rc = CreateSubMesh();
		rc->SetDrawRange(offsetcount.first, offsetcount.second);
	}

	ReculateHash();
}

bool RenderObjectResource::_DeprecatedFilter(bool isd)
{
	return false;
}

void RenderObjectResource::FlushVertexBuffer(const VertexStream& stream)
{
	if (m_rpVertexBufferEntity)
	{
		m_rpVertexBufferEntity->FlushVertexBuffer(stream);
		UpdateDefaultCommand();
	}
	ReculateHash();
}

void RenderObjectResource::ChangeVertexBuffer(const VertexStream& stream)
{
	if (m_rpVertexBufferEntity)
	{
		m_rpVertexBufferEntity->ChangeVertexBuffer(stream);
		UpdateDefaultCommand();
	}
	ReculateHash();
}

void RenderObjectResource::ChangeIndexBuffer(const IndicesStream& stream)
{
	if (m_rpIndicesBufferEntity)
	{
		m_rpIndicesBufferEntity->ChangeIndiesBuffer(stream);
		UpdateDefaultCommand();
	}
	ReculateHash();
}

void RenderObjectResource::FlushIndiesBuffer(const IndicesStream& buffer)
{
	if (m_rpIndicesBufferEntity)
	{
		m_rpIndicesBufferEntity->FlushIndiesBuffer(buffer);
		UpdateDefaultCommand();
	}
	ReculateHash();
}

bool RenderObjectResource::SetInstanceCount(uint count)
{
	m_InstanceCount = count;
	return true;
}

SubMesh* RenderObjectResource::CreateSubMesh()
{
	SubMesh* rc;
	if (m_CommandIndex < m_SubMeshs.size())
	{
		rc = m_SubMeshs[m_CommandIndex];
	}
	else
	{
		rc = _NEW SubMesh();
		m_SubMeshs.push_back(rc);
	}
	rc->Enable();
	++m_CommandIndex;
	return rc;
}

void RenderObjectResource::DisableSubMeshs()
{
	m_CommandIndex = 0;
	for (SubMesh* rc : m_SubMeshs)
	{
		rc->Disable();
	}
}

VertexStream* RenderObjectResource::GetVertexStream()
{
	return m_rpVertexBufferEntity ?
		m_rpVertexBufferEntity->GetVertexStream()
		: NULL;
}

IndicesStream* RenderObjectResource::GetIndexStream()
{
	return m_rpIndicesBufferEntity ?
		m_rpIndicesBufferEntity->GetIndexStream()
		: NULL;
}

uint RenderObjectResource::GetVertexBufferHandle()
{
	return m_rpVertexBufferEntity ?
		m_rpVertexBufferEntity->GetGpuHandle()
		: nullhandle;
}

NS_JYE_END