
#pragma once

#include "private/Define.h"
#include "Core/Interface/IModuleInterface.h"

NS_JYE_BEGIN

class ImguiFunctionModulde : public IModuleInterface
{
public:
	ImguiFunctionModulde() {}
	virtual ~ImguiFunctionModulde() {}

	virtual void StartupModule();
	virtual void ShutdownModule();
};

JY_EXTERN IMGUI_FUNCTION_API IModuleInterface* CreateSystem();
JY_EXTERN IMGUI_FUNCTION_API void DestroySystem(IModuleInterface* plugin);

NS_JYE_END