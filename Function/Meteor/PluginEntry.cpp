
#pragma once

#include "PluginEntry.h"
#include "Core/Interface/ILogSystem.h"
#include "Meteor.h"

NS_JYE_BEGIN

void MeteorModulde::StartupModule()
{
	Meteor::Create();
}

void MeteorModulde::ShutdownModule()
{
	Meteor::Destroy();
}

IModuleInterface* CreateSystem()
{
	JYLOG("imgui function loaded!");
	return _NEW MeteorModulde();
}

void DestroySystem(IModuleInterface* plugin)
{
	JYLOG("imgui function destory!");
	plugin->ShutdownModule();
	SAFE_DELETE(plugin);
}

NS_JYE_END