
#pragma once

#include "PluginEntry.h"
#include "RenderPipelineSystem.h"
#include "Core/Interface/ILogSystem.h"

NS_JYE_BEGIN

static RenderPipelineSystem* g_pipelinesystem = nullptr;

void PipelineSystemModulde::StartupModule()
{
	g_pipelinesystem = _NEW RenderPipelineSystem();
}

void PipelineSystemModulde::ShutdownModule()
{
	SAFE_DELETE(g_pipelinesystem);
}

IModuleInterface* CreateSystem()
{
	JYLOG("render pipeline system loaded!");
	return _NEW PipelineSystemModulde();
}

void DestroySystem(IModuleInterface* plugin)
{
	JYLOG("render pipeline system destory!");
	plugin->ShutdownModule();
	SAFE_DELETE(plugin);
}

NS_JYE_END