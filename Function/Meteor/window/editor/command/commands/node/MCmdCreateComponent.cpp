#include "MCmdCreateComponent.h"
#include "Engine/object/SceneManager.h"
#include "Engine/object/Scene.h"
#include "util/MSceneManagerExt.h"
NS_JYE_BEGIN

MCmdCreateComponent::MCmdCreateComponent(int64 id, const String& comname)
	: m_goID(id)
	, m_comID(0)
	, m_compName(comname)
{

}

MCmdCreateComponent::~MCmdCreateComponent()
{

}

Object* MCmdCreateComponent::_DoIt()
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
	return component;
}

void MCmdCreateComponent::_Undo()
{
	GObject* node = GetGameObject(m_goID);
	if (node)
	{
		node->RemoveComponent(m_compName);
	}
}

NS_JYE_END