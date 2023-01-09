
#pragma once

#include "private/Define.h"
#include "Core/Interface/IModuleInterface.h"

NS_JYE_BEGIN

class LogSystemModulde : public IModuleInterface
{
public:
	LogSystemModulde() {}
	virtual ~LogSystemModulde() {}

	virtual void StartupModule();
	virtual void ShutdownModule();
};

JY_EXTERN LOG_SYSTEM_API IModuleInterface* CreateSystem();
JY_EXTERN LOG_SYSTEM_API void DestroySystem(IModuleInterface* plugin);

NS_JYE_END