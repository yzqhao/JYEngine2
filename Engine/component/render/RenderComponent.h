#pragma once

#include "Engine/private/Define.h"
#include "Engine/component/Component.h"
#include "Engine/render/material/MaterialEntity.h"
#include "Engine/render/RenderObjectEntity.h"
#include "Math/MathUtil.h"

NS_JYE_BEGIN

class MaterialParameter;
class IMessage;

class ENGINE_API RenderComponent : public Component
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	RenderComponent() = delete;
	RenderComponent(GObject* pHostNode);
	virtual ~RenderComponent();

	void ResizeMaterials(int size);
	void AddMaterialEntity(MaterialEntity* ma);
	void RemoveMaterialEntity(int index);
	void ChangeMaterialEntity(int index, MaterialEntity* ma);
	uint GetMaterialCount();
	MaterialEntity* GetMaterialEntity(int index) const;
	const std::vector<MaterialEntity*>& GetMaterialEntities();
	void PushMetadata(const IMetadata& meta);
	void SetParameter(RHIDefine::ParameterSlot att, MaterialParameter* obj);
	void SetParameter(const String& paramName, MaterialParameter* obj);
	void SetParameter(int materialIndex, RHIDefine::ParameterSlot att, MaterialParameter* obj);
	void SetParameter(int materialIndex, const String& paramName, MaterialParameter* obj);
	VertexStream* GetVertexStream();
	IndicesStream* GetIndexStream();
	bool ChangeVertexBuffer(const VertexStream& stream);//缓存更新
	bool ChangeIndexBuffer(const IndicesStream& stream); //索引缓存更新
	Map<String, MaterialParameter*> GetUniformParameter(int index) const;
	void ChangeRenderObjectEntity(const IMetadata& meta);
	void ChangeRenderObjectEntity(const ISourceMetadataList& metas);

	bool TryRecaculateBindBox();
	void MessagePosition(const IMessage& message);
	void MessageScale(const IMessage& message);
	void MessageRotate(const IMessage& message);

	virtual void BeforeSave(Stream* pStream) override;
	virtual void PostSave(Stream* pStream) override;
	virtual void PostLateLoad(Stream* pStream) override;

	FORCEINLINE RHIDefine::RenderMode GetRenderMode() const;
	FORCEINLINE bool isKeepSource() const;
	FORCEINLINE RenderObjectEntity* GetRenderObjectEntity();
	FORCEINLINE void CreateResource();
	FORCEINLINE SubMesh* CreateSubMesh();
	FORCEINLINE SubMesh* GetSubMeshByIndex(int index);
	FORCEINLINE void DisableSubMeshs();
	FORCEINLINE uint GetDrawCount() const;
	FORCEINLINE void SetRenderOrder(int order);
	FORCEINLINE int GetRenderOrder();
	FORCEINLINE void SetSequence(int order);
	FORCEINLINE int GetSequence();
	FORCEINLINE void SetRenderProperty(GraphicDefine::RenderProperty rp);
	FORCEINLINE void EraseRenderProperty(GraphicDefine::RenderProperty rp);
	FORCEINLINE bool isRenderProperty(GraphicDefine::RenderProperty rp) const;
	FORCEINLINE bool isNotRenderProperty(GraphicDefine::RenderProperty rp) const;
	FORCEINLINE void FlushVertexBuffer(const VertexStream& stream);//缓存更新
	FORCEINLINE void FlushIndiesBuffer(const IndicesStream& buffer);
	FORCEINLINE bool isSuccess() const;

	FORCEINLINE void SetBindBox(const Math::AABB& box);
	FORCEINLINE const Math::AABB& GetBindBox();
	FORCEINLINE const Math::AABB& GetOriginalBindBox();
	FORCEINLINE bool Intersect(const Math::Ray& r);

protected:
	virtual void _DoOnAttachNode() override;
	virtual void _DoOnDetachNode() override;
