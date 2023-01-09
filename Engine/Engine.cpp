#include "Engine.h"
#include "Core/Interface/ITimeSystem.h"
#include "Core/Interface/ILogSystem.h"
#include "Core/Interface/IFileSystem.h"
#include "Core/Interface/IResourceSystem.h"
#include "Core/Interface/IWindow.h"
#include "Core/Interface/IApplication.h"
#include "Core/Interface/IScriptSystem.h"
#include "Core/Interface/IConfigSystem.h"
#include "Engine/platform/PlatformEngine.h"
#include "IGraphicSystem.h"
#include "Core/Main.h"
#include "object/SceneManager.h"
#include "pipeline/PipelineSystem.h"
#include "RHI/RHI.h"
#include "ProjectSetting.h"
#include "queue/RenderQueueSystem.h"
#include "queue/LightQueueSystem.h"

NS_JYE_BEGIN

SINGLETON_IMPLEMENT(Engine);

static IModuleInterface* g_iterfaceFileSys = NULL;
static IModuleInterface* g_iterfaceResourceSys = NULL;
static IModuleInterface* g_iterfaceLuaScriptSys = NULL;
static IModuleInterface* g_iterfacePipelineSys = NULL;

static Vector<String> g_moduleNames =
{
	"LogSystem",
	"TimeSystem",
	"FileSystem",
	"ConfigSystem",
	"ResourceSystem",
	//"LuaScriptSystem",
#ifdef _EDITOR
	"MeteorEditor",
#endif
	"RenderPipelineSystem",

	// plugin
	"FlatSerializeSystem",
};

void Engine::_OnCreate()
{
	Main::Initialize();

	m_modules.reserve(g_moduleNames.size());
	for (auto& mn : g_moduleNames)
	{
		LoadModule(mn);
	}

	IGraphicSystem::Create();
	IGraphicSystem::Instance()->Initialize();

	RenderQueueSystem::Create();
	LightQueueSystem::Create();
	SceneManager::Create();
	ProjectSetting::Create();

	IWindow* mainwindows = IApplication::Instance()->GetMainWindow();
	mainwindows->SetResizeviewCallback(MulticastDelegate<int, int, int, int, int, int>::DelegateT::CreateRaw(this, & Engine::OnResizeview));

	int x, y;
	mainwindows->GetWindowSize(&x, &y);
	m_ViewResolution.m_Resolution = Math::IntVec2(x, y);
	m_ViewResolution.m_ViewSize = Math::IntVec4(0, 0, x, y);

	IConfigSystem::Instance()->SetDefaultConfigure(IApplication::Instance()->GetConfigPath());

	RHIInit();

	IApplication::Instance()->SetMainthreadCallback(MulticastDelegate<>::DelegateT::CreateRaw(this, &Engine::OnMainthreadUpdate));
	IApplication::Instance()->SetTouchCallback(MulticastDelegate<const TouchInputs&>::DelegateT::CreateRaw(this, &Engine::OnTouch));
	IApplication::Instance()->SetKeyboardCallback(MulticastDelegate<InputPropery::Keyboard, InputPropery::KeyStatus>::DelegateT::CreateRaw(this, &Engine::OnKeyboard));
	IApplication::Instance()->SetThreadSetupCallback(MulticastDelegate<>::DelegateT::CreateRaw(this, &Engine::OnThreadSetup));
	IApplication::Instance()->SetThreadExitCallback(MulticastDelegate<>::DelegateT::CreateRaw(this, &Engine::OnThreadExit));
	IApplication::Instance()->SetLostDeviceCallback(MulticastDelegate<>::DelegateT::CreateRaw(this, &Engine::OnLostDevice));
	IApplication::Instance()->SetCleanCallback(MulticastDelegate<>::DelegateT::CreateRaw(this, &Engine::Clean));
}

void Engine::_OnDestroy()
{
	RHIExit();

	ProjectSetting::Destroy();
	SceneManager::Destroy();
	LightQueueSystem::Create();
	RenderQueueSystem::Create();
	IGraphicSystem::Destroy();

	for (auto& mn : m_modules)
	{
		ModuleManager::Instance()->ReleasePlugin(mn);
	}

	Main::Terminate();
}

