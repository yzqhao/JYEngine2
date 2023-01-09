#include "EditorSystem.h"
#include "MDefined.h"
#include "Engine/resource/TextureMetadata.h"
#include "util/MFileUtility.h"
#include "util/MSceneManagerExt.h"
#include "Engine/component/transform/TransformComponent.h"
#include "Engine/object/GObject.h"
#include "BundleManager.h"
#include "Core/Interface/IFileSystem.h"
#include "Engine/object/SceneManager.h"
#include "../command/MCommandManager.h"
#include "../command/commands/scene/MCmdDeserializeScene.h"
#include "Engine/component/camera/CameraComponent.h"
#include "Engine/component/light/LightComponent.h"
#include "Engine/render/RenderTargetEntity.h"
#include "../command/commands/scene/MCmdCreateGenericNode.h"
#include "../command/commands/node/MCmdCreateDefaultCamera.h"
#include "../command/commands/node/MCmdCreateDefaultLight.h"
#include "../command/commands/MCmdPropertyChange.h"
#include "../command/commands/scene/MCmdDestoryNode.h"
#include "WindowSystem.h"


NS_JYE_BEGIN

const static String CameraDefaultName = "cameranode";
const static String LightDefaultNamse[GraphicDefine::LI_COUNT] =
{
	"AmbientLight",
	"DirectionLight",
	"PointLight",
	"SpotLight"
};

SINGLETON_IMPLEMENT(EditorSystem);

EditorSystem::EditorSystem()
	: m_prefabMode(false)
	, m_bshowGrid(true)
	, m_showEverything(false)
	//, m_EditorStatus(MDefined::STOP)
{

}

EditorSystem::~EditorSystem()
{

}

void EditorSystem::_OnCreate()
{
	for (auto& it : MDefined::Icons)
	{
		TextureEntity* tex = _NEW TextureEntity();
		tex->PushMetadata(
			TextureFileMetadata(
				RHIDefine::TEXTURE_2D,
				RHIDefine::TU_STATIC,
				RHIDefine::PixelFormat::PF_AUTO,
				1,
				true,
				0,
				RHIDefine::TextureWarp::TW_REPEAT,
				RHIDefine::TextureWarp::TW_REPEAT,
				RHIDefine::TextureFilter::TF_LINEAR,
				RHIDefine::TextureFilter::TF_LINEAR,
				it.second, true, false));
		tex->CreateResource();
		m_icons[it.first] = tex;
	}
}

void EditorSystem::_OnDestroy()
{

}

void EditorSystem::Update(float dt)
{
	GObject* selected = m_select.GetSelectedGObject();
	if (selected)
	{
		CameraComponent* cameraComponent = selected->TryGetComponent<CameraComponent>();
		if (cameraComponent)
			cameraComponent->Recalculate();
	}
}

void EditorSystem::OpenScene(const String& scenefile, OpenSceneCaller& openevent)
{
	m_openSceneCaller = std::move(openevent);
	m_sceneFile = scenefile;

	String parentname, filename, parentpath, assetpath;
	MFileUtility::GetParentDirectoryName(scenefile, parentname);
	MFileUtility::GetParentDirectory(scenefile, parentpath);
	MFileUtility::GetParentDirectory(parentpath, assetpath);
	MFileUtility::getFileNameNotPosfix(scenefile, filename);
	if (parentname == filename)
	{
		String assetpath = MFileUtility::GetRelativePath(parentpath, {});
		SetAssetsPath(assetpath);
	}
}

static void __OnOpenSceneEvent()
{
	EditorSystem::Instance()->OnOpenSceneEvent();
}

void EditorSystem::SetAssetsPath(const String& path)
{
	if (GetPrefabMode())
	{
		return;
	}

	MDefined::SystemDirectories["scene_asset"] = path;
	if (m_OnAssetsPathChangeCaller.GetOwner())
	{
		m_OnAssetsPathChangeCaller.Execute(StaticDelegate<void()>(&__OnOpenSceneEvent));
	}
}

