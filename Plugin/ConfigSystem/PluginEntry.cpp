
#pragma once

#include "PluginEntry.h"
#include "ConfigSystem.h"
#include "Core/Interface/ILogSystem.h"

NS_JYE_BEGIN

static ConfigSystem* g_system = nullptr;

void ConfigSystemModulde::StartupModule()
{
	g_system = _NEW ConfigSystem();
}

void ConfigSystemModulde::ShutdownModule()
{
	SAFE_DELETE(g_system);
}

IModuleInterface* CreateSystem()
{
	JYLOG("config system loaded!");
	return _NEW ConfigSystemModulde();
}

void DestroySystem(IModuleInterface* plugin)
{
	JYLOG("config system destory!");
	plugin->ShutdownModule();
	SAFE_DELETE(plugin);
}

NS_JYE_END