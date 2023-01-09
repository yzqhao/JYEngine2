#include "../bgfx_p.h"

#if BGFX_CONFIG_RENDERER_OPENGLES
#include "OpenGLEnv.h"

#if BX_PLATFORM_ANDROID || BX_PLATFORM_IOS

using namespace bgfx;
using namespace bgfx::gl;
DeviceFeatureLevel::Enum OpenGLMobile::CurrentFeatureLevelSupport = DeviceFeatureLevel::NONE;

#endif  // Platform selection.

#endif //  BGFX_CONFIG_RENDERER_OPENGLES
