
#pragma once

#include "PluginEntry.h"
#include "LogSystem.h"

NS_JYE_BEGIN

static LogSystem* g_system = nullptr;

void LogSystemModulde::StartupModule()
{
	g_system = _NEW LogSystem();
}

void LogSystemModulde::ShutdownModule()
{
	SAFE_DELETE(g_system);
}

IModuleInterface* CreateSystem()
{
	return _NEW LogSystemModulde();
}

void DestroySystem(IModuleInterface* plugin)
{
	plugin->ShutdownModule();
	SAFE_DELETE(plugin);
}

NS_JYE_END