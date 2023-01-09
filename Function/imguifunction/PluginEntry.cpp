
#pragma once

#include "PluginEntry.h"
#include "Core/Interface/ILogSystem.h"
#include "AdapterImgui.h"

NS_JYE_BEGIN

void ImguiFunctionModulde::StartupModule()
{
	AdapterImgui::Create();
}

void ImguiFunctionModulde::ShutdownModule()
{
	AdapterImgui::Destroy();
}

IModuleInterface* CreateSystem()
{
	JYLOG("imgui function loaded!");
	return _NEW ImguiFunctionModulde();
}

void DestroySystem(IModuleInterface* plugin)
{
	JYLOG("imgui function destory!");
	plugin->ShutdownModule();
	SAFE_DELETE(plugin);
}

NS_JYE_END