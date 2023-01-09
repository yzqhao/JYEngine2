
#include "RenderComponent.h"
#include "Engine/object/GObject.h"
#include "Engine/object/Scene.h"
#include "Engine/queue/IRenderQueue.h"
#include "System/Delegate.h"

NS_JYE_BEGIN

static void RenderComponent_m_GetRenderOrder(RenderComponent* self, int* value)
{
	*value = self->GetRenderOrder();
}

static void RenderComponent_m_SetRenderOrder(RenderComponent* self, int* value)
{
	self->SetRenderOrder(*value);
}

IMPLEMENT_RTTI(RenderComponent, Component);
BEGIN_ADD_PROPERTY(RenderComponent, Component);
REGISTER_PROPERTY(m_eRenderProperty, m_eRenderProperty, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_RenderOrder, m_RenderOrder, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_Sequence, m_Sequence, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_pRenderObject, m_pRenderObject, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_pMaterialEntities, m_pMaterialEntities, Property::F_SAVE_LOAD_CLONE)
ADD_PROPERTY_MEM_FUNCTION(m_RenderOrder, RenderComponent_m_GetRenderOrder, RenderComponent_m_SetRenderOrder)
END_ADD_PROPERTY

COMPONENT_FACTORY_FUNC(RenderComponent)
IMPLEMENT_INITIAL_NO_FACTORY_BEGIN(RenderComponent)
IMPLEMENT_INITIAL_END

RenderComponent::RenderComponent(GObject* pHostNode)
	: Component(pHostNode)
	, m_pRenderObject(_NEW RenderObjectEntity)
	, m_RenderOrder(0)
	, m_Sequence(0)
	, m_isSuccess(true)		// 没有异步加载，暂时设置为true
	, m_isDirtyBindBox(true)
	, m_HasSetBindBox(false)
	, m_eRenderProperty(GraphicDefine::RP_DEFAULT)
{

}

RenderComponent::~RenderComponent()
{
	for (int i = 0; i < m_pMaterialEntities.size(); i++)
	{
		SAFE_DELETE(m_pMaterialEntities[i]);
	}
	SAFE_DELETE(m_pRenderObject);
}

void RenderComponent::_DoOnAttachNode()
{
	Scene* scene = GetParentObject()->GetScene();
	if (scene->GetRenderQueue())
	{
		scene->GetRenderQueue()->AttachRenderComponent(this);
	}
	else
	{
		JYERROR("IRenderQueue in Scene's SceneRender is null ");
	}

	m_hMessagePosition = GetParentObject()->RegisterMessageListener(IMessage::SA_POSITION, MessageDelegateDelegate::CreateRaw(this, &RenderComponent::MessagePosition));
	m_hMessageScale = GetParentObject()->RegisterMessageListener(IMessage::SA_SCALE, MessageDelegateDelegate::CreateRaw(this, &RenderComponent::MessageScale));
	m_hMessageRot = GetParentObject()->RegisterMessageListener(IMessage::SA_ROTATION, MessageDelegateDelegate::CreateRaw(this, &RenderComponent::MessageRotate));
	_DirtyBindBox();

	m_hChangeStaticID = RegisterChangeIDListener(ChangeStaticIDEventDelegate::CreateRaw(this, &RenderComponent::_ChangeID));
}

void RenderComponent::_DoOnDetachNode()
{
	Scene* scene = GetParentObject()->GetScene();
	if (scene->GetRenderQueue())
	{
		scene->GetRenderQueue()->DetachRenderComponent(this);
	}

	GetParentObject()->RemoveMessageListener(IMessage::SA_POSITION, m_hMessagePosition);
	GetParentObject()->RemoveMessageListener(IMessage::SA_SCALE, m_hMessageScale);
	GetParentObject()->RemoveMessageListener(IMessage::SA_ROTATION, m_hMessageRot);
	
	RemoveChangeIDListener(m_hChangeStaticID);
}

void RenderComponent::ResizeMaterials(int size)
{
	for (uint i = size; i < m_pMaterialEntities.size(); ++i)
	{
		SAFE_DELETE(m_pMaterialEntities[i]);
	}
	m_pMaterialEntities.resize(size, NULL);
}

void RenderComponent::AddMaterialEntity(MaterialEntity* ma)
{
	m_pMaterialEntities.push_back(_NEW MaterialEntity(*ma));
}

void RenderComponent::RemoveMaterialEntity(int index)
{
	JY_ASSERT(IsVectorIndexValid(m_pMaterialEntities, index));
	m_pMaterialEntities.erase(m_pMaterialEntities.begin() + index);
}

void RenderComponent::ChangeMaterialEntity(int index, MaterialEntity* ma)
{
	if (index == m_pMaterialEntities.size())
	{
		m_pMaterialEntities.push_back(_NEW MaterialEntity(*ma));
	}
	else
	{
		SAFE_DELETE(m_pMaterialEntities[index]);
		m_pMaterialEntities[index] = _NEW MaterialEntity(*ma);
	}
}

uint RenderComponent::GetMaterialCount()
{
	return m_pMaterialEntities.size();
}

MaterialEntity* RenderComponent::GetMaterialEntity(int index) const
{
	if (index < 0 || index > m_pMaterialEntities.size())
	{
		return NULL;
	}
	return m_pMaterialEntities[index];
}

const std::vector<MaterialEntity*>& RenderComponent::GetMaterialEntities()
{
	return m_pMaterialEntities;
}

void RenderComponent::PushMetadata(const IMetadata& meta)
{
	m_pRenderObject->PushMetadata(meta);
}

void RenderComponent::SetParameter(RHIDefine::ParameterSlot att, MaterialParameter* obj)
{
	for (int materialIndex = 0; materialIndex < m_pMaterialEntities.size(); ++materialIndex)
	{
		MaterialEntity* mat = m_pMaterialEntities[materialIndex];
		if (mat)
		{
			mat->SetParameter(att, obj);
		}
	}
}

