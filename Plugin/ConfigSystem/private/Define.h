
#pragma once

#include "../../System/System.h"

NS_JYE_BEGIN

#ifdef _PLATFORM_WINDOWS
#ifdef CONFIG_SYSTEM_EXPORTS
    #define CONFIG_SYSTEM_API     __declspec(dllexport)
#else
    #define CONFIG_SYSTEM_API     __declspec(dllimport)
#endif
#else
    #define CONFIG_SYSTEM_API
#endif

NS_JYE_END
