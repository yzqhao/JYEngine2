#pragma once

#include "Engine/private/Define.h"
#include "Engine/graphic/IAsyncResource.h"
#include "RHI/RHIDefine.h"
#include "Engine/resource/IAsyncEntity.h"
#include "Engine/graphic/RenderObjectResource.h"
#include "Engine/resource/ISharedSource.h"
#include "Engine/resource/RenderObjectMetadata.h"
#include "material/MaterialEntity.h"
#include "VertexBufferEntity.h"
#include "IndicesBufferEntity.h"
#include "Math/IntVec2.h"
#include "Math/Vec2.h"
#include "Math/Vec3.h"
#include "Math/Vec4.h"
#include "Math/AABB.h"

NS_JYE_BEGIN

class MaterialParameter;

class RenderObjectSource : public IMonopolySource
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	RenderObjectSource();
	virtual ~RenderObjectSource();

	int GetMaterialCount();
	MaterialEntity* GetMaterialEntity(int index);
	const std::vector<MaterialEntity*>& GetMaterialEntities();
	void SetMaterialEntities(const std::vector<MaterialEntity*>&);
	MaterialEntity* MakeMaterialAttachment();
	bool hasParameter(RHIDefine::ParameterSlot att);
	void SetParameter(RHIDefine::ParameterSlot att, MaterialParameter* obj);
	void SetKeepSource(bool isKeepSource);
	void SetDynamicBatch(bool isDynamicBatch);
	bool isKeepSource() const;
	const Vector<Vector<int>>& GetAffectedJointIDs();
	void ChangeMaterialEntity(MaterialEntity* ma, int index);
	String& GetMeshName();
	Math::AABB GetBindBox();
	Math::IntVec2 GetDrawRange(int ind) const;

private:
	virtual IAsyncResource* _DoCreateResource() override;//将资源数据制作为实体资源
	virtual bool _DoSetupResource(IAsyncResource* res) override;//重新创建资源
	virtual bool _DoLoadResource() override;//加载资源

	virtual bool _SetupMeshData(RenderObjectData::MeshData* meshData);

private:

	RHIDefine::RenderMode m_eRenderMode;
	Math::AABB m_BindingBox;
	std::vector<MaterialEntity*> m_pMaterialEntities; //多个材质
	VertexBufferEntity* m_pVertexBufferEntity;
	IndicesBufferEntity* m_pIndicesBufferEntity;
	VertexBufferEntity* m_pInstanceBufferEntity;
	std::vector<std::vector<int>> m_AffectedJointIDs;
	std::vector<std::pair<int, int>> m_indicesOffsetAndLength;
	bool m_isKeepSource;
	bool m_isDynamicBatch;
	String m_MeshPath;
};
DECLARE_Ptr(RenderObjectSource);
TYPE_MARCO(RenderObjectSource);

class ENGINE_API RenderObjectEntity : public IAsyncEntity<RenderObjectResource, RenderObjectSource>
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	RenderObjectEntity();
	virtual ~RenderObjectEntity();
	DECLARE_OBJECT_CTOR_FUNC(RenderObjectEntity);

	VertexStream* GetVertexStream();
	IndicesStream* GetIndexStream();
	const std::vector<std::vector <int>>& GetAffectedIDs();

	virtual void BeforeSave(Stream* pStream) override;
	virtual void PostSave(Stream* pStream) override;
	virtual void PostLoad(Stream* pStream) override;

	FORCEINLINE SubMesh* CreateSubMesh();
	FORCEINLINE void DisableSubMeshs();
	FORCEINLINE uint GetDrawCount() const;
	FORCEINLINE uint GetInstanceCount() const;
	FORCEINLINE bool SetInstanceCount(uint count);
	FORCEINLINE void ChangeVertexBuffer(const VertexStream& stream);
	FORCEINLINE void ChangeIndexBuffer(const IndicesStream& stream);
	FORCEINLINE void FlushVertexBuffer(const VertexStream& stream);
	FORCEINLINE void FlushIndiesBuffer(const IndicesStream& buffer);
	FORCEINLINE RHIDefine::RenderMode GetRenderMode();

	FORCEINLINE void SetKeepSource(bool isKeepSource);
	FORCEINLINE void SetDynamicBatch(bool isDynamicBatch);
	FORCEINLINE bool isKeepSource() const;
	FORCEINLINE std::string& GetMeshName();
	FORCEINLINE Math::AABB GetBindBox();
	FORCEINLINE Math::IntVec2 GetDrawRange(int ind) const;
	FORCEINLINE uint GetSubMeshsSize() const;
	FORCEINLINE uint GetVertexBufferHandle();
	FORCEINLINE VertexBufferEntity* GetVertexBufferEntity();
	FORCEINLINE IndicesBufferEntity* GetIndexBufferEntity();
	FORCEINLINE SubMesh* GetSubMeshByIndex(int index);
	FORCEINLINE size_t GetSubMeshHashByIndex(int index);