void EditorSystem::CreateScene(const String& path, const String& name)
{
	Scene* sce = SceneManager::Instance()->CreateScene(MDefined::EditorSceneName);	// 创建一个新的场景
	MSceneManagerExt::Instance()->InitEditScene(sce);
	DefaultScene(); // 默认创建一个场景摄像机
	m_sceneFile = path + name + ".scene";
	MFileUtility::MCreateDirectory(path);
	BundleManager::SaveScene(m_sceneFile);
	String assetspath;
	MFileUtility::ConverToRelativePath(path, MDefined::ProjectPath, assetspath);
	SetAssetsPath(assetspath);
}

void EditorSystem::OnOpenSceneEvent()
{
	if (m_openSceneCaller.GetOwner())
	{
		CreateObjectFrombundle(m_sceneFile);
		m_openSceneCaller.Execute();
	}
}

void EditorSystem::CreateObjectFrombundle(const String& filename)
{
	String path = IFileSystem::Instance()->PathAssembly(filename);

	Scene* sce = MCommandManager::Instance()->Execute<MCmdDeserializeScene>(path);

	MCommandManager::Instance()->Reset();	// 序列化场景需要把指令清空
}

void EditorSystem::DefaultScene()
{
	int64 editSceneID = MSceneManagerExt::Instance()->GetEditSceneID();
	//CommandManager:SetTriggerSceneID(editSceneID);
	// 创建默认相机
	GObject* node = MCommandManager::Instance()->Execute<MCmdCreateGenericNode>(CameraDefaultName);
	CameraComponent* cameracom = MCommandManager::Instance()->Execute<MCmdCreateDefaultCamera>(node->GetStaticID(), true);
	GObject* defaultLightNode = MCommandManager::Instance()->Execute<MCmdCreateGenericNode>(LightDefaultNamse[GraphicDefine::LT_DIRECTIONAL]);
	LightComponent* lightcom = MCommandManager::Instance()->Execute<MCmdCreateDefaultLight>(defaultLightNode->GetStaticID(), GraphicDefine::LT_DIRECTIONAL);
	TransformComponent* tcom = defaultLightNode->CreateComponent<TransformComponent>();
	Math::Vec3 angle(146 / 180 * Math::PI, 0, 0);
	Math::Quaternion rotate(angle);
	MCommandManager::Instance()->Execute<MCmdPropertyChangeQuaternion>(tcom->GetStaticID(), "m_LocalRotation", rotate);
}

void EditorSystem::CreateCameraNode(bool flag)
{
	M_CMD_EXEC_BEGIN;
	GObject* node = MCommandManager::Instance()->Execute<MCmdCreateGenericNode>(CameraDefaultName);
	CameraComponent* cameracom = MCommandManager::Instance()->Execute<MCmdCreateDefaultCamera>(node->GetStaticID(), flag);
	M_CMD_EXEC_END;
}

GObject* EditorSystem::CreateEmptyNode(const String& nodename)
{
	M_CMD_EXEC_BEGIN;
	GObject* node = MCommandManager::Instance()->Execute<MCmdCreateGenericNode>(nodename);
	M_CMD_EXEC_END;
	return node;
}

GObject* EditorSystem::CreateLight(GraphicDefine::LightType lt)
{
	M_CMD_EXEC_BEGIN;
	GObject* node = MCommandManager::Instance()->Execute<MCmdCreateGenericNode>(LightDefaultNamse[lt]);
	LightComponent* lightcom = MCommandManager::Instance()->Execute<MCmdCreateDefaultLight>(node->GetStaticID(), lt);
	M_CMD_EXEC_END;
	return node;
}

TextureEntity* EditorSystem::GetEditorCameraOutput()
{
	GObject* ediorCamera = MSceneManagerExt::Instance()->GetEditCamera();
	CameraComponent* cameraComponent = ediorCamera->TryGetComponent<CameraComponent>();
	RenderTargetEntity* rendertarget = cameraComponent->GetAttachedRenderTarget();
	return rendertarget->GetAttachment(RHIDefine::TA_COLOR_0);
}

