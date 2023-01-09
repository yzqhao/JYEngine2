
#pragma once

#include "private/Define.h"
#include "Core/Interface/IModuleInterface.h"

NS_JYE_BEGIN

class TimeSystemModulde : public IModuleInterface
{
public:
	TimeSystemModulde() {}
	virtual ~TimeSystemModulde() {}

	virtual void StartupModule();
	virtual void ShutdownModule();
};

JY_EXTERN TIME_SYSTEM_API IModuleInterface* CreateSystem();
JY_EXTERN TIME_SYSTEM_API void DestroySystem(IModuleInterface* plugin);

NS_JYE_END