private:
	virtual void _OnCreateResource() override;

};
DECLARE_Ptr(RenderObjectEntity);
TYPE_MARCO(RenderObjectEntity);


FORCEINLINE VertexBufferEntity* RenderObjectEntity::GetVertexBufferEntity()
{
	return _GetResourcePtr()->GetVertexBufferEntity();
}

FORCEINLINE IndicesBufferEntity* RenderObjectEntity::GetIndexBufferEntity()
{
	return _GetResourcePtr()->GetIndexBufferEntity();
}

FORCEINLINE uint RenderObjectEntity::GetVertexBufferHandle()
{
	return _GetResourcePtr()->GetVertexBufferHandle();
}

FORCEINLINE SubMesh* RenderObjectEntity::CreateSubMesh()
{
	return _GetResourcePtr()->CreateSubMesh();
}

FORCEINLINE void RenderObjectEntity::DisableSubMeshs()
{
	_GetResourcePtr()->DisableSubMeshs();
}

FORCEINLINE uint RenderObjectEntity::GetDrawCount() const
{
	if (_GetResourcePtr())
	{
		return _GetResourcePtr()->GetDrawCount();
	}
	else
	{
		return 0;
	}

}

FORCEINLINE uint RenderObjectEntity::GetInstanceCount() const
{
	if (_GetResourcePtr())
	{
		return _GetResourcePtr()->GetInstanceCount();
	}
	else
	{
		return 0;
	}
}

FORCEINLINE bool RenderObjectEntity::SetInstanceCount(uint count)
{
	if (_GetResourcePtr())
	{
		return _GetResourcePtr()->SetInstanceCount(count);
	}
	else
	{
		return false;
	}
}

FORCEINLINE void RenderObjectEntity::ChangeVertexBuffer(const VertexStream& stream)
{
	if (isSuccess())
	{
		_GetResourcePtr()->ChangeVertexBuffer(stream);
	}
}

FORCEINLINE void RenderObjectEntity::ChangeIndexBuffer(const IndicesStream& stream)
{
	if (isSuccess())
	{
		_GetResourcePtr()->ChangeIndexBuffer(stream);
	}
}

FORCEINLINE void RenderObjectEntity::FlushVertexBuffer(const VertexStream& stream)
{
	if (isSuccess())
	{
		_GetResourcePtr()->FlushVertexBuffer(stream);
	}
}

FORCEINLINE void RenderObjectEntity::FlushIndiesBuffer(const IndicesStream& buffer)
{
	if (isSuccess())
	{
		_GetResourcePtr()->FlushIndiesBuffer(buffer);
	}
}

FORCEINLINE RHIDefine::RenderMode RenderObjectEntity::GetRenderMode()
{
	return _GetResourcePtr()->GetRenderMode();
}

FORCEINLINE void RenderObjectEntity::SetKeepSource(bool isKeepSource)
{
	_GetSourcePtr()->SetKeepSource(isKeepSource);
}

FORCEINLINE void RenderObjectEntity::SetDynamicBatch(bool isDynamicBatch)
{
	_GetSourcePtr()->SetDynamicBatch(isDynamicBatch);
}

FORCEINLINE bool RenderObjectEntity::isKeepSource() const
{
	return _GetSourcePtr()->isKeepSource();
}

FORCEINLINE std::string& RenderObjectEntity::GetMeshName()
{
	return _GetSourcePtr()->GetMeshName();
}

FORCEINLINE Math::AABB RenderObjectEntity::GetBindBox()
{
	return _GetSourcePtr()->GetBindBox();
}

FORCEINLINE Math::IntVec2 RenderObjectEntity::GetDrawRange(int ind) const
{
	return _GetSourcePtr()->GetDrawRange(ind);
}

FORCEINLINE uint RenderObjectEntity::GetSubMeshsSize() const
{
	if (_GetResourcePtr())
	{
		return _GetResourcePtr()->GetSubMeshsSize();
	}
	return 0;
}

FORCEINLINE SubMesh* RenderObjectEntity::GetSubMeshByIndex(int index)
{
	return _GetResourcePtr()->GetSubMeshByIndex(index);
}

FORCEINLINE size_t RenderObjectEntity::GetSubMeshHashByIndex(int index)
{
	return _GetResourcePtr()->GetSubMeshHashByIndex(index);
}

#include "RenderObjectEntity.inl"

NS_JYE_END