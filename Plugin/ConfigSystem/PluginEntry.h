
#pragma once

#include "private/Define.h"
#include "Core/Interface/IModuleInterface.h"

NS_JYE_BEGIN

class ConfigSystemModulde : public IModuleInterface
{
public:
	ConfigSystemModulde() {}
	virtual ~ConfigSystemModulde() {}

	virtual void StartupModule();
	virtual void ShutdownModule();
};

JY_EXTERN CONFIG_SYSTEM_API IModuleInterface* CreateSystem();
JY_EXTERN CONFIG_SYSTEM_API void DestroySystem(IModuleInterface* plugin);

NS_JYE_END