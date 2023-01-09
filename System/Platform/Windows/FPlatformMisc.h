#pragma once

#include "../../System.h"

#include <windows.h>

NS_JYE_BEGIN

struct SYSTEM_API FPlatformMisc
{
    static void PlatformInit();
    FORCEINLINE static void MemoryBarrier() { _mm_sfence(); }
};

NS_JYE_END
