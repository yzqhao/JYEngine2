#pragma once

#include "Engine/private/Define.h"
#include "Core/Propery/InputPropery.h"
#include "Core/Propery/TouchInput.h"
#include "System/Delegate.h"
#include "ViewResolution.h"
#include "System/Singleton.hpp"

NS_JYE_BEGIN

class IVirtualMachine;

class ENGINE_API Engine
{
	SINGLETON_DECLARE_ONCREATE_ONDESTROY(Engine);
private:
	struct Agentdata
	{
		uint		Index;
		std::string Function;
	};
	typedef std::list<Agentdata*>	AgentdateList;
	DECLARE_MULTICAST_DELEGATE(UpdateCallback);
public:
	Engine() {}
	~Engine() {}
	void Clean();
	void OnLostDevice();
	void OnThreadSetup();
	void OnThreadExit();
	void OnMainthreadUpdate();
	void OnResizeview(int bx, int by, int w, int h, int vw, int vh);
	void LoadModule(const String& modulename);
	void OnTouch(const TouchInputs& touchs);
	void OnKeyboard(InputPropery::Keyboard key, InputPropery::KeyStatus status);

	void AddSynchronizeUpdateCallback(IVirtualMachine* agent, const std::string& func);
	void RemoveSynchronizeUpdateCallback(IVirtualMachine* agent, const std::string& func);
	void AddTouchCallback(IVirtualMachine* agent, const std::string& func);
	void RemoveTouchCallback(IVirtualMachine* agent, const std::string& func);
	void AddKeyboardCallback(IVirtualMachine* agent, const std::string& func);
	void RemoveKeyboardCallback(IVirtualMachine* agent, const std::string& func);

	FORCEINLINE void AddEarlyUpdateCallback(UpdateCallbackDelegate& func);
	FORCEINLINE void AddUpdateCallback(UpdateCallbackDelegate& func);
	FORCEINLINE void AddLateUpdateCallback(UpdateCallbackDelegate& func);
	FORCEINLINE const Math::IntVec4& GetMainViewSize();
	FORCEINLINE const Math::IntVec2& GetMainResolution();

private:
	void _OnCreate();
	void _OnDestroy();

	void _SyncUpdate();

	void _DoAddCallback(IVirtualMachine* vm, const std::string& func, AgentdateList& l);
	void _DoRemoveCallback(IVirtualMachine* vm, const std::string& func, AgentdateList& l);

	ViewResolution m_ViewResolution;
	UpdateCallback m_EarlyUpdateCallback;
	UpdateCallback m_UpdateCallback;
	UpdateCallback m_LateUpdateCallback;
	Vector<class IModuleInterface*> m_modules;

	AgentdateList m_SyncUpdateList;
	AgentdateList m_ActionList;
	AgentdateList m_ActionKeyboardList;
};

FORCEINLINE void Engine::AddEarlyUpdateCallback(UpdateCallbackDelegate& func)
{
	m_EarlyUpdateCallback.Add(std::move(func));
}

FORCEINLINE void Engine::AddUpdateCallback(UpdateCallbackDelegate& func)
{
	m_UpdateCallback.Add(std::move(func));
}

FORCEINLINE void Engine::AddLateUpdateCallback(UpdateCallbackDelegate& func)
{
	m_LateUpdateCallback.Add(std::move(func));
}

FORCEINLINE const Math::IntVec4& Engine::GetMainViewSize()
{
	return m_ViewResolution.m_ViewSize;
}

FORCEINLINE const Math::IntVec2& Engine::GetMainResolution()
{
	return m_ViewResolution.m_Resolution;
}

NS_JYE_END