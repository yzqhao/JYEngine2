
#pragma once

#include "private/Define.h"
#include "Core/Interface/IModuleInterface.h"

NS_JYE_BEGIN

class MeteorModulde : public IModuleInterface
{
public:
	MeteorModulde() {}
	virtual ~MeteorModulde() {}

	virtual void StartupModule();
	virtual void ShutdownModule();
};

JY_EXTERN METEOR_API IModuleInterface* CreateSystem();
JY_EXTERN METEOR_API void DestroySystem(IModuleInterface* plugin);

NS_JYE_END