#pragma once

#include "Engine/private/Define.h"
#include "Math/IntVec2.h"
#include "Math/IntVec4.h"
#include "Math/Vec2.h"
#include "Math/Vec4.h"
#include "RHI/RHIDefine.h"
#include "Engine/graphic/IAsyncResource.h"
#include "Engine/render/SubMesh.h"
#include "Engine/render/VertexBufferEntity.h"
#include "Engine/render/IndicesBufferEntity.h"
#include "Core/Interface/ILogSystem.h"

NS_JYE_BEGIN

class ENGINE_API RenderObjectResource : public IMonopolyResource
{
public:
	RenderObjectResource();
	virtual ~RenderObjectResource();
public:
	virtual void ReleaseResource();

	void SetupRenderObject(VertexBufferEntity* vbo, IndicesBufferEntity* ibo, std::vector<std::pair<int, int>>& offsetAndLength);
	void FlushVertexBuffer(const VertexStream& stream);
	void ChangeVertexBuffer(const VertexStream& stream);
	void ChangeIndexBuffer(const IndicesStream& stream);
	void FlushIndiesBuffer(const IndicesStream& buffer);
	bool SetInstanceCount(uint count);
	VertexStream* GetVertexStream();
	IndicesStream* GetIndexStream();
	SubMesh* CreateSubMesh();
	void DisableSubMeshs();
	uint GetVertexBufferHandle();

	FORCEINLINE RHIDefine::RenderMode GetRenderMode();
	FORCEINLINE uint GetDrawCount() const;
	FORCEINLINE uint GetInstanceCount() const;
	FORCEINLINE void SetRenderMode(RHIDefine::RenderMode rm);
	FORCEINLINE uint GetSubMeshsSize() const;
	FORCEINLINE VertexBufferEntity* GetVertexBufferEntity();
	FORCEINLINE IndicesBufferEntity* GetIndexBufferEntity();
	FORCEINLINE SubMesh* GetSubMeshByIndex(int index);
	FORCEINLINE size_t GetSubMeshHashByIndex(int index);

private:

	virtual bool _DeprecatedFilter(bool isd) override;

	void UpdateDefaultCommand();
	FORCEINLINE void ReculateHash();

	Vector<SubMesh*> m_SubMeshs;
	SubMesh m_DefaultCommand;
	VertexBufferEntity* m_rpVertexBufferEntity;
	IndicesBufferEntity* m_rpIndicesBufferEntity;
	RHIDefine::RenderMode	m_eRenderMode;
	uint m_DrawCount;
	uint m_InstanceCount;
	uint m_CommandIndex;
	size_t m_hashValue;
};

FORCEINLINE VertexBufferEntity* RenderObjectResource::GetVertexBufferEntity()
{
	return m_rpVertexBufferEntity;
}

FORCEINLINE RHIDefine::RenderMode RenderObjectResource::GetRenderMode()
{
	return m_eRenderMode;
}

FORCEINLINE uint RenderObjectResource::GetDrawCount() const
{
	return m_DrawCount;
}

FORCEINLINE uint RenderObjectResource::GetInstanceCount() const
{
	return m_InstanceCount;
}

FORCEINLINE void RenderObjectResource::SetRenderMode(RHIDefine::RenderMode rm)
{
	m_eRenderMode = rm;
}

FORCEINLINE uint RenderObjectResource::GetSubMeshsSize() const
{
	return std::max<size_t>(m_SubMeshs.size(), 1);
}

FORCEINLINE SubMesh* RenderObjectResource::GetSubMeshByIndex(int index)
{
	SubMesh* pSubMesh = nullptr;
	if (m_SubMeshs.size() > 0)
	{
		if (index >= 0 && index < m_SubMeshs.size())
		{
			pSubMesh = m_SubMeshs[index];
		}
		else
		{
			JYERROR("SubMesh: Invalid submesh index %d!", index);
		}
	}
	else
	{
		pSubMesh = &m_DefaultCommand;
	}

	return pSubMesh;
}

FORCEINLINE IndicesBufferEntity* RenderObjectResource::GetIndexBufferEntity()
{
	return m_rpIndicesBufferEntity;
}

FORCEINLINE void RenderObjectResource::ReculateHash()
{
	handle indexHandle = (m_rpIndicesBufferEntity != nullptr ? m_rpIndicesBufferEntity->GetGpuHandle() : 0);
	handle vertexHandle = (m_rpVertexBufferEntity != nullptr ? m_rpVertexBufferEntity->GetGpuHandle() : 0);

	std::string strV;
	strV.append(std::to_string(indexHandle));
	strV.append(std::to_string(vertexHandle));
	std::hash<std::string> hashV;
	m_hashValue = hashV(strV);
}

FORCEINLINE size_t RenderObjectResource::GetSubMeshHashByIndex(int index)
{
	SubMesh* subMesh = GetSubMeshByIndex(index);
	return subMesh->GetHash() ^ (m_hashValue << 1);
}

NS_JYE_END