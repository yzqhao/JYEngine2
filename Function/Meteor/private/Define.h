
#pragma once

#include "../../Core/Core.h"

NS_JYE_BEGIN

#ifdef _PLATFORM_WINDOWS
#ifdef METEOR_EXPORTS
    #define METEOR_API     __declspec(dllexport)
#else
    #define METEOR_API     __declspec(dllimport)
#endif
#else
    #define METEOR_API
#endif

NS_JYE_END
