
#pragma once

#include "../../private/Define.h"
#include "Core/Interface/IModuleManager.h"

#if PLATFORM_WIN32

#include <windows.h>

NS_JYE_BEGIN

class ENGINE_API ModuleManager : public IModuleManager
{
public:
	ModuleManager();
	virtual ~ModuleManager();

	virtual void AddLibraryPath(const String& path) override;

private:
	virtual IModuleInterface* _DoLoadPlugin(const String& name);
	virtual void _DoReleasePlugin(IModuleInterface* plugin);

	struct PluginData
	{
		String	m_Name;
		HMODULE m_Handle;
		IModuleInterface* m_pPlugin;
	};
	Map< uint64, PluginData > m_LoadedPluginMap;
};

NS_JYE_END

#endif