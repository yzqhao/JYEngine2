#pragma once

#include "bgfx_p.h"

namespace bgfx
{
	class Platform
	{
	public:
		static FORCEINLINE bool IsPCPlatform()
		{
			return BX_ENABLED(BX_PLATFORM_WINDOWS || BX_PLATFORM_OSX || BX_PLATFORM_LINUX);
		}

		static FORCEINLINE bool IsMobilePlatform()
		{
			return BX_ENABLED(BX_PLATFORM_ANDROID || BX_PLATFORM_IOS);
		}
	};
}