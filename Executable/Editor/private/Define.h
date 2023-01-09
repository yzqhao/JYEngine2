
#pragma once

#include "../../Core/Core.h"

NS_JYE_BEGIN

#ifdef _PLATFORM_WINDOWS
#ifdef RESOURCE_SYSTEM_EXPORTS
    #define APPLICATION_EXPORT_API     __declspec(dllexport)
#else
    #define APPLICATION_EXPORT_API     __declspec(dllimport)
#endif
#else
    #define APPLICATION_EXPORT_API
#endif

NS_JYE_END
