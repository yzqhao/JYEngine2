#include "MCmdCreateDefaultCamera.h"
#include "Engine/object/SceneManager.h"
#include "Engine/object/Scene.h"
#include "util/MSceneManagerExt.h"
#include "Engine/component/camera/CameraComponent.h"

NS_JYE_BEGIN

MCmdCreateDefaultCamera::MCmdCreateDefaultCamera(uint64 id, bool isMain)
	: m_goID(id)
	, m_comID(0)
	, m_isMain(isMain)
{

}

MCmdCreateDefaultCamera::~MCmdCreateDefaultCamera()
{

}

Object* MCmdCreateDefaultCamera::_DoIt()
{
	CameraComponent* component = nullptr;
	GObject* node = GetGameObject(m_goID);
	if (node)
	{
		component = node->CreateComponent<CameraComponent>();
		component->Awake();
		component->CreatePerspectiveProjection(0.1, 1000);
		component->LookAt(Math::Vec3(0, 0, 1), Math::Vec3(0, 0, 0), Math::Vec3(0, 1, 0));
		component->Recalculate();
		component->SetActive(true); 
		if (m_comID == 0)
		{
			m_comID = component->GetStaticID();
		}
		else
		{
			component->SetStaticID(m_comID);
		}
		if (m_isMain)
		{
			Scene* sce = GetScene();
			sce->SetMainCamera(component);
		}
	}
	return component;
}

void MCmdCreateDefaultCamera::_Undo()
{
	GObject* node = GetGameObject(m_goID);
	if (node)
	{
		CameraComponent* component = node->TryGetComponent<CameraComponent>();
		node->RemoveComponent(component);
	}
}

NS_JYE_END