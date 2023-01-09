#pragma once

#include "private/Define.h"
#include "Core/Interface/IApplication.h"

NS_JYE_BEGIN

class IModuleManager;
class IVirtualMachine;

class Application : public IApplication
{
public:
	Application(IWindow* wd);
	virtual ~Application();

	virtual bool PreInitial() override;
	virtual bool OnInitial(const String& root, const String& save) override;
	virtual bool OnTerminal() override;
	virtual const String& GetConfigPath() { return m_ConfigPath; }

	FORCEINLINE void SetConfigPath(const String& custompath);

protected:

	String m_ConfigPath;

	IVirtualMachine* m_mainVM;

	IModuleManager* m_moduleManager{};
};

FORCEINLINE void Application::SetConfigPath(const std::string& custompath)
{
	m_ConfigPath = custompath;
}

NS_JYE_END