GObject* EditorSystem::GetNodeByID(uint64 id)
{
	Scene* sce = MSceneManagerExt::Instance()->GetEditScene();
	return sce->GetGObjectByID(id);
}

void EditorSystem::Select(GObject* node)
{
	GObject* currentNode = GetSelectedGObject();
	if (currentNode)
	{
		if (currentNode == node)
		{
			return;	// 连续两次点中同一个node
		}

		LightComponent* currentlightcomponent = currentNode->TryGetComponent<LightComponent>();
		if (currentlightcomponent) 
			currentlightcomponent->SetLightShow(false);

		CameraComponent* currentcameracomponent = currentNode->TryGetComponent<CameraComponent>();
		if (currentcameracomponent)
			currentcameracomponent->SetGizmoShow(false);
	}

	if (node)
	{
		LightComponent* currentlightcomponent = node->TryGetComponent<LightComponent>();
		if (currentlightcomponent)
			currentlightcomponent->SetLightShow(true);

		CameraComponent* currentcameracomponent = node->TryGetComponent<CameraComponent>();
		if (currentcameracomponent)
			currentcameracomponent->SetGizmoShow(true);
	}

	m_select.Select(node);
}

void EditorSystem::Select(Scene* sce)
{
	m_select.Select(sce);
}

void EditorSystem::Select(const String& str)
{
	m_select.Select(str);
}

GObject* EditorSystem::GetSelectedGObject()
{
	return m_select.GetSelectedGObject();
}

Scene* EditorSystem::GetSelectedScene()
{
	return m_select.GetSelectedScene();
}

String* EditorSystem::GetSelectedString()
{
	return m_select.GetSelectedString();
}

void EditorSystem::Destroy(GObject* node)
{
	M_CMD_EXEC_BEGIN;
	MCommandManager::Instance()->Execute<MCmdDestoryNode>(node->GetStaticID());
	M_CMD_EXEC_END;
}

void EditorSystem::DestroyNodeTree(GObject* node)
{
	Destroy(node);
}

void EditorSystem::ClearScene()
{
	Select((GObject*)nullptr);
	WindowSystem::Instance()->ClearScene();
	MSceneManagerExt::Instance()->ClearScene();
}
#if 0
void EditorSystem::Play()
{
	m_EditorStatus = MDefined::PLAY;
}

void EditorSystem::Pause()
{
	if (IsPlay())
	{
		int status = m_EditorStatus | MDefined::PAUSE;
		m_EditorStatus = (MDefined::EditorStatus)status;
	}
}

void EditorSystem::Stop()
{
	if (IsPlay())
	{

	}
	m_EditorStatus = MDefined::STOP;
}
#endif
//-------------------- EditorSystemaSelect --------------------------- 
EditorSystem::EditorSystemaSelect::EditorSystemaSelect()
	: m_go(nullptr)
	, m_sce(nullptr)
	, m_str(_NEW String())
{

}

EditorSystem::EditorSystemaSelect::~EditorSystemaSelect()
{
	SAFE_DELETE(m_str);
}

void EditorSystem::EditorSystemaSelect::Select(GObject* node)
{
	m_go = node;
	m_selectType = TSelected_GObject;
}

void EditorSystem::EditorSystemaSelect::Select(Scene* sce)
{
	m_sce = sce;
	m_selectType = TSelected_Scene;
}

void EditorSystem::EditorSystemaSelect::Select(const String& str)
{
	*m_str = str;
	m_selectType = TSelected_String;
}

GObject* EditorSystem::EditorSystemaSelect::GetSelectedGObject()
{
	return m_selectType == TSelected_GObject ? m_go : nullptr;
}

Scene* EditorSystem::EditorSystemaSelect::GetSelectedScene()
{
	return m_selectType == TSelected_Scene ? m_sce : nullptr;
}

String* EditorSystem::EditorSystemaSelect::GetSelectedString()
{
	return m_selectType == TSelected_String ? m_str : nullptr;
}
//-------------------- EditorSystemaSelect --------------------------- 

NS_JYE_END