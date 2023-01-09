
#pragma once

#include "../../System/System.h"

NS_JYE_BEGIN

#ifdef _PLATFORM_WINDOWS
#ifdef FLAT_SERIALIZE_SYSTEM_EXPORTS
    #define FLAT_SERIALIZE_SYSTEM_API     __declspec(dllexport)
#else
    #define FLAT_SERIALIZE_SYSTEM_API     __declspec(dllimport)
#endif
#else
    #define FLAT_SERIALIZE_SYSTEM_API
#endif

NS_JYE_END
