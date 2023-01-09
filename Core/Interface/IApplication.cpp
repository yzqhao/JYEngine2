#include "IApplication.h"
#include "IWindow.h"

NS_JYE_BEGIN

SYSTEM_SINGLETON_IMPLEMENT(IApplication);

IApplication::IApplication(IWindow* mainwindow)
	: m_pMainWindow(mainwindow)
{
	SYSTEM_SINGLETON_INITIALIZE;
}

IApplication::~IApplication(void)
{
	SYSTEM_SINGLETON_DESTROY;
}

void IApplication::Pasue()
{
	m_isPause = true;
}

void IApplication::Resume()
{
	m_isPause = false;
}

bool IApplication::Update()
{
	if (m_isRunning && !m_isPause)
	{
		m_pMainWindow->OnBeginTick();
		m_MainthreadCallback.Broadcast();
		m_pMainWindow->OnEndTick();
	}
	return m_isRunning;
}

void IApplication::ThreadSetup()
{
	m_ThreadSetupCallback.Broadcast();
}

void IApplication::ThreadExit()
{
	m_ThreadExitCallback.Broadcast();
}

void IApplication::LostDevice()
{
	m_ThreadExitCallback.Broadcast();
}

void IApplication::Clean()
{
	m_CleanCallback.Broadcast();
}

void IApplication::Char(uint c)
{
	m_CharCallback.Broadcast(c);
}

void IApplication::Touch(const TouchInputs& input)
{
	m_TouchPressCallback.Broadcast(input);
}

void IApplication::Scroll(float x, float y)
{
	m_ScrollCallback.Broadcast(x, y);
}

void IApplication::OnKeyboard(uint ogikey, InputPropery::KeyStatus status)
{
	auto k = m_KeyMapping.find(ogikey);
	if (m_KeyMapping.end() != k)
	{
		m_KeyboardCallback.Broadcast(k->second, status);
	}
}

bool IApplication::OnInitial(const String& root, const String& save)
{
	m_RootPath = root;
	m_SavePath = save;
	return true;
}

NS_JYE_END