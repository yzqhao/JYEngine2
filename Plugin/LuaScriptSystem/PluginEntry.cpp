
#pragma once

#include "PluginEntry.h"
#include "LuaScriptSystem.h"
#include "Core/Interface/ILogSystem.h"

NS_JYE_BEGIN

static LuaScriptSystem* g_resourcsystem = nullptr;

void LuaScriptSystemModulde::StartupModule()
{
	g_resourcsystem = _NEW LuaScriptSystem();
}

void LuaScriptSystemModulde::ShutdownModule()
{
	SAFE_DELETE(g_resourcsystem);
}

IModuleInterface* CreateSystem()
{
	JYLOG("script system loaded!");
	return _NEW LuaScriptSystemModulde();
}

void DestroySystem(IModuleInterface* plugin)
{
	JYLOG("script system destory!");
	plugin->ShutdownModule();
	SAFE_DELETE(plugin);
}

NS_JYE_END