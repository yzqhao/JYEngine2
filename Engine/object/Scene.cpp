
#include "Scene.h"
#include "SceneManager.h"
#include "Engine/component/camera/CameraComponent.h"
#include "Core/Configure.h"
#include "Engine/queue/IRenderQueue.h"
#include "Engine/queue/ILightQueue.h"
#include "Engine/render/RenderTargetEntity.h"
#include "Engine/resource/RenderTargetMetadata.h"
#include "Engine/resource/TextureMetadata.h"
#include "Engine/component/light/LightComponent.h"
#include "Engine/component/render/RenderComponent.h"
#include "Engine/queue/RenderQueueSystem.h"
#include "Engine/queue/LightQueueSystem.h"
#include "Engine/queue/RenderQueueSystem.h"

#include <functional>

NS_JYE_BEGIN

IMPLEMENT_RTTI(Scene, Object);
BEGIN_ADD_PROPERTY(Scene, Object);
REGISTER_PROPERTY(m_rootObject, m_rootObject, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_name, m_name, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_Sequence, m_Sequence, Property::F_SAVE_LOAD_CLONE)
//REGISTER_PROPERTY(m_StaticID, m_StaticID, Property::F_SAVE_LOAD_CLONE)	// 不序列号id，只作为管理器当作key索引
REGISTER_PROPERTY(m_MainCamera, m_MainCamera, Property::F_SAVE_LOAD_CLONE)
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(Scene)
IMPLEMENT_INITIAL_END

Scene::Scene()
	: m_rootObject(nullptr)
	, m_renderQueue(NULL)
	, m_lightQueue(NULL)
	, m_Sequence(0)
	, m_StaticID(ObjectIDAllocator::AllocID())
	, m_MainCamera(nullptr)
#ifdef _EDITOR
	, m_editorCamera(nullptr)
	, m_pDefaultRenderTarget(nullptr)
#endif
{
	m_renderQueue = RenderQueueSystem::Instance()->CreateRenderQueue(this);
	m_lightQueue = LightQueueSystem::Instance()->CreateEnvironment();
}

Scene::Scene(uint64 sid, const String& name)
	: m_name(name)
	, m_rootObject(nullptr)
	, m_renderQueue(NULL)
	, m_lightQueue(NULL)
	, m_StaticID(sid)
	, m_MainCamera(nullptr)
#ifdef _EDITOR
	, m_editorCamera(nullptr)
	, m_pDefaultRenderTarget(nullptr)
#endif
{
	m_renderQueue = RenderQueueSystem::Instance()->CreateRenderQueue(this);
	m_lightQueue = LightQueueSystem::Instance()->CreateEnvironment();

	m_rootObject = CreateObject("root_node");
}

Scene::~Scene()
{
	SAFE_DELETE(m_renderQueue);
	SAFE_DELETE(m_lightQueue);
}

void Scene::Tick(float dt)
{
	m_pWindowCameraSequence.clear();
	for (auto camera : m_pAttachedCameras)
	{
		if (camera.second->isActiveHierarchy())
		{
			IWindow* win = camera.second->GetRenderWindow();
			WindowCameraSequence::iterator it = m_pWindowCameraSequence.find(win);
			if (m_pWindowCameraSequence.end() == it)
			{
				std::vector<CameraComponent*> sc;
				m_pWindowCameraSequence.insert({ win, sc });
			}
			m_pWindowCameraSequence.at(win).push_back(camera.second);
		}
	}
	for (auto& itcameras : m_pWindowCameraSequence)
	{
		std::sort(itcameras.second.begin(), itcameras.second.end(),
			[&](CameraComponent* ra, CameraComponent* rb) -> bool
			{
				return ra->GetSequence() < rb->GetSequence();;
			});
	}
	
	// updata scene
	_OnStart(m_rootObject);
	_OnUpdate(m_rootObject, dt);
	_OnLateUpdate(m_rootObject, dt);
}

#ifdef _EDITOR
void Scene::SetEditorCamera(CameraComponent* camera)
{
	m_editorCamera = camera;
}

