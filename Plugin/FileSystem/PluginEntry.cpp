
#pragma once

#include "PluginEntry.h"
#include "FileSystem.h"
#include "Core/Interface/ILogSystem.h"

NS_JYE_BEGIN

static FileSystem* g_filesystem = nullptr;

void FileSystemModulde::StartupModule()
{
	g_filesystem = _NEW FileSystem();
}

void FileSystemModulde::ShutdownModule()
{
	SAFE_DELETE(g_filesystem);
}

IModuleInterface* CreateSystem()
{
	JYLOG("file system loaded!");
	return _NEW FileSystemModulde();
}

void DestroySystem(IModuleInterface* plugin)
{
	JYLOG("file system destory!");
	plugin->ShutdownModule();
	SAFE_DELETE(plugin);
}

NS_JYE_END