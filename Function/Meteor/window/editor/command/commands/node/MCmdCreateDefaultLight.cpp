#include "MCmdCreateDefaultLight.h"
#include "Engine/object/SceneManager.h"
#include "Engine/object/Scene.h"
#include "util/MSceneManagerExt.h"
#include "Engine/component/light/LightComponent.h"

NS_JYE_BEGIN

MCmdCreateDefaultLight::MCmdCreateDefaultLight(int64 id, GraphicDefine::LightType lt)
	: m_goID(id)
	, m_comID(0)
	, m_lighttype(lt)
{

}

MCmdCreateDefaultLight::~MCmdCreateDefaultLight()
{

}

Object* MCmdCreateDefaultLight::_DoIt()
{
	LightComponent* component = nullptr;
	GObject* node = GetGameObject(m_goID);
	if (node)
	{
		component = node->CreateComponent<LightComponent>();
		component->Awake();
		component->Setup(m_lighttype);
		component->SetColor(Math::Vec3(1, 1, 1));
		component->SetActive(true); 
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

void MCmdCreateDefaultLight::_Undo()
{
	GObject* node = GetGameObject(m_goID);
	if (node)
	{
		LightComponent* component = node->TryGetComponent<LightComponent>();
		node->RemoveComponent(component);
	}
}

NS_JYE_END