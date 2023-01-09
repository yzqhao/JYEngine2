#include "MSceneManagerExt.h"
#include "MSceneExt.h"
#include "Engine/component/camera/CameraComponent.h"

NS_JYE_BEGIN

SINGLETON_IMPLEMENT(MSceneManagerExt);

MSceneManagerExt::MSceneManagerExt()
	: m_PreviewSceneID(0)
	, m_EditSceneID(0)
	, m_EditCamera(nullptr)
	, m_Grid(nullptr)
{

}

MSceneManagerExt::~MSceneManagerExt()
{

}

void MSceneManagerExt::_OnCreate()
{

}

void MSceneManagerExt::_OnDestroy()
{

}

void MSceneManagerExt::InitEditScene(Scene* sce)
{
	MSceneExt::SceneInit(sce);
	m_EditCamera = MSceneExt::SceneCreateEditorCamera(sce);
	m_Grid = MSceneExt::SceneCreateGrid(sce);
	m_EditSceneID = sce->GetStaticID();
	sce->SetEditorCamera(m_EditCamera->TryGetComponent<CameraComponent>());
}

void MSceneManagerExt::InitPreviewScene(Scene* sce)
{
	sce->SetSequence(MSceneExt::MID);
	sce->CreateDefaultRenderTarget(Math::IntVec2(128, 128));
	m_PreviewSceneID = sce->GetStaticID();
}

Scene* MSceneManagerExt::GetEditScene()
{
	Scene* res = nullptr;
	if (m_EditSceneID != 0)
	{
		res = SceneManager::Instance()->GetScenebyId(m_EditSceneID);
	}
	return res;
}

Scene* MSceneManagerExt::GetPreviewScene()
{
	Scene* res = nullptr;
	if (m_PreviewSceneID != 0)
	{
		res = SceneManager::Instance()->GetScenebyId(m_PreviewSceneID);
	}
	return res;
}

void MSceneManagerExt::RegisterSyncScene(Scene* sce)
{
	if (std::find(m_sceneList.begin(), m_sceneList.end(), sce) == m_sceneList.end())
	{
		m_sceneList.push_back(sce);
	}
}

void MSceneManagerExt::ClearScene()
{
	Scene* editsce = GetEditScene();
	if (editsce)
	{
		m_EditSceneID = 0;
		m_PreviewSceneID = 0;
		m_EditCamera = nullptr;
		m_Grid = nullptr;
		editsce->SetMainCamera(nullptr);
		SceneManager::Instance()->DeleteScene(editsce);
	}

	for (auto& it : m_sceneList)
	{
		SceneManager::Instance()->DeleteScene(it);
	}
}


NS_JYE_END