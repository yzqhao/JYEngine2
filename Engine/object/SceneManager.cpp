
#include "SceneManager.h"
#include "Scene.h"
#include "Engine/pipeline/PipelineSystem.h"

NS_JYE_BEGIN

IMPLEMENT_RTTI(SceneManager, Object);
BEGIN_ADD_PROPERTY(SceneManager, Object);
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(SceneManager)
IMPLEMENT_INITIAL_END

SINGLETON_IMPLEMENT(SceneManager);

SceneManager::SceneManager()
	: m_UtilityScene(nullptr)
{
	m_UtilityScene = CreateScene("InternelUitlity");  //默认创建一个引擎内部使用的Scene
	m_UtilityScene->SetSequence(INT_MIN);  //保证内部UtilityScene最先渲染
}

SceneManager::~SceneManager()
{
	for (auto s : m_Scenes)
	{
		s.second->ClearNodes();
		SAFE_DELETE(s.second);
	}
}

Scene* SceneManager::CreateScene(const String& sceneName)
{
	uint64 staticId = m_RandomGenerator();
	Scene* scene = _NEW Scene(staticId, sceneName);
	m_Scenes[scene->GetStaticID()] = scene;
	PipelineSystem::Instance()->AttachScene(scene);
	return scene;
}

void SceneManager::InsertScene(Scene* sce)
{
	m_Scenes[sce->GetStaticID()] = sce;
	PipelineSystem::Instance()->AttachScene(sce);
}

void SceneManager::DeleteSceneByName(const String& sceneName)
{
	for (auto s : m_Scenes)
	{
		if (s.second->GetName() == sceneName)
		{
			s.second->ClearNodes();
			PipelineSystem::Instance()->DetachScene(s.second);
			s.second->ClearNodes();
			SAFE_DELETE(s.second);
			m_Scenes.erase(s.first);
			break;
		}
	}
}

void JYE::SceneManager::DeleteScene(Scene* sce)
{
	auto it = m_Scenes.find(sce->GetStaticID());
	if (it != m_Scenes.end())
	{
		PipelineSystem::Instance()->DetachScene(it->second);
		it->second->ClearNodes();
		SAFE_DELETE(it->second);
		m_Scenes.erase(it->first);
	}
	else
	{
		JYLOG("delete scene nof find");
	}
}

Scene* SceneManager::GetScene(const String& sceneName) const
{
	for (auto s : m_Scenes)
	{
		if (s.second->GetName() == sceneName)
		{
			return s.second;
		}
	}
	return nullptr;
}

Scene* SceneManager::GetScenebyId(uint64 staticId) const
{
	auto itr = m_Scenes.find(staticId);
	if (itr != m_Scenes.end())
	{
		return itr->second;
	}
	return nullptr;
}

Scene* SceneManager::GetUtilityScene() const
{
	return m_UtilityScene;
}

NS_JYE_END