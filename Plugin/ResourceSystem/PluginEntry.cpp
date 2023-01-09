
#pragma once

#include "PluginEntry.h"
#include "ResourceSystem.h"
#include "Core/Interface/ILogSystem.h"

NS_JYE_BEGIN

static ResourceSystem* g_resourcsystem = nullptr;

void ResourceSystemModulde::StartupModule()
{
	g_resourcsystem = _NEW ResourceSystem();
}

void ResourceSystemModulde::ShutdownModule()
{
	SAFE_DELETE(g_resourcsystem);
}

IModuleInterface* CreateSystem()
{
	JYLOG("resource system loaded!");
	return _NEW ResourceSystemModulde();
}

void DestroySystem(IModuleInterface* plugin)
{
	JYLOG("resource system destory!");
	plugin->ShutdownModule();
	SAFE_DELETE(plugin);
}

NS_JYE_END