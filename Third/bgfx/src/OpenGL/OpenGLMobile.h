#pragma once

namespace bgfx { namespace gl {

#include <GLES2/gl3def.h>

#include "glimports.h"
#include "OpenGLBase.h"

struct OpenGLMobile : public OpenGLBase
{
	/** Abbrv for platform support. */
	static FORCEINLINE bool Support_GLES3_0()
	{
		return GetCurrentFeatureLevel() >= DeviceFeatureLevel::ES3_0;
	}

	static FORCEINLINE bool Support_GLES3_1()
	{
		return GetCurrentFeatureLevel() >= DeviceFeatureLevel::ES3_1;
	}

	static FORCEINLINE bool Support_GLES3_2()
	{
		return GetCurrentFeatureLevel() >= DeviceFeatureLevel::ES3_2;
	}

	static FORCEINLINE bool IsLowEndMobile()
	{
		return GetCurrentFeatureLevel() == DeviceFeatureLevel::ES2;
	}

	static FORCEINLINE DeviceFeatureLevel::Enum GetCurrentFeatureLevel()
	{
		if (CurrentFeatureLevelSupport != DeviceFeatureLevel::NONE)
		{
			return CurrentFeatureLevelSupport;
		}
		else
		{
			GLint major_version;
			GLint minor_version;
			glGetIntegerv(GL_MAJOR_VERSION, &major_version);
			glGetIntegerv(GL_MINOR_VERSION, &minor_version);
			if (major_version >= 3)
			{
				switch (minor_version)
				{
				case 0:
					CurrentFeatureLevelSupport = DeviceFeatureLevel::ES3_0;
					break;
				case 1:
					CurrentFeatureLevelSupport = DeviceFeatureLevel::ES3_1;
					break;
				case 2:
					CurrentFeatureLevelSupport = DeviceFeatureLevel::ES3_2;
					break;
				default:
					break;
				}
			}
			else if (major_version == 2)
			{
				CurrentFeatureLevelSupport = DeviceFeatureLevel::ES2;
			}
			return CurrentFeatureLevelSupport;
		}
	}

	static FORCEINLINE void SetCurrentFeatureLevel(DeviceFeatureLevel::Enum level)
	{
		CurrentFeatureLevelSupport = level;
	}

	static FORCEINLINE bool SupportsPixelBuffer()
	{
		if (GetCurrentFeatureLevel() <= DeviceFeatureLevel::ES2)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

protected:
	/** feature level info. */
	static DeviceFeatureLevel::Enum CurrentFeatureLevelSupport;

};


} /* namespace gl */
} // namespace bgfx

