#include "MCmdDeserializeScene.h"
#include "Engine/object/SceneManager.h"
#include "Engine/object/Scene.h"
#include "util/MSceneManagerExt.h"
#include "../../../system/BundleManager.h"
#include "../../../system/EditorSystem.h"

NS_JYE_BEGIN

MCmdDeserializeScene::MCmdDeserializeScene(const String& scenefile)
	: m_scene(nullptr)
	, m_scenefile(scenefile)
{

}

MCmdDeserializeScene::~MCmdDeserializeScene()
{

}

Object* MCmdDeserializeScene::_DoIt()
{
	if (!m_scene)	
	{
		// scene视图的场景
		m_scene = BundleManager::Deserialize<Scene>(m_scenefile);
		MSceneManagerExt::Instance()->InitEditScene(m_scene);

		// game视图的场景
		Scene* sce = BundleManager::Deserialize<Scene>(m_scenefile);
		MSceneManagerExt::Instance()->InitPreviewScene(sce);
		MSceneManagerExt::Instance()->RegisterSyncScene(sce);
	}
	else
	{

	}
	return m_scene;
}

void MCmdDeserializeScene::_Undo()
{
	EditorSystem::Instance()->ClearScene();
}

NS_JYE_END