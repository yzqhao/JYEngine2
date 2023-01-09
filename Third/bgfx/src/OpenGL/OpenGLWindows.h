#pragma once

namespace bgfx { namespace gl {

#include "glimports.h"
#include "OpenGLBase.h"

struct OpenGLWindows : public OpenGLBase
{
	/** Abbrv for platform support. */
	static FORCEINLINE bool Support_GLES3_0()
	{
		return false;
	}

	static FORCEINLINE bool Support_GLES3_1()
	{
		return false;
	}

	static FORCEINLINE bool Support_GLES3_2()
	{
		return false;
	}

	static FORCEINLINE bool IsLowEndMobile()
	{
		return false;
	}

	static FORCEINLINE void SetCurrentFeatureLevel(DeviceFeatureLevel::Enum level) {	}

	static FORCEINLINE DeviceFeatureLevel::Enum GetCurrentFeatureLevel()
	{
		return DeviceFeatureLevel::SM5;
	}

	static FORCEINLINE bool SupportsPixelBuffer() { return true; }

};

typedef OpenGLWindows OpenGL;


} /* namespace gl */ } // namespace bgfx

