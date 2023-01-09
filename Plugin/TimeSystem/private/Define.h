
#pragma once

#include "../../System/System.h"

NS_JYE_BEGIN

#ifdef _PLATFORM_WINDOWS
#ifdef TIME_SYSTEM_EXPORTS
    #define TIME_SYSTEM_API     __declspec(dllexport)
#else
    #define TIME_SYSTEM_API     __declspec(dllimport)
#endif
#else
    #define TIME_SYSTEM_API
#endif

NS_JYE_END