RenderTargetEntity* Scene::CreateDefaultRenderTarget(const Math::IntVec2& resolution)
{
	if (!m_pDefaultRenderTarget)
	{
		JY_ASSERT(NULL == m_pDefaultRenderTarget);
		m_pDefaultRenderTarget = _NEW RenderTargetEntity();
		m_pDefaultRenderTarget->PushMetadata(
			RenderTargetMetadata(
				RHIDefine::RT_RENDER_TARGET_2D,
				Math::IntVec4(0, 0, resolution.x, resolution.y),
				resolution,
				Math::Vec2(1.0f, 1.0f)));

		TextureEntity* texEnt = m_pDefaultRenderTarget->MakeTextureAttachment(RHIDefine::TA_COLOR_0);
		texEnt->PushMetadata(TextureBufferMetadata(
			resolution,
			RHIDefine::TextureType::TEXTURE_2D,
			RHIDefine::TextureUseage::TU_STATIC,
			RHIDefine::PixelFormat::PF_R8G8B8A8,
			1, false, 0,
			RHIDefine::TextureWarp::TW_REPEAT,
			RHIDefine::TextureWarp::TW_REPEAT,
			RHIDefine::TextureFilter::TF_NEAREST,
			RHIDefine::TextureFilter::TF_NEAREST,
			(byte*)NULL));

		TextureEntity* depTexEnt = m_pDefaultRenderTarget->MakeTextureAttachment(RHIDefine::TA_DEPTH_STENCIL);
		depTexEnt->PushMetadata(DepthRenderBufferMetadata(
			resolution,
			RHIDefine::PixelFormat::PF_DEPTH24_STENCIL8
		));
		m_pDefaultRenderTarget->CreateResource();
	}
	return m_pDefaultRenderTarget;
}

RenderTargetEntity* Scene::GetDefaultRenderTarget() const
{
	return m_pDefaultRenderTarget;
}

void Scene::ChangeDefaultResolution(const Math::IntVec2& resolution)
{
	m_ChangeResolution = resolution;
	for (auto camera : m_pAttachedCameras)
	{
		GObject* node = camera.second->GetParentObject();
		if (!node->isLayer(MaskConstant::MC_MASK_EDITOR_UI_LAYER))
		{
			camera.second->ChangeResolution(resolution);
		}
	}
}

void Scene::AddToScene(GObject* go)
{
	m_gobjects.insert({go->GetStaticID(), go});
}

int Scene::Render(IWindow* win, RenderContext& context)
{
	uint drawcall = 0;
	WindowCameraSequence::iterator it = m_pWindowCameraSequence.find(win);

	bool isEditorScene = m_editorCamera != NULL;
	if (m_pWindowCameraSequence.end() != it)
	{
		for (auto camera : it->second)
		{
			if (!isEditorScene || (camera->GetLayerMask() & MaskConstant::MC_MASK_EDITOR_SCENE_LAYER))
			{
				drawcall += camera->Render(context);
			}
		}
	}
	return drawcall;
}
#else
int Scene::Render(IWindow* win, RenderContext& contex)
{
	uint drawcall = 0;
	WindowCameraSequence::iterator it = m_pWindowCameraSequence.find(win);

	if (m_pWindowCameraSequence.end() != it)
	{
		for (auto camera : it->second)
		{
			drawcall += camera->Render(contex);
		}
	}
	return drawcall;
}
#endif

GObject* Scene::GetGObjectByID(uint64 go_id) const
{
	uint64 realid = go_id & s_gameobjectIDMask;
	auto iter = m_gobjects.find(realid);
	if (iter != m_gobjects.end())
	{
		return iter->second;
	}

	return nullptr;
}

void Scene::ClearNodes()
{
	DeleteGObject(m_rootObject);
}

GObject* Scene::CreateObject(const String& objName)
{
	GObject* node = _NEW GObject(this);
	if (m_rootObject)
	{
		m_rootObject->AddChild(node);
	}
	m_gobjects[node->GetStaticID()] = node;
	node->SetName(objName);

	node->RegisterChangeIDListener(MulticastDelegate<Animatable*, uint64, uint64>::DelegateT::CreateRaw(this, &Scene::_ChangeGObjectID));

	return node;
}

