
#pragma once

#include "PluginEntry.h"
#include "TimeSystem.h"
#include "Core/Interface/ILogSystem.h"

NS_JYE_BEGIN

static TimeSystem* g_system = nullptr;

void TimeSystemModulde::StartupModule()
{
	g_system = _NEW TimeSystem();
}

void TimeSystemModulde::ShutdownModule()
{
	SAFE_DELETE(g_system);
}

IModuleInterface* CreateSystem()
{
	JYLOG("time system loaded!");
	return _NEW TimeSystemModulde();
}

void DestroySystem(IModuleInterface* plugin)
{
	JYLOG("time system destory!");
	plugin->ShutdownModule();
	SAFE_DELETE(plugin);
}

NS_JYE_END