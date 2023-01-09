
#pragma once

#include "private/Define.h"
#include "Core/Interface/IModuleInterface.h"

NS_JYE_BEGIN

class LuaScriptSystemModulde : public IModuleInterface
{
public:
	LuaScriptSystemModulde() {}
	virtual ~LuaScriptSystemModulde() {}

	virtual void StartupModule();
	virtual void ShutdownModule();
};

JY_EXTERN LUA_SCRIPT_SYSTEM_API IModuleInterface* CreateSystem();
JY_EXTERN LUA_SCRIPT_SYSTEM_API void DestroySystem(IModuleInterface* plugin);

NS_JYE_END