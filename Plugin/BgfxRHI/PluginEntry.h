
#pragma once

#include "private/Define.h"
#include "Core/Interface/IModuleInterface.h"

NS_JYE_BEGIN

JY_EXTERN BGFXRHI_API IModuleInterface* CreateSystem();
JY_EXTERN BGFXRHI_API void DestroySystem(IModuleInterface* plugin);

NS_JYE_END