#pragma once

#include "IModuleManager.h"

NS_JYE_BEGIN

SYSTEM_SINGLETON_IMPLEMENT(IModuleManager);

IModuleManager::IModuleManager()
{
	SYSTEM_SINGLETON_INITIALIZE;
}

IModuleManager::~IModuleManager()
{
	SYSTEM_SINGLETON_DESTROY;
}

IModuleInterface* IModuleManager::LoadPlugin(const String& name)
{
	return _DoLoadPlugin(name);
}

void IModuleManager::ReleasePlugin(IModuleInterface* plugin)
{
	_DoReleasePlugin(plugin);
}

NS_JYE_END