protected:
	FORCEINLINE void _DirtyBindBox();
	FORCEINLINE void _TryInitBindBox(const Math::AABB& box);

	void _ChangeID(Animatable* self, uint64 oldid, uint64 newid);

	void _DoCaculateBindbox();

	Vector<MaterialEntity*> m_pMaterialEntities; //多个材质
	uint m_eRenderProperty;
	uint m_RenderOrder;
	int m_Sequence;//摄像机层级
	bool m_isSuccess;
	RenderObjectEntity* m_pRenderObject;

	Math::AABB m_OgiBindBox;
	Math::AABB m_TransBindBox;
	bool m_isDirtyBindBox;
	bool m_HasSetBindBox;
	DelegateHandle m_hMessagePosition;
	DelegateHandle m_hMessageScale;
	DelegateHandle m_hMessageRot;
	DelegateHandle m_hChangeStaticID;
};
DECLARE_Ptr(RenderComponent);
TYPE_MARCO(RenderComponent);

FORCEINLINE void RenderComponent::_DirtyBindBox()
{
	m_isDirtyBindBox = true;
}

FORCEINLINE void RenderComponent::_TryInitBindBox(const Math::AABB& box)
{
	if (!m_HasSetBindBox)
	{
		SetBindBox(box);
	}
}

FORCEINLINE RHIDefine::RenderMode RenderComponent::GetRenderMode() const
{
	return m_pRenderObject->GetRenderMode();
}

FORCEINLINE bool RenderComponent::isKeepSource() const
{
	return m_pRenderObject->isKeepSource();
}

FORCEINLINE RenderObjectEntity* RenderComponent::GetRenderObjectEntity()
{
	return m_pRenderObject;
}

FORCEINLINE void RenderComponent::CreateResource() //开始加载资源
{
	_DirtyBindBox();
	m_pRenderObject->CreateResource();
}

FORCEINLINE SubMesh* RenderComponent::CreateSubMesh()
{
	return m_pRenderObject->CreateSubMesh();
}

FORCEINLINE SubMesh* RenderComponent::GetSubMeshByIndex(int index)
{
	return m_pRenderObject->GetSubMeshByIndex(index);
}

FORCEINLINE void RenderComponent::DisableSubMeshs()
{
	m_pRenderObject->DisableSubMeshs();
}

FORCEINLINE uint RenderComponent::GetDrawCount() const
{
	return m_pRenderObject->GetDrawCount();
}

FORCEINLINE void RenderComponent::SetRenderOrder(int order)
{
	m_RenderOrder = order;
}

FORCEINLINE int RenderComponent::GetRenderOrder()
{
	return m_RenderOrder;
}

FORCEINLINE void RenderComponent::SetSequence(int s)
{
	m_Sequence = s;
}

FORCEINLINE int RenderComponent::GetSequence()
{
	return m_Sequence;
}

FORCEINLINE void RenderComponent::SetRenderProperty(GraphicDefine::RenderProperty rp)
{
	m_eRenderProperty |= rp;
}

FORCEINLINE void RenderComponent::EraseRenderProperty(GraphicDefine::RenderProperty rp)
{
	m_eRenderProperty &= ~rp;
}

FORCEINLINE bool RenderComponent::isRenderProperty(GraphicDefine::RenderProperty rp) const
{
	return (m_eRenderProperty & rp) != 0;
}

FORCEINLINE bool RenderComponent::isNotRenderProperty(GraphicDefine::RenderProperty rp) const
{
	return (m_eRenderProperty & rp) == 0;
}

FORCEINLINE void RenderComponent::FlushVertexBuffer(const VertexStream& stream)
{
	m_pRenderObject->FlushVertexBuffer(stream);
}

FORCEINLINE void RenderComponent::FlushIndiesBuffer(const IndicesStream& buffer)
{
	m_pRenderObject->FlushIndiesBuffer(buffer);
}

FORCEINLINE bool RenderComponent::isSuccess() const
{
	return m_isSuccess;
}

FORCEINLINE void RenderComponent::SetBindBox(const Math::AABB& box)
{
	m_isDirtyBindBox = true;
	m_OgiBindBox = box;
	m_HasSetBindBox = true;
}

FORCEINLINE const Math::AABB& RenderComponent::GetBindBox()
{
	TryRecaculateBindBox();
	return m_TransBindBox;
}

FORCEINLINE const Math::AABB& RenderComponent::GetOriginalBindBox()
{
	return m_OgiBindBox;
}

FORCEINLINE bool RenderComponent::Intersect(const Math::Ray& r)
{
	TryRecaculateBindBox();
	Math::Vec3 out;
	return Math::MathUtil::intersects(r, m_TransBindBox, out);
}


NS_JYE_END