void RenderComponent::SetParameter(const String& paramName, MaterialParameter* obj)
{
	for(int materialIndex = 0; materialIndex < m_pMaterialEntities.size(); ++materialIndex)
	{
		MaterialEntity* mat = m_pMaterialEntities[materialIndex];
		if (mat)
		{
			mat->SetParameter(paramName, obj);
		}
	}
}

void RenderComponent::SetParameter(int materialIndex, RHIDefine::ParameterSlot att, MaterialParameter* obj)
{
	if (materialIndex >= 0 && materialIndex < m_pMaterialEntities.size())
	{
		MaterialEntity* mat = m_pMaterialEntities[materialIndex];
		if (mat)
		{
			mat->SetParameter(att, obj);
		}
	}
}

void RenderComponent::SetParameter(int materialIndex, const String& paramName, MaterialParameter* obj)
{
	if (materialIndex >= 0 && materialIndex < m_pMaterialEntities.size())
	{
		MaterialEntity* mat = m_pMaterialEntities[materialIndex];
		if (mat)
		{
			mat->SetParameter(paramName, obj);
		}
	}
}

VertexStream* RenderComponent::GetVertexStream()
{
	return m_pRenderObject->GetVertexStream();
}

IndicesStream* RenderComponent::GetIndexStream()
{
	return m_pRenderObject->GetIndexStream();
}

bool RenderComponent::ChangeVertexBuffer(const VertexStream& stream) //缓存更新
{
	if (stream.isValid())
	{
		m_pRenderObject->ChangeVertexBuffer(stream);
		return true;
	}
	return false;
}

bool RenderComponent::ChangeIndexBuffer(const IndicesStream& stream) //索引缓存更新
{
	m_pRenderObject->ChangeIndexBuffer(stream);
	return true;
}

Map<String, MaterialParameter*> RenderComponent::GetUniformParameter(int index) const
{
	return std::move(m_pMaterialEntities[index]->GetParameters());
}

void RenderComponent::ChangeRenderObjectEntity(const IMetadata& meta)
{
	SAFE_DELETE(m_pRenderObject);
	m_pRenderObject = _NEW RenderObjectEntity;
	m_pRenderObject->PushMetadata(meta);
	m_pRenderObject->CreateResource();

	SetBindBox(m_pRenderObject->GetBindBox());
	//_PropertyChange();
}

void RenderComponent::ChangeRenderObjectEntity(const ISourceMetadataList& metas)
{
	SAFE_DELETE(m_pRenderObject);
	m_pRenderObject = _NEW RenderObjectEntity;
	m_pRenderObject->PushMetadata(metas);
	m_pRenderObject->CreateResource();

	SetBindBox(m_pRenderObject->GetBindBox());
	//_PropertyChange();
}

void RenderComponent::BeforeSave(Stream* pStream)
{
	Component::BeforeSave(pStream);
}

void RenderComponent::PostSave(Stream* pStream)
{
	Component::PostSave(pStream);
}

void RenderComponent::PostLateLoad(Stream* pStream)
{
	Component::PostLateLoad(pStream);
	SetBindBox(m_pRenderObject->GetBindBox());
}

//-------------------- AABB --------------------------- 
void RenderComponent::MessagePosition(const IMessage& message)
{
	_DirtyBindBox();
}

void RenderComponent::MessageScale(const IMessage& message)
{
	_DirtyBindBox();
}

void RenderComponent::MessageRotate(const IMessage& message)
{
	_DirtyBindBox();
}

bool RenderComponent::TryRecaculateBindBox()
{
	bool res = m_isDirtyBindBox;
	if (m_isDirtyBindBox)
	{
		m_isDirtyBindBox = false;
		_DoCaculateBindbox();
	}

	return res;
}

void RenderComponent::_DoCaculateBindbox()
{
	m_TransBindBox = m_OgiBindBox;
	if (GetParentObject())
	{
		const Math::Mat4* local2WorldMat = GetParentObject()->Attribute<Math::Mat4>(GraphicDefine::GA_WORLD_TRANSFORM);

		if (local2WorldMat)
		{
			Math::Vec3 extends = m_OgiBindBox.GetExtent();

			Math::Vec3 newExtends;
			newExtends.x = Math::ABS(local2WorldMat->a11 * extends.x) + Math::ABS(local2WorldMat->a21 * extends.y)
				+ Math::ABS(local2WorldMat->a31 * extends.z);
			newExtends.y = Math::ABS(local2WorldMat->a12 * extends.x) + Math::ABS(local2WorldMat->a22 * extends.y)
				+ Math::ABS(local2WorldMat->a32 * extends.z);
			newExtends.z = Math::ABS(local2WorldMat->a13 * extends.x) + Math::ABS(local2WorldMat->a23 * extends.y)
				+ Math::ABS(local2WorldMat->a33 * extends.z);

			Math::Vec3 centerPos = m_OgiBindBox.GetCenter() * (*local2WorldMat);
			m_TransBindBox = Math::AABB(centerPos - newExtends, centerPos + newExtends);
		}
	}
}

//-------------------- change id --------------------------- 
void RenderComponent::_ChangeID(Animatable* self, uint64 oldid, uint64 newid)
{
	JY_ASSERT(self == this);
	// light组件修改id
	IRenderQueue* renderQueue = GetParentObject()->GetScene()->GetRenderQueue();
	auto& comMap = renderQueue->m_pAttachedComponentMap;
	auto it = comMap.find(oldid);
	if (it != comMap.end())
	{
		comMap.erase(it);
		comMap[newid] = this;
	}
}

NS_JYE_END