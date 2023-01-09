
#pragma once

#include "../../System/System.h"

NS_JYE_BEGIN

#ifdef _PLATFORM_WINDOWS
#ifdef FILE_SYSTEM_EXPORTS
    #define FILE_SYSTEM_API     __declspec(dllexport)
#else
    #define FILE_SYSTEM_API     __declspec(dllimport)
#endif
#else
    #define FILE_SYSTEM_API
#endif

NS_JYE_END
