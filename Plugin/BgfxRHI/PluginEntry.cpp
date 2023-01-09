
#pragma once

#include "PluginEntry.h"
#include "BgfxRHI.h"
#include "Core/Interface/ILogSystem.h"

NS_JYE_BEGIN

IModuleInterface* CreateSystem()
{
	JYLOGTAG(LogBgfxRHI, "bgfx rhi loaded!");
	return _NEW FBgfxDynamicRHIModule();
}

void DestroySystem(IModuleInterface* plugin)
{
	JYLOGTAG(LogBgfxRHI, "bgfx rhi destory!");
	plugin->ShutdownModule();
	SAFE_DELETE(plugin);
}

NS_JYE_END