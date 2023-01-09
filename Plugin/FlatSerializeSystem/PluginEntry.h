
#pragma once

#include "private/Define.h"
#include "Core/Interface/IModuleInterface.h"

NS_JYE_BEGIN

class FlatSerializeSystemModulde : public IModuleInterface
{
public:
	FlatSerializeSystemModulde() {}
	virtual ~FlatSerializeSystemModulde() {}

	virtual void StartupModule();
	virtual void ShutdownModule();
};

JY_EXTERN FLAT_SERIALIZE_SYSTEM_API IModuleInterface* CreateSystem();
JY_EXTERN FLAT_SERIALIZE_SYSTEM_API void DestroySystem(IModuleInterface* plugin);

NS_JYE_END