void Engine::_SyncUpdate()
{
	if (!m_SyncUpdateList.empty())
	{
		AgentdateList faillist;
		for (Agentdata* data : m_SyncUpdateList)
		{
			IVirtualMachine* vm = IScriptSystem::Instance()->GetVirtualMachineByIdx(data->Index);
			vm->CallScriptFunc(data->Function);
		}
	}
}

void Engine::_DoAddCallback(IVirtualMachine* vm, const std::string& func, AgentdateList& l)
{
	uint index = vm->GetIndex();
	if (nullhandle == index)
	{
		JYERROR("can't add update function with name %s", func.c_str());
	}
	else
	{
		Agentdata* data = _NEW Agentdata;
		data->Function = func;
		data->Index = index;
		l.push_back(data);
	}
}

void Engine::_DoRemoveCallback(IVirtualMachine* vm, const std::string& func, AgentdateList& l)
{
	for (AgentdateList::iterator it = l.begin();
		l.end() != it; )
	{
		Agentdata* data = *it;
		if (data->Index == vm->GetIndex() && data->Function == func)
		{
			AgentdateList::iterator temp = it++;
			l.erase(temp);
			SAFE_DELETE(data);
		}
		else
		{
			++it;
		}
	}
}

void Engine::Clean()
{

}

void Engine::OnLostDevice()
{

}

void Engine::OnThreadSetup()
{

}

void Engine::OnThreadExit()
{

}

void Engine::OnMainthreadUpdate()
{
	ITimeSystem::Instance()->UpdateFPS();
	float fTime = ITimeSystem::Instance()->GetGamePlayTime();
	float fFPS = ITimeSystem::Instance()->GetFPS();
	float dt = ITimeSystem::Instance()->GetDetTime();
	//JYLOG("time : %f, fps : %f", fTime, fFPS);

	m_EarlyUpdateCallback.Broadcast();	// 外部注册，主要用来更新imgui插件
	_SyncUpdate();
	m_UpdateCallback.Broadcast();		// 外部注册，主要用来更新编辑器逻辑
	m_LateUpdateCallback.Broadcast();	// 外部注册，主要用来更新imgui插件

	IGraphicSystem::Instance()->Update();	// 暂时没用
	PipelineSystem::Instance()->Update(dt);	// 更新场景里gameobject和component
	RenderQueueSystem::Instance()->Update();// 用来计算每个render组件包围盒，并计算整个场景包围盒
	LightQueueSystem::Instance()->Update();	// 计算灯光参数和通过包围盒处理阴影

	IGraphicSystem::Instance()->Render();	// 渲染线程，最后提交rhi指令
}

void Engine::OnResizeview(int bx, int by, int w, int h, int vw, int vh)
{
	m_ViewResolution.m_Resolution = Math::IntVec2(w, h);
	m_ViewResolution.m_ViewSize = Math::IntVec4(bx, by, vw, vh);
}

void Engine::LoadModule(const String& modulename)
{
	m_modules.push_back(static_cast<IModuleInterface*>(ModuleManager::Instance()->LoadPlugin(modulename)));
}

void Engine::OnTouch(const TouchInputs& touchs)
{

}

void Engine::OnKeyboard(InputPropery::Keyboard key, InputPropery::KeyStatus status)
{
	
}

void Engine::AddSynchronizeUpdateCallback(IVirtualMachine* agent, const std::string& func)
{
	_DoAddCallback(agent, func, m_SyncUpdateList);
}

void Engine::RemoveSynchronizeUpdateCallback(IVirtualMachine* agent, const std::string& func)
{
	_DoRemoveCallback(agent, func, m_SyncUpdateList);
}

void Engine::AddTouchCallback(IVirtualMachine* agent, const std::string& func)
{
	_DoAddCallback(agent, func, m_ActionList);
}

void Engine::RemoveTouchCallback(IVirtualMachine* agent, const std::string& func)
{
	_DoRemoveCallback(agent, func, m_ActionList);
}

void Engine::AddKeyboardCallback(IVirtualMachine* agent, const std::string& func)
{
	_DoAddCallback(agent, func, m_ActionKeyboardList);
}

void Engine::RemoveKeyboardCallback(IVirtualMachine* agent, const std::string& func)
{
	_DoRemoveCallback(agent, func, m_ActionKeyboardList);
}

NS_JYE_END