void Scene::_DoDeleteNode(GObject* node)
{
	auto itr = m_gobjects.find(node->GetStaticID());
	if (itr != m_gobjects.end())
	{
		const auto& nodes = node->GetChildren();
		for (auto it = nodes.begin(); nodes.end() != it;)
		{
			GObject* current = *it;
			++it;
			_DoDeleteNode(current);
		}
		itr->second->BeforeDelete();
		SAFE_DELETE(itr->second);
		m_gobjects.erase(itr);
	}
	else
	{
		JYERROR("error:Scene can't find node by Id!");
	}
}

void Scene::DeleteGObject(GObject* node)
{
	if (node == NULL)  return;

	//先把根节点置空，否则脚本删除了根节点，再删除scene时候会野指针
	if (m_rootObject && m_rootObject->GetStaticID() == node->GetStaticID())
		m_rootObject = NULL;

	GObject* root = node->GetRoot();
	if (root)
	{
		root->DetachNode(node);
	}
	
	_DoDeleteNode(node);
}

void Scene::DeleteGObjectByID(uint64 go_id)
{
	auto itr = m_gobjects.find(go_id);
	if (itr != m_gobjects.end())
	{
		DeleteGObject(itr->second);
	}
}

void Scene::AttachCamera(CameraComponent* com)
{
	m_pAttachedCameras.insert({ com->GetStaticID(), com });
}

void Scene::DetachCamera(CameraComponent* com)
{
	m_pAttachedCameras.erase(com->GetStaticID());
}

void Scene::SetMainCamera(CameraComponent* mainCamera)
{
	m_MainCamera = mainCamera;
}

void Scene::BeforeSave(Stream* pStream)
{
	
}

void Scene::PostSave(Stream* pStream)
{

}

void Scene::PostLoad(Stream* pStream)
{
	std::function<void(GObject*)> travScene = [&](GObject* pGObj) -> void
	{
		m_gobjects.insert({ pGObj->GetStaticID(), pGObj });	// 收集所有的gameobject
		for (auto childnode : pGObj->GetChildren())
		{
			travScene(childnode);
		}
	};
	travScene(m_rootObject);
	
	for (auto& it : m_gobjects)
	{
		it.second->SetScene(this);
	}
	

	// 更新场景管理器
	SceneManager::Instance()->InsertScene(this);
}

void Scene::_OnStart(GObject* node)
{
	if (node == nullptr) return;

	if (node->isActiveHierarchy())
	{
		// 先启动父节点
		node->Start();

		// 启动子节点
		for (auto childnode : node->GetChildren())
		{
			childnode->Start();
		}
	}
}

void Scene::_OnUpdate(GObject* node, float dt)
{
	if (node == nullptr) return;

	if (node->isActiveHierarchy())
	{	
		// 先更新父节点
		node->Update(dt);

		// 更新子节点
		for (auto childnode : node->GetChildren())
		{
			childnode->Update(dt);
		}
	}
}

void Scene::_OnLateUpdate(GObject* node, float dt)
{
	if (node == nullptr) return;

	if (node->isActiveHierarchy())
	{
		// 先更新父节点
		node->LateUpdate(dt);

		// 更新子节点
		for (auto childnode : node->GetChildren())
		{
			childnode->LateUpdate(dt);
		}
	}
}

void Scene::_ChangeGObjectID(Animatable* pAniAble, uint64 oldid, uint64 newid)
{
	auto itr = m_gobjects.find(oldid);
	Animatable* go = nullptr;
	if (itr != m_gobjects.end())
	{
		go = itr->second;
		JY_ASSERT(go == pAniAble);
		m_gobjects.erase(itr);
	}
	m_gobjects[newid] = static_cast<GObject*>(go);
}

void Scene::CloneEvent(uint64 goid, GObject* cloneNode)
{
	uint64 temp = GenerateID32();
	uint64 clonenodeid = temp << 32;
	cloneNode->SetStaticID(clonenodeid);
	m_gobjects[clonenodeid] = static_cast<GObject*>(cloneNode);
	m_rootObject->AddChild(cloneNode);
}

NS_JYE_END