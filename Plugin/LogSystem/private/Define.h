
#pragma once

#include "../../System/System.h"

NS_JYE_BEGIN

#ifdef _PLATFORM_WINDOWS
#ifdef LOG_SYSTEM_EXPORTS
    #define LOG_SYSTEM_API     __declspec(dllexport)
#else
    #define LOG_SYSTEM_API     __declspec(dllimport)
#endif
#else
    #define LOG_SYSTEM_API
#endif

NS_JYE_END
