
#pragma once

#include "../../Core/Core.h"

NS_JYE_BEGIN

#ifdef _PLATFORM_WINDOWS
#ifdef LUA_SCRIPT_EXPORTS
    #define LUA_SCRIPT_SYSTEM_API     __declspec(dllexport)
#else
    #define LUA_SCRIPT_SYSTEM_API     __declspec(dllimport)
#endif
#else
    #define LUA_SCRIPT_SYSTEM_API
#endif

NS_JYE_END
