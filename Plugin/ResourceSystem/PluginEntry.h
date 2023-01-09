
#pragma once

#include "private/Define.h"
#include "Core/Interface/IModuleInterface.h"

NS_JYE_BEGIN

class ResourceSystemModulde : public IModuleInterface
{
public:
	ResourceSystemModulde() {}
	virtual ~ResourceSystemModulde() {}

	virtual void StartupModule();
	virtual void ShutdownModule();
};

JY_EXTERN RESOURCE_SYSTEM_API IModuleInterface* CreateSystem();
JY_EXTERN RESOURCE_SYSTEM_API void DestroySystem(IModuleInterface* plugin);

NS_JYE_END