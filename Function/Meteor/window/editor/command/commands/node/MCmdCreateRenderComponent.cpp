#include "MCmdCreateRenderComponent.h"
#include "Engine/object/SceneManager.h"
#include "Engine/object/Scene.h"
#include "util/MSceneManagerExt.h"
#include "Engine/component/render/RenderComponent.h"
#include "Engine/resource/RenderObjectMetadata.h"

NS_JYE_BEGIN

MCmdCreateRenderComponent::MCmdCreateRenderComponent(int64 id, const String& comname)
	: m_goID(id)
	, m_comID(0)
	, m_compName(comname)
{

}

MCmdCreateRenderComponent::~MCmdCreateRenderComponent()
{

}

Object* MCmdCreateRenderComponent::_DoIt()
{
	Component* component = nullptr;
	GObject* node = GetGameObject(m_goID);
	if (node)
	{
		component = node->CreateComponent(m_compName);
		RenderComponent* ren = static_cast<RenderComponent*>(component);
		ren->PushMetadata(RenderObjectMeshFileMetadate(
			RHIDefine::MU_DYNAMIC,
			"comm:mesh/default.dynamicmesh"));
		ren->CreateResource();
		component->Awake();
		if (m_comID == 0)
		{
			m_comID = component->GetStaticID();
		}
		else
		{
			component->SetStaticID(m_comID);
		}
	}
	return component;
}

void MCmdCreateRenderComponent::_Undo()
{
	GObject* node = GetGameObject(m_goID);
	if (node)
	{
		node->RemoveComponent(m_compName);
	}
}

NS_JYE_END