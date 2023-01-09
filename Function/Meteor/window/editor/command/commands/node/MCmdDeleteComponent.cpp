#include "MCmdDeleteComponent.h"
#include "Engine/object/SceneManager.h"
#include "Engine/object/Scene.h"
#include "util/MSceneManagerExt.h"
#include "Engine/component/light/LightComponent.h"

NS_JYE_BEGIN

MCmdDeleteComponent::MCmdDeleteComponent(int64 id, const String& comname)
	: m_goID(id)
	, m_comID(0)
	, m_compName(comname)
{

}

MCmdDeleteComponent::~MCmdDeleteComponent()
{

}

Object* MCmdDeleteComponent::_DoIt()
{
	GObject* node = GetGameObject(m_goID);
	if (node)
	{
		node->RemoveComponent(m_compName);
	}
	return nullptr;
}

void MCmdDeleteComponent::_Undo()
{
	Component* component = nullptr;
	GObject* node = GetGameObject(m_goID);
	if (node)
	{
		component = node->CreateComponent(m_compName);
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
}

NS_JYE_END