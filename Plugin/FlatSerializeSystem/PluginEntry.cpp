
#pragma once

#include "PluginEntry.h"
#include "FlatSerializerSystem.h"
#include "Core/Interface/ILogSystem.h"

NS_JYE_BEGIN

static FlatSerializerSystem* g_system = nullptr;

void FlatSerializeSystemModulde::StartupModule()
{
	g_system = _NEW FlatSerializerSystem();
}

void FlatSerializeSystemModulde::ShutdownModule()
{
	SAFE_DELETE(g_system);
}

IModuleInterface* CreateSystem()
{
	JYLOG("time system loaded!");
	return _NEW FlatSerializeSystemModulde();
}

void DestroySystem(IModuleInterface* plugin)
{
	JYLOG("time system destory!");
	plugin->ShutdownModule();
	SAFE_DELETE(plugin);
}

NS_JYE_END