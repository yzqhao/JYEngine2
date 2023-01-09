#pragma once

#include "PlatformConfig.h"

#define THREADSAFEOPRATOR
#ifdef PLATFORM_WIN32
#include "Windows/WinThreadSafeOprator.h"
#else
#include "Linux/LinuxThreadSafeOprator.h"
#endif
#undef THREADSAFEOPRATOR
