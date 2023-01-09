#pragma once

#include "../Core.h"
#include "../../System/Singleton.hpp"
#include "../Propery/InputPropery.h"
#include "../Propery/TouchInput.h"
#include "../../System/Delegate.h"

NS_JYE_BEGIN

class IWindow;

class CORE_API IApplication
{
	SYSTEM_SINGLETON_DECLEAR(IApplication);
private:
	DECLARE_MULTICAST_DELEGATE(MainthreadCallback);
	DECLARE_MULTICAST_DELEGATE(ThreadSetupCallback);
	DECLARE_MULTICAST_DELEGATE(ThreadExitCallback);
	DECLARE_MULTICAST_DELEGATE(LostDeviceCallback);
	DECLARE_MULTICAST_DELEGATE(CleanCallback);
	DECLARE_MULTICAST_DELEGATE(CharCallback, uint);
	DECLARE_MULTICAST_DELEGATE(TouchCallback, const TouchInputs&);
	DECLARE_MULTICAST_DELEGATE(ScrollCallback, float, float);
	DECLARE_MULTICAST_DELEGATE(KeyboardCallback, InputPropery::Keyboard, InputPropery::KeyStatus);
public:
	IApplication(IWindow* mainwindow);
	~IApplication();

	IWindow* GetMainWindow()	{ return m_pMainWindow; }

	void Pasue();
	void Resume();

	virtual bool Update();
	void ThreadSetup();//线程初始化
	void ThreadExit();//线程退出
	void LostDevice();
	void Clean();
	void Char(uint c);
	void Scroll(float x, float y);
	void Touch(const TouchInputs& input);	//输入的ID属于区间[0,4]
	void OnKeyboard(uint ogikey, InputPropery::KeyStatus status);

	void SetMainthreadCallback(MainthreadCallbackDelegate& func) { m_MainthreadCallback.Add(std::move(func)); }
	void SetThreadSetupCallback(ThreadSetupCallbackDelegate& func) { m_ThreadSetupCallback.Add(std::move(func)); }
	void SetThreadExitCallback(ThreadExitCallbackDelegate& func) { m_ThreadExitCallback.Add(std::move(func)); }
	void SetLostDeviceCallback(LostDeviceCallbackDelegate& func) { m_LostDeviceCallback.Add(std::move(func)); }
	void SetCleanCallback(CleanCallbackDelegate& func) { m_CleanCallback.Add(std::move(func)); }
	void SetCharCallback(CharCallbackDelegate& func) { m_CharCallback.Add(std::move(func)); }
	void SetTouchCallback(TouchCallbackDelegate& func) { m_TouchPressCallback.Add(std::move(func)); }
	void SetScrollCallback(ScrollCallbackDelegate& func) { m_ScrollCallback.Add(std::move(func)); }
	void SetKeyboardCallback(KeyboardCallbackDelegate& func) { m_KeyboardCallback.Add(std::move(func)); }

	void SetNullRender(bool bNullRender) { m_isNullRender = bNullRender; }
	bool IsNullRender() const { return m_isNullRender; }

	virtual bool PreInitial() { return true; }
	virtual bool OnInitial(const String& root, const String& save);
	virtual bool OnTerminal() { return true; }
	virtual const String& GetConfigPath() = 0;


	FORCEINLINE void RegisterKeyMapping(uint id, InputPropery::Keyboard kb);
	FORCEINLINE void Abort();
	FORCEINLINE const String& GetRootpath() const;
	FORCEINLINE const String& GetSavepath() const;

protected:

	IWindow* m_pMainWindow;
	Map<uint, InputPropery::Keyboard> m_KeyMapping;
	bool m_isPause{ false };
	bool m_isNullRender{ false };
	volatile bool m_isRunning{ true };
	String m_RootPath;
	String m_SavePath;

	MainthreadCallback m_MainthreadCallback;
	ThreadSetupCallback m_ThreadSetupCallback;
	ThreadExitCallback m_ThreadExitCallback;
	LostDeviceCallback m_LostDeviceCallback;
	CleanCallback m_CleanCallback;
	CharCallback m_CharCallback;
	ScrollCallback m_ScrollCallback;
	TouchCallback m_TouchPressCallback;
	KeyboardCallback m_KeyboardCallback;
};

FORCEINLINE void IApplication::RegisterKeyMapping(uint id, InputPropery::Keyboard kb)
{
	m_KeyMapping.insert({ id, kb });
}

FORCEINLINE void IApplication::Abort()
{
	m_isRunning = false;
}

FORCEINLINE const String& IApplication::GetRootpath() const
{
	return m_RootPath;
}

FORCEINLINE const String& IApplication::GetSavepath() const
{
	return m_SavePath;
}

NS_JYE_END