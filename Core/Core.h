
#pragma once

#include "../Math/Math.h"
#include "../System/DataStruct.h"

NS_JYE_BEGIN

#ifdef _PLATFORM_WINDOWS
#ifdef CORE_EXPORTS
    #define CORE_API     __declspec(dllexport)
#else
    #define CORE_API     __declspec(dllimport)
#endif
#else
    #define CORE_API
#endif

NS_JYE_END
