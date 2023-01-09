#include "../bgfx_p.h"

#include "OpenGLEnv.h"

using namespace bx;
using namespace bgfx;
using namespace bgfx::gl;

bool OpenGLBase::s_FrameBufferFetchNeedDef				= false;
bool OpenGLBase::bSupportsShaderMRTFramebufferFetch		= false;
bool OpenGLBase::s_FrameBufferFetchDepthStencil			= false;
bool OpenGLBase::s_ShaderTextureLod						= false;
bool OpenGLBase::s_FrameBufferFetch						= false;
bool OpenGLBase::s_bSupportsColorBufferHalfFloat		= false;
bool OpenGLBase::s_bSupportsColorBufferFloat			= false;
OpenGLBase::OESTextureType OpenGLBase::s_OesType		= OESTextureType::None;

// In desktop OpenGL 4+ and OpenGL ES 3.0+, specific GL formats GL_x_INTEGER are used for integer textures.
// For older desktop OpenGL contexts, GL names without _INTEGER suffix were used.
// See http://docs.gl/gl4/glTexImage2D, http://docs.gl/gl3/glTexImage2D, http://docs.gl/es3/glTexImage2D
#if BGFX_CONFIG_RENDERER_OPENGL >= 40 || BGFX_CONFIG_RENDERER_OPENGLES
#	define RED_INTEGER  GL_RED_INTEGER
#	define RG_INTEGER   GL_RG_INTEGER
#	define RGB_INTEGER  GL_RGB_INTEGER
#	define RGBA_INTEGER GL_RGBA_INTEGER
#else
#	define RED_INTEGER  GL_RED
#	define RG_INTEGER   GL_RG
#	define RGB_INTEGER  GL_RGB
#	define RGBA_INTEGER GL_RGBA
#endif

TextureFormatInfo OpenGLBase::s_textureFormat[] =
{
	{ GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,            GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT,       GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,            GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,            GL_ZERO,                         false }, // BC1
	{ GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,            GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT,       GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,            GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,            GL_ZERO,                         false }, // BC2
	{ GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,            GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT,       GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,            GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,            GL_ZERO,                         false }, // BC3
	{ GL_COMPRESSED_LUMINANCE_LATC1_EXT,           GL_ZERO,                                      GL_COMPRESSED_LUMINANCE_LATC1_EXT,           GL_COMPRESSED_LUMINANCE_LATC1_EXT,           GL_ZERO,                         false }, // BC4
	{ GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT,     GL_ZERO,                                      GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT,     GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT,     GL_ZERO,                         false }, // BC5
	{ GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB,     GL_ZERO,                                      GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB,     GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB,     GL_ZERO,                         false }, // BC6H
	{ GL_COMPRESSED_RGBA_BPTC_UNORM_ARB,           GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB,      GL_COMPRESSED_RGBA_BPTC_UNORM_ARB,           GL_COMPRESSED_RGBA_BPTC_UNORM_ARB,           GL_ZERO,                         false }, // BC7
	{ GL_ETC1_RGB8_OES,                            GL_ZERO,                                      GL_ETC1_RGB8_OES,                            GL_ETC1_RGB8_OES,                            GL_ZERO,                         false }, // ETC1
	{ GL_COMPRESSED_RGB8_ETC2,                     GL_ZERO,                                      GL_COMPRESSED_RGB8_ETC2,                     GL_COMPRESSED_RGB8_ETC2,                     GL_ZERO,                         false }, // ETC2
	{ GL_COMPRESSED_RGBA8_ETC2_EAC,                GL_COMPRESSED_SRGB8_ETC2,                     GL_COMPRESSED_RGBA8_ETC2_EAC,                GL_COMPRESSED_RGBA8_ETC2_EAC,                GL_ZERO,                         false }, // ETC2A
	{ GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_ZERO,                         false }, // ETC2A1
	{ GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG,          GL_COMPRESSED_SRGB_PVRTC_2BPPV1_EXT,          GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG,          GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG,          GL_ZERO,                         false }, // PTC12
	{ GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG,          GL_COMPRESSED_SRGB_PVRTC_4BPPV1_EXT,          GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG,          GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG,          GL_ZERO,                         false }, // PTC14
	{ GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG,         GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT,    GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG,         GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG,         GL_ZERO,                         false }, // PTC12A
	{ GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG,         GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT,    GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG,         GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG,         GL_ZERO,                         false }, // PTC14A
	{ GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG,         GL_ZERO,                                      GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG,         GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG,         GL_ZERO,                         false }, // PTC22
	{ GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG,         GL_ZERO,                                      GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG,         GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG,         GL_ZERO,                         false }, // PTC24
	{ GL_ATC_RGB_AMD,                              GL_ZERO,                                      GL_ATC_RGB_AMD,                              GL_ATC_RGB_AMD,                              GL_ZERO,                         false }, // ATC
	{ GL_ATC_RGBA_EXPLICIT_ALPHA_AMD,              GL_ZERO,                                      GL_ATC_RGBA_EXPLICIT_ALPHA_AMD,              GL_ATC_RGBA_EXPLICIT_ALPHA_AMD,              GL_ZERO,                         false }, // ATCE
	{ GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD,          GL_ZERO,                                      GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD,          GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD,          GL_ZERO,                         false }, // ATCI
	{ GL_COMPRESSED_RGBA_ASTC_4x4_KHR,             GL_COMPRESSED_SRGB8_ASTC_4x4_KHR,             GL_COMPRESSED_RGBA_ASTC_4x4_KHR,             GL_COMPRESSED_RGBA_ASTC_4x4_KHR,             GL_ZERO,                         false }, // ASTC4x4
	{ GL_COMPRESSED_RGBA_ASTC_5x5_KHR,             GL_COMPRESSED_SRGB8_ASTC_5x5_KHR,             GL_COMPRESSED_RGBA_ASTC_5x5_KHR,             GL_COMPRESSED_RGBA_ASTC_5x5_KHR,             GL_ZERO,                         false }, // ASTC5x5
	{ GL_COMPRESSED_RGBA_ASTC_6x6_KHR,             GL_COMPRESSED_SRGB8_ASTC_6x6_KHR,             GL_COMPRESSED_RGBA_ASTC_6x6_KHR,             GL_COMPRESSED_RGBA_ASTC_6x6_KHR,             GL_ZERO,                         false }, // ASTC6x6
	{ GL_COMPRESSED_RGBA_ASTC_8x5_KHR,             GL_COMPRESSED_SRGB8_ASTC_8x5_KHR,             GL_COMPRESSED_RGBA_ASTC_8x5_KHR,             GL_COMPRESSED_RGBA_ASTC_8x5_KHR,             GL_ZERO,                         false }, // ASTC8x5
	{ GL_COMPRESSED_RGBA_ASTC_8x6_KHR,             GL_COMPRESSED_SRGB8_ASTC_8x6_KHR,             GL_COMPRESSED_RGBA_ASTC_8x6_KHR,             GL_COMPRESSED_RGBA_ASTC_8x6_KHR,             GL_ZERO,                         false }, // ASTC8x6
	{ GL_COMPRESSED_RGBA_ASTC_10x5_KHR,            GL_COMPRESSED_SRGB8_ASTC_10x5_KHR,            GL_COMPRESSED_RGBA_ASTC_10x5_KHR,            GL_COMPRESSED_RGBA_ASTC_10x5_KHR,            GL_ZERO,                         false }, // ASTC10x5
	{ GL_ZERO,                                     GL_ZERO,                                      GL_ZERO,                                     GL_ZERO,                                     GL_ZERO,                         false }, // Unknown
	{ GL_ZERO,                                     GL_ZERO,                                      GL_ZERO,                                     GL_ZERO,                                     GL_ZERO,                         false }, // R1
	{ GL_ALPHA,                                    GL_ZERO,                                      GL_ALPHA,                                    GL_ALPHA,                                    GL_UNSIGNED_BYTE,                false }, // A8
	{ GL_R8,                                       GL_ZERO,                                      GL_RED,                                      GL_RED,                                      GL_UNSIGNED_BYTE,                false }, // R8
	{ GL_R8I,                                      GL_ZERO,                                      RED_INTEGER,                                 GL_RED_INTEGER,                              GL_BYTE,                         false }, // R8I
	{ GL_R8UI,                                     GL_ZERO,                                      RED_INTEGER,                                 GL_RED_INTEGER,                              GL_UNSIGNED_BYTE,                false }, // R8U
	{ GL_R8_SNORM,                                 GL_ZERO,                                      GL_RED,                                      GL_RED,                                      GL_BYTE,                         false }, // R8S
	{ GL_R16,                                      GL_ZERO,                                      GL_RED,                                      GL_RED,                                      GL_UNSIGNED_SHORT,               false }, // R16
	{ GL_R16I,                                     GL_ZERO,                                      RED_INTEGER,                                 GL_RED_INTEGER,                              GL_SHORT,                        false }, // R16I
	{ GL_R16UI,                                    GL_ZERO,                                      RED_INTEGER,                                 GL_RED_INTEGER,                              GL_UNSIGNED_SHORT,               false }, // R16U
	{ GL_R16F,                                     GL_ZERO,                                      GL_RED,                                      GL_RED,                                      GL_HALF_FLOAT,                   false }, // R16F
	{ GL_R16_SNORM,                                GL_ZERO,                                      GL_RED,                                      GL_RED,                                      GL_SHORT,                        false }, // R16S
	{ GL_R32I,                                     GL_ZERO,                                      RED_INTEGER,                                 GL_RED_INTEGER,                              GL_INT,                          false }, // R32I
	{ GL_R32UI,                                    GL_ZERO,                                      RED_INTEGER,                                 GL_RED_INTEGER,                              GL_UNSIGNED_INT,                 false }, // R32U
	{ GL_R32F,                                     GL_ZERO,                                      GL_RED,                                      GL_RED,                                      GL_FLOAT,                        false }, // R32F
	{ GL_RG8,                                      GL_ZERO,                                      GL_RG,                                       GL_RG,                                       GL_UNSIGNED_BYTE,                false }, // RG8
	{ GL_RG8I,                                     GL_ZERO,                                      RG_INTEGER,                                  GL_RG_INTEGER,                               GL_BYTE,                         false }, // RG8I
	{ GL_RG8UI,                                    GL_ZERO,                                      RG_INTEGER,                                  GL_RG_INTEGER,                               GL_UNSIGNED_BYTE,                false }, // RG8U
	{ GL_RG8_SNORM,                                GL_ZERO,                                      GL_RG,                                       GL_RG,                                       GL_BYTE,                         false }, // RG8S
	{ GL_RG16,                                     GL_ZERO,                                      GL_RG,                                       GL_RG,                                       GL_UNSIGNED_SHORT,               false }, // RG16
	{ GL_RG16I,                                    GL_ZERO,                                      RG_INTEGER,                                  GL_RG_INTEGER,                               GL_SHORT,                        false }, // RG16I
	{ GL_RG16UI,                                   GL_ZERO,                                      RG_INTEGER,                                  GL_RG_INTEGER,                               GL_UNSIGNED_SHORT,               false }, // RG16U
	{ GL_RG16F,                                    GL_ZERO,                                      GL_RG,                                       GL_RG,                                       GL_HALF_FLOAT,                   false }, // RG16F
	{ GL_RG16_SNORM,                               GL_ZERO,                                      GL_RG,                                       GL_RG,                                       GL_SHORT,                        false }, // RG16S
	{ GL_RG32I,                                    GL_ZERO,                                      RG_INTEGER,                                  GL_RG_INTEGER,                               GL_INT,                          false }, // RG32I
	{ GL_RG32UI,                                   GL_ZERO,                                      RG_INTEGER,                                  GL_RG_INTEGER,                               GL_UNSIGNED_INT,                 false }, // RG32U
	{ GL_RG32F,                                    GL_ZERO,                                      GL_RG,                                       GL_RG,                                       GL_FLOAT,                        false }, // RG32F
	{ GL_RGB8,                                     GL_SRGB8,                                     GL_RGB,                                      GL_RGB,                                      GL_UNSIGNED_BYTE,                false }, // RGB8
	{ GL_RGB8I,                                    GL_ZERO,                                      RGB_INTEGER,                                 GL_RGB_INTEGER,                              GL_BYTE,                         false }, // RGB8I
	{ GL_RGB8UI,                                   GL_ZERO,                                      RGB_INTEGER,                                 GL_RGB_INTEGER,                              GL_UNSIGNED_BYTE,                false }, // RGB8U
	{ GL_RGB8_SNORM,                               GL_ZERO,                                      GL_RGB,                                      GL_RGB,                                      GL_BYTE,                         false }, // RGB8S
	{ GL_RGB9_E5,                                  GL_ZERO,                                      GL_RGB,                                      GL_RGB,                                      GL_UNSIGNED_INT_5_9_9_9_REV,     false }, // RGB9E5F
	{ GL_RGBA8,                                    GL_SRGB8_ALPHA8,                              GL_BGRA,                                     GL_BGRA,                                     GL_UNSIGNED_BYTE,                false }, // BGRA8
	{ GL_RGBA8,                                    GL_SRGB8_ALPHA8,                              GL_RGBA,                                     GL_RGBA,                                     GL_UNSIGNED_BYTE,                false }, // RGBA8
	{ GL_RGBA8I,                                   GL_ZERO,                                      RGBA_INTEGER,                                GL_RGBA_INTEGER,                             GL_BYTE,                         false }, // RGBA8I
	{ GL_RGBA8UI,                                  GL_ZERO,                                      RGBA_INTEGER,                                GL_RGBA_INTEGER,                             GL_UNSIGNED_BYTE,                false }, // RGBA8U
	{ GL_RGBA8_SNORM,                              GL_ZERO,                                      GL_RGBA,                                     GL_RGBA,                                     GL_BYTE,                         false }, // RGBA8S
	{ GL_RGBA16,                                   GL_ZERO,                                      GL_RGBA,                                     GL_RGBA,                                     GL_UNSIGNED_SHORT,               false }, // RGBA16
	{ GL_RGBA16I,                                  GL_ZERO,                                      RGBA_INTEGER,                                GL_RGBA_INTEGER,                             GL_SHORT,                        false }, // RGBA16I
	{ GL_RGBA16UI,                                 GL_ZERO,                                      RGBA_INTEGER,                                GL_RGBA_INTEGER,                             GL_UNSIGNED_SHORT,               false }, // RGBA16U
	{ GL_RGBA16F,                                  GL_ZERO,                                      GL_RGBA,                                     GL_RGBA,                                     GL_HALF_FLOAT,                   false }, // RGBA16F
	{ GL_RGBA16_SNORM,                             GL_ZERO,                                      GL_RGBA,                                     GL_RGBA,                                     GL_SHORT,                        false }, // RGBA16S
	{ GL_RGBA32I,                                  GL_ZERO,                                      RGBA_INTEGER,                                GL_RGBA_INTEGER,                             GL_INT,                          false }, // RGBA32I
	{ GL_RGBA32UI,                                 GL_ZERO,                                      RGBA_INTEGER,                                GL_RGBA_INTEGER,                             GL_UNSIGNED_INT,                 false }, // RGBA32U
	{ GL_RGBA32F,                                  GL_ZERO,                                      GL_RGBA,                                     GL_RGBA,                                     GL_FLOAT,                        false }, // RGBA32F
	{ GL_RGB565,                                   GL_ZERO,                                      GL_RGB,                                      GL_RGB,                                      GL_UNSIGNED_SHORT_5_6_5,         false }, // R5G6B5
	{ GL_RGBA4,                                    GL_ZERO,                                      GL_BGRA,                                     GL_BGRA,                                     GL_UNSIGNED_SHORT_4_4_4_4_REV,   false }, // RGBA4
	{ GL_RGB5_A1,                                  GL_ZERO,                                      GL_BGRA,                                     GL_BGRA,                                     GL_UNSIGNED_SHORT_1_5_5_5_REV,   false }, // RGB5A1
	{ GL_RGB10_A2,                                 GL_ZERO,                                      GL_RGBA,                                     GL_RGBA,                                     GL_UNSIGNED_INT_2_10_10_10_REV,  false }, // RGB10A2
	{ GL_R11F_G11F_B10F,                           GL_ZERO,                                      GL_RGB,                                      GL_RGB,                                      GL_UNSIGNED_INT_10F_11F_11F_REV, false }, // RG11B10F
	{ GL_ZERO,                                     GL_ZERO,                                      GL_ZERO,                                     GL_ZERO,                                     GL_ZERO,                         false }, // UnknownDepth
	{ GL_DEPTH_COMPONENT16,                        GL_ZERO,                                      GL_DEPTH_COMPONENT,                          GL_DEPTH_COMPONENT,                          GL_UNSIGNED_SHORT,               false }, // D16
	{ GL_DEPTH_COMPONENT24,                        GL_ZERO,                                      GL_DEPTH_COMPONENT,                          GL_DEPTH_COMPONENT,                          GL_UNSIGNED_INT,                 false }, // D24
	{ GL_DEPTH24_STENCIL8,                         GL_ZERO,                                      GL_DEPTH_STENCIL,                            GL_DEPTH_STENCIL,                            GL_UNSIGNED_INT_24_8,            false }, // D24S8
	{ GL_DEPTH_COMPONENT32,                        GL_ZERO,                                      GL_DEPTH_COMPONENT,                          GL_DEPTH_COMPONENT,                          GL_UNSIGNED_INT,                 false }, // D32
	{ GL_DEPTH_COMPONENT32F,                       GL_ZERO,                                      GL_DEPTH_COMPONENT,                          GL_DEPTH_COMPONENT,                          GL_FLOAT,                        false }, // D16F
	{ GL_DEPTH_COMPONENT32F,                       GL_ZERO,                                      GL_DEPTH_COMPONENT,                          GL_DEPTH_COMPONENT,                          GL_FLOAT,                        false }, // D24F
	{ GL_DEPTH_COMPONENT32F,                       GL_ZERO,                                      GL_DEPTH_COMPONENT,                          GL_DEPTH_COMPONENT,                          GL_FLOAT,                        false }, // D32F
	{ GL_STENCIL_INDEX8,                           GL_ZERO,                                      GL_STENCIL_INDEX,                            GL_STENCIL_INDEX,                            GL_UNSIGNED_BYTE,                false }, // D0S8
};
BX_STATIC_ASSERT(TextureFormat::Count == BX_COUNTOF(OpenGLBase::s_textureFormat));

bool OpenGLBase::s_textureFilter[TextureFormat::Count + 1];

const GLenum OpenGLBase::s_cmpFunc[] =
{
	0, // ignored
	GL_LESS,
	GL_LEQUAL,
	GL_EQUAL,
	GL_GEQUAL,
	GL_GREATER,
	GL_NOTEQUAL,
	GL_NEVER,
	GL_ALWAYS,
};

GLenum OpenGLBase::s_textureAddress[] =
{
	GL_REPEAT,
	GL_MIRRORED_REPEAT,
	GL_CLAMP_TO_EDGE,
	GL_CLAMP_TO_BORDER,
};

const Blend OpenGLBase::s_blendFactor[] =
{
	{ 0,                           0,                           false }, // ignored
	{ GL_ZERO,                     GL_ZERO,                     false }, // ZERO
	{ GL_ONE,                      GL_ONE,                      false }, // ONE
	{ GL_SRC_COLOR,                GL_SRC_COLOR,                false }, // SRC_COLOR
	{ GL_ONE_MINUS_SRC_COLOR,      GL_ONE_MINUS_SRC_COLOR,      false }, // INV_SRC_COLOR
	{ GL_SRC_ALPHA,                GL_SRC_ALPHA,                false }, // SRC_ALPHA
	{ GL_ONE_MINUS_SRC_ALPHA,      GL_ONE_MINUS_SRC_ALPHA,      false }, // INV_SRC_ALPHA
	{ GL_DST_ALPHA,                GL_DST_ALPHA,                false }, // DST_ALPHA
	{ GL_ONE_MINUS_DST_ALPHA,      GL_ONE_MINUS_DST_ALPHA,      false }, // INV_DST_ALPHA
	{ GL_DST_COLOR,                GL_DST_COLOR,                false }, // DST_COLOR
	{ GL_ONE_MINUS_DST_COLOR,      GL_ONE_MINUS_DST_COLOR,      false }, // INV_DST_COLOR
	{ GL_SRC_ALPHA_SATURATE,       GL_ONE,                      false }, // SRC_ALPHA_SAT
	{ GL_CONSTANT_COLOR,           GL_CONSTANT_COLOR,           true  }, // FACTOR
	{ GL_ONE_MINUS_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR, true  }, // INV_FACTOR
};

const GLenum OpenGLBase::s_blendEquation[] =
{
	GL_FUNC_ADD,
	GL_FUNC_SUBTRACT,
	GL_FUNC_REVERSE_SUBTRACT,
	GL_MIN,
	GL_MAX,
};

const GLenum OpenGLBase::s_stencilOp[] =
{
	GL_ZERO,
	GL_KEEP,
	GL_REPLACE,
	GL_INCR_WRAP,
	GL_INCR,
	GL_DECR_WRAP,
	GL_DECR,
	GL_INVERT,
};

const GLenum OpenGLBase::s_stencilFace[] =
{
	GL_FRONT_AND_BACK,
	GL_FRONT,
	GL_BACK,
};

GLenum OpenGLBase::s_rboFormat[] =
{
	GL_ZERO,               // BC1
	GL_ZERO,               // BC2
	GL_ZERO,               // BC3
	GL_ZERO,               // BC4
	GL_ZERO,               // BC5
	GL_ZERO,               // BC6H
	GL_ZERO,               // BC7
	GL_ZERO,               // ETC1
	GL_ZERO,               // ETC2
	GL_ZERO,               // ETC2A
	GL_ZERO,               // ETC2A1
	GL_ZERO,               // PTC12
	GL_ZERO,               // PTC14
	GL_ZERO,               // PTC12A
	GL_ZERO,               // PTC14A
	GL_ZERO,               // PTC22
	GL_ZERO,               // PTC24
	GL_ZERO,               // ATC
	GL_ZERO,               // ATCE
	GL_ZERO,               // ATCI
	GL_ZERO,               // ASTC4x4
	GL_ZERO,               // ASTC5x5
	GL_ZERO,               // ASTC6x6
	GL_ZERO,               // ASTC8x5
	GL_ZERO,               // ASTC8x6
	GL_ZERO,               // ASTC10x5
	GL_ZERO,               // Unknown
	GL_ZERO,               // R1
	GL_ALPHA,              // A8
	GL_R8,                 // R8
	GL_R8I,                // R8I
	GL_R8UI,               // R8U
	GL_R8_SNORM,           // R8S
	GL_R16,                // R16
	GL_R16I,               // R16I
	GL_R16UI,              // R16U
	GL_R16F,               // R16F
	GL_R16_SNORM,          // R16S
	GL_R32I,               // R32I
	GL_R32UI,              // R32U
	GL_R32F,               // R32F
	GL_RG8,                // RG8
	GL_RG8I,               // RG8I
	GL_RG8UI,              // RG8U
	GL_RG8_SNORM,          // RG8S
	GL_RG16,               // RG16
	GL_RG16I,              // RG16I
	GL_RG16UI,             // RG16U
	GL_RG16F,              // RG16F
	GL_RG16_SNORM,         // RG16S
	GL_RG32I,              // RG32I
	GL_RG32UI,             // RG32U
	GL_RG32F,              // RG32F
	GL_RGB8,               // RGB8
	GL_RGB8I,              // RGB8I
	GL_RGB8UI,             // RGB8UI
	GL_RGB8_SNORM,         // RGB8S
	GL_RGB9_E5,            // RGB9E5F
	GL_RGBA8,              // BGRA8
	GL_RGBA8,              // RGBA8
	GL_RGBA8I,             // RGBA8I
	GL_RGBA8UI,            // RGBA8UI
	GL_RGBA8_SNORM,        // RGBA8S
	GL_RGBA16,             // RGBA16
	GL_RGBA16I,            // RGBA16I
	GL_RGBA16UI,           // RGBA16U
	GL_RGBA16F,            // RGBA16F
	GL_RGBA16_SNORM,       // RGBA16S
	GL_RGBA32I,            // RGBA32I
	GL_RGBA32UI,           // RGBA32U
	GL_RGBA32F,            // RGBA32F
	GL_RGB565,             // R5G6B5
	GL_RGBA4,              // RGBA4
	GL_RGB5_A1,            // RGB5A1
	GL_RGB10_A2,           // RGB10A2
	GL_R11F_G11F_B10F,     // RG11B10F
	GL_ZERO,               // UnknownDepth
	GL_DEPTH_COMPONENT16,  // D16
	GL_DEPTH_COMPONENT24,  // D24
	GL_DEPTH24_STENCIL8,   // D24S8
	GL_DEPTH_COMPONENT32,  // D32
	GL_DEPTH_COMPONENT32F, // D16F
	GL_DEPTH_COMPONENT32F, // D24F
	GL_DEPTH_COMPONENT32F, // D32F
	GL_STENCIL_INDEX8,     // D0S8
};
BX_STATIC_ASSERT(TextureFormat::Count == BX_COUNTOF(OpenGLBase::s_rboFormat));

GLenum OpenGLBase::s_imageFormat[] =
{
	GL_ZERO,           // BC1
	GL_ZERO,           // BC2
	GL_ZERO,           // BC3
	GL_ZERO,           // BC4
	GL_ZERO,           // BC5
	GL_ZERO,           // BC6H
	GL_ZERO,           // BC7
	GL_ZERO,           // ETC1
	GL_ZERO,           // ETC2
	GL_ZERO,           // ETC2A
	GL_ZERO,           // ETC2A1
	GL_ZERO,           // PTC12
	GL_ZERO,           // PTC14
	GL_ZERO,           // PTC12A
	GL_ZERO,           // PTC14A
	GL_ZERO,           // PTC22
	GL_ZERO,           // PTC24
	GL_ZERO,           // ATC
	GL_ZERO,           // ATCE
	GL_ZERO,           // ATCI
	GL_ZERO,           // ASTC4x4
	GL_ZERO,           // ASTC5x5
	GL_ZERO,           // ASTC6x6
	GL_ZERO,           // ASTC8x5
	GL_ZERO,           // ASTC8x6
	GL_ZERO,           // ASTC10x5
	GL_ZERO,           // Unknown
	GL_ZERO,           // R1
	GL_ALPHA,          // A8
	GL_R8,             // R8
	GL_R8I,            // R8I
	GL_R8UI,           // R8UI
	GL_R8_SNORM,       // R8S
	GL_R16,            // R16
	GL_R16I,           // R16I
	GL_R16UI,          // R16U
	GL_R16F,           // R16F
	GL_R16_SNORM,      // R16S
	GL_R32I,           // R32I
	GL_R32UI,          // R32U
	GL_R32F,           // R32F
	GL_RG8,            // RG8
	GL_RG8I,           // RG8I
	GL_RG8UI,          // RG8U
	GL_RG8_SNORM,      // RG8S
	GL_RG16,           // RG16
	GL_RG16I,          // RG16I
	GL_RG16UI,         // RG16U
	GL_RG16F,          // RG16F
	GL_RG16_SNORM,     // RG16S
	GL_RG32I,          // RG32I
	GL_RG32UI,         // RG32U
	GL_RG32F,          // RG32F
	GL_RGB8,           // RGB8
	GL_RGB8I,          // RGB8I
	GL_RGB8UI,         // RGB8UI
	GL_RGB8_SNORM,     // RGB8S
	GL_RGB9_E5,        // RGB9E5F
	GL_RGBA8,          // BGRA8
	GL_RGBA8,          // RGBA8
	GL_RGBA8I,         // RGBA8I
	GL_RGBA8UI,        // RGBA8UI
	GL_RGBA8_SNORM,    // RGBA8S
	GL_RGBA16,         // RGBA16
	GL_RGBA16I,        // RGBA16I
	GL_RGBA16UI,       // RGBA16U
	GL_RGBA16F,        // RGBA16F
	GL_RGBA16_SNORM,   // RGBA16S
	GL_RGBA32I,        // RGBA32I
	GL_RGBA32UI,       // RGBA32U
	GL_RGBA32F,        // RGBA32F
	GL_RGB565,         // R5G6B5
	GL_RGBA4,          // RGBA4
	GL_RGB5_A1,        // RGB5A1
	GL_RGB10_A2,       // RGB10A2
	GL_R11F_G11F_B10F, // RG11B10F
	GL_ZERO,           // UnknownDepth
	GL_ZERO,           // D16
	GL_ZERO,           // D24
	GL_ZERO,           // D24S8
	GL_ZERO,           // D32
	GL_ZERO,           // D16F
	GL_ZERO,           // D24F
	GL_ZERO,           // D32F
	GL_ZERO,           // D0S8
};
BX_STATIC_ASSERT(TextureFormat::Count == BX_COUNTOF(OpenGLBase::s_imageFormat));

// Extension registry
//
// ANGLE:
// https://github.com/google/angle/tree/master/extensions
//
// CHROMIUM:
// https://chromium.googlesource.com/chromium/src.git/+/refs/heads/git-svn/gpu/GLES2/extensions/CHROMIUM
//
// EGL:
// https://www.khronos.org/registry/egl/extensions/
//
// GL:
// https://www.opengl.org/registry/
//
// GLES:
// https://www.khronos.org/registry/gles/extensions/
//
// WEBGL:
// https://www.khronos.org/registry/webgl/extensions/
//
Extension OpenGLBase::s_extension[] =
{
	{ "AMD_conservative_depth",                   false,                             true  },
	{ "AMD_multi_draw_indirect",                  false,                             true  },

	{ "ANGLE_depth_texture",                      false,                             true  },
	{ "ANGLE_framebuffer_blit",                   false,                             true  },
	{ "ANGLE_framebuffer_multisample",            false,                             false },
	{ "ANGLE_instanced_arrays",                   false,                             true  },
	{ "ANGLE_texture_compression_dxt1",           false,                             true  },
	{ "ANGLE_texture_compression_dxt3",           false,                             true  },
	{ "ANGLE_texture_compression_dxt5",           false,                             true  },
	{ "ANGLE_timer_query",                        false,                             true  },
	{ "ANGLE_translated_shader_source",           false,                             true  },

	{ "APPLE_texture_format_BGRA8888",            false,                             true  },
	{ "APPLE_texture_max_level",                  false,                             true  },

	{ "ARB_clip_control",                         BGFX_CONFIG_RENDERER_OPENGL >= 43, true  },
	{ "ARB_compute_shader",                       BGFX_CONFIG_RENDERER_OPENGL >= 43, true  },
	{ "ARB_conservative_depth",                   BGFX_CONFIG_RENDERER_OPENGL >= 42, true  },
	{ "ARB_copy_image",                           BGFX_CONFIG_RENDERER_OPENGL >= 42, true  },
	{ "ARB_debug_label",                          false,                             true  },
	{ "ARB_debug_output",                         BGFX_CONFIG_RENDERER_OPENGL >= 43, true  },
	{ "ARB_depth_buffer_float",                   BGFX_CONFIG_RENDERER_OPENGL >= 33, true  },
	{ "ARB_depth_clamp",                          BGFX_CONFIG_RENDERER_OPENGL >= 32, true  },
	{ "ARB_draw_buffers_blend",                   BGFX_CONFIG_RENDERER_OPENGL >= 40, true  },
	{ "ARB_draw_indirect",                        BGFX_CONFIG_RENDERER_OPENGL >= 40, true  },
	{ "ARB_draw_instanced",                       BGFX_CONFIG_RENDERER_OPENGL >= 33, true  },
	{ "ARB_ES3_compatibility",                    BGFX_CONFIG_RENDERER_OPENGL >= 43, true  },
	{ "ARB_framebuffer_object",                   BGFX_CONFIG_RENDERER_OPENGL >= 30, true  },
	{ "ARB_framebuffer_sRGB",                     BGFX_CONFIG_RENDERER_OPENGL >= 30, true  },
	{ "ARB_get_program_binary",                   BGFX_CONFIG_RENDERER_OPENGL >= 41, true  },
	{ "ARB_half_float_pixel",                     BGFX_CONFIG_RENDERER_OPENGL >= 30, true  },
	{ "ARB_half_float_vertex",                    BGFX_CONFIG_RENDERER_OPENGL >= 30, true  },
	{ "ARB_instanced_arrays",                     BGFX_CONFIG_RENDERER_OPENGL >= 33, true  },
	{ "ARB_internalformat_query",                 BGFX_CONFIG_RENDERER_OPENGL >= 42, true  },
	{ "ARB_internalformat_query2",                BGFX_CONFIG_RENDERER_OPENGL >= 43, true  },
	{ "ARB_invalidate_subdata",                   BGFX_CONFIG_RENDERER_OPENGL >= 43, true  },
	{ "ARB_map_buffer_range",                     BGFX_CONFIG_RENDERER_OPENGL >= 30, true  },
	{ "ARB_multi_draw_indirect",                  BGFX_CONFIG_RENDERER_OPENGL >= 43, true  },
	{ "ARB_multisample",                          false,                             true  },
	{ "ARB_occlusion_query",                      BGFX_CONFIG_RENDERER_OPENGL >= 33, true  },
	{ "ARB_occlusion_query2",                     BGFX_CONFIG_RENDERER_OPENGL >= 33, true  },
	{ "ARB_program_interface_query",              BGFX_CONFIG_RENDERER_OPENGL >= 43, true  },
	{ "ARB_provoking_vertex",                     BGFX_CONFIG_RENDERER_OPENGL >= 32, true  },
	{ "ARB_sampler_objects",                      BGFX_CONFIG_RENDERER_OPENGL >= 33, true  },
	{ "ARB_seamless_cube_map",                    BGFX_CONFIG_RENDERER_OPENGL >= 32, true  },
	{ "ARB_shader_bit_encoding",                  BGFX_CONFIG_RENDERER_OPENGL >= 33, true  },
	{ "ARB_shader_image_load_store",              BGFX_CONFIG_RENDERER_OPENGL >= 42, true  },
	{ "ARB_shader_storage_buffer_object",         BGFX_CONFIG_RENDERER_OPENGL >= 43, true  },
	{ "ARB_shader_texture_lod",                   BGFX_CONFIG_RENDERER_OPENGL >= 30, true  },
	{ "ARB_shader_viewport_layer_array",          false,                             true  },
	{ "ARB_texture_compression_bptc",             BGFX_CONFIG_RENDERER_OPENGL >= 44, true  },
	{ "ARB_texture_compression_rgtc",             BGFX_CONFIG_RENDERER_OPENGL >= 30, true  },
	{ "ARB_texture_cube_map_array",               BGFX_CONFIG_RENDERER_OPENGL >= 40, true  },
	{ "ARB_texture_float",                        BGFX_CONFIG_RENDERER_OPENGL >= 30, true  },
	{ "ARB_texture_multisample",                  BGFX_CONFIG_RENDERER_OPENGL >= 32, true  },
	{ "ARB_texture_rg",                           BGFX_CONFIG_RENDERER_OPENGL >= 30, true  },
	{ "ARB_texture_rgb10_a2ui",                   BGFX_CONFIG_RENDERER_OPENGL >= 33, true  },
	{ "ARB_texture_stencil8",                     false,                             true  },
	{ "ARB_texture_storage",                      BGFX_CONFIG_RENDERER_OPENGL >= 42, true  },
	{ "ARB_texture_swizzle",                      BGFX_CONFIG_RENDERER_OPENGL >= 33, true  },
	{ "ARB_timer_query",                          BGFX_CONFIG_RENDERER_OPENGL >= 33, true  },
	{ "ARB_uniform_buffer_object",                BGFX_CONFIG_RENDERER_OPENGL >= 31, true  },
	{ "ARB_vertex_array_object",                  BGFX_CONFIG_RENDERER_OPENGL >= 30, true  },
	{ "ARB_vertex_type_2_10_10_10_rev",           false,                             true  },

	{ "ATI_meminfo",                              false,                             true  },

	{ "CHROMIUM_color_buffer_float_rgb",          false,                             true  },
	{ "CHROMIUM_color_buffer_float_rgba",         false,                             true  },
	{ "CHROMIUM_depth_texture",                   false,                             true  },
	{ "CHROMIUM_framebuffer_multisample",         false,                             true  },
	{ "CHROMIUM_texture_compression_dxt3",        false,                             true  },
	{ "CHROMIUM_texture_compression_dxt5",        false,                             true  },

	{ "EXT_bgra",                                 false,                             true  },
	{ "EXT_blend_color",                          BGFX_CONFIG_RENDERER_OPENGL >= 31, true  },
	{ "EXT_blend_minmax",                         BGFX_CONFIG_RENDERER_OPENGL >= 14, true  },
	{ "EXT_blend_subtract",                       BGFX_CONFIG_RENDERER_OPENGL >= 14, true  },
	{ "EXT_color_buffer_half_float",              false,                             true  }, // GLES2 extension.
	{ "EXT_color_buffer_float",                   false,                             true  }, // GLES2 extension.
	{ "EXT_copy_image",                           false,                             true  }, // GLES2 extension.
	{ "EXT_compressed_ETC1_RGB8_sub_texture",     false,                             true  }, // GLES2 extension.
	{ "EXT_debug_label",                          false,                             true  },
	{ "EXT_debug_marker",                         false,                             true  },
	{ "EXT_debug_tool",                           false,                             true  }, // RenderDoc extension.
	{ "EXT_discard_framebuffer",                  false,                             true  }, // GLES2 extension.
	{ "EXT_disjoint_timer_query",                 false,                             true  }, // GLES2 extension.
	{ "EXT_draw_buffers",                         false,                             true  }, // GLES2 extension.
	{ "EXT_draw_instanced",                       false,                             true  }, // GLES2 extension.
	{ "EXT_instanced_arrays",                     false,                             true  }, // GLES2 extension.
	{ "EXT_frag_depth",                           false,                             true  }, // GLES2 extension.
	{ "EXT_framebuffer_blit",                     BGFX_CONFIG_RENDERER_OPENGL >= 30, true  },
	{ "EXT_framebuffer_object",                   BGFX_CONFIG_RENDERER_OPENGL >= 30, true  },
	{ "EXT_framebuffer_sRGB",                     BGFX_CONFIG_RENDERER_OPENGL >= 30, true  },
	{ "EXT_gpu_shader4",                          BGFX_CONFIG_RENDERER_OPENGL >= 31, true  },
	{ "EXT_multi_draw_indirect",                  false,                             true  }, // GLES3.1 extension.
	{ "EXT_occlusion_query_boolean",              false,                             true  }, // GLES2 extension.
	{ "EXT_packed_float",                         BGFX_CONFIG_RENDERER_OPENGL >= 33, true  },
	{ "EXT_read_format_bgra",                     false,                             true  },
	{ "EXT_shader_image_load_store",              false,                             true  },
	{ "EXT_shader_texture_lod",                   false,                             true  }, // GLES2 extension.
	{ "EXT_shadow_samplers",                      false,                             true  },
	{ "EXT_sRGB_write_control",                   false,                             true  }, // GLES2 extension.
	{ "EXT_texture_array",                        BGFX_CONFIG_RENDERER_OPENGL >= 30, true  },
	{ "EXT_texture_compression_dxt1",             false,                             true  },
	{ "EXT_texture_compression_latc",             false,                             true  },
	{ "EXT_texture_compression_rgtc",             BGFX_CONFIG_RENDERER_OPENGL >= 30, true  },
	{ "EXT_texture_compression_s3tc",             false,                             true  },
	{ "EXT_texture_cube_map_array",               false,                             true  }, // GLES3.1 extension.
	{ "EXT_texture_filter_anisotropic",           false,                             true  },
	{ "EXT_texture_format_BGRA8888",              false,                             true  },
	{ "EXT_texture_rg",                           false,                             true  }, // GLES2 extension.
	{ "EXT_texture_shared_exponent",              false,                             true  },
	{ "EXT_texture_snorm",                        BGFX_CONFIG_RENDERER_OPENGL >= 30, true  },
	{ "EXT_texture_sRGB",                         false,                             true  },
	{ "EXT_texture_storage",                      false,                             true  },
	{ "EXT_texture_swizzle",                      false,                             true  },
	{ "EXT_texture_type_2_10_10_10_REV",          false,                             true  },
	{ "EXT_timer_query",                          BGFX_CONFIG_RENDERER_OPENGL >= 33, true  },
	{ "EXT_unpack_subimage",                      false,                             true  },
	{ "EXT_sRGB",                                 false,                             true  }, // GLES2 extension.
	{ "EXT_multisampled_render_to_texture",       false,                             true  }, // GLES2 extension.

	{ "GOOGLE_depth_texture",                     false,                             true  },

	{ "IMG_multisampled_render_to_texture",       false,                             true  },
	{ "IMG_read_format",                          false,                             true  },
	{ "IMG_shader_binary",                        false,                             true  },
	{ "IMG_texture_compression_pvrtc",            false,                             true  },
	{ "IMG_texture_compression_pvrtc2",           false,                             true  },
	{ "IMG_texture_format_BGRA8888",              false,                             true  },

	{ "INTEL_fragment_shader_ordering",           false,                             true  },

	{ "KHR_debug",                                BGFX_CONFIG_RENDERER_OPENGL >= 43, true  },
	{ "KHR_no_error",                             false,                             true  },

	{ "MOZ_WEBGL_compressed_texture_s3tc",        false,                             true  },
	{ "MOZ_WEBGL_depth_texture",                  false,                             true  },

	{ "NV_conservative_raster",                   false,                             true  },
	{ "NV_copy_image",                            false,                             true  },
	{ "NV_draw_buffers",                          false,                             true  }, // GLES2 extension.
	{ "NV_draw_instanced",                        false,                             true  }, // GLES2 extension.
	{ "NV_instanced_arrays",                      false,                             true  }, // GLES2 extension.
	{ "NV_occlusion_query",                       false,                             true  },
	{ "NV_texture_border_clamp",                  false,                             true  }, // GLES2 extension.
	{ "NVX_gpu_memory_info",                      false,                             true  },

	{ "OES_copy_image",                           false,                             true  },
	{ "OES_compressed_ETC1_RGB8_texture",         false,                             true  },
	{ "OES_depth24",                              false,                             true  },
	{ "OES_depth32",                              false,                             true  },
	{ "OES_depth_texture",                        false,                             true  },
	{ "OES_element_index_uint",                   false,                             true  },
	{ "OES_fragment_precision_high",              false,                             true  },
	{ "OES_fbo_render_mipmap",                    false,                             true  },
	{ "OES_get_program_binary",                   false,                             true  },
	{ "OES_required_internalformat",              false,                             true  },
	{ "OES_packed_depth_stencil",                 false,                             true  },
	{ "OES_read_format",                          false,                             true  },
	{ "OES_rgb8_rgba8",                           false,                             true  },
	{ "OES_standard_derivatives",                 false,                             true  },
	{ "OES_texture_3D",                           false,                             true  },
	{ "OES_texture_float",                        false,                             true  },
	{ "OES_texture_float_linear",                 false,                             true  },
	{ "OES_texture_npot",                         false,                             true  },
	{ "OES_texture_half_float",                   false,                             true  },
	{ "OES_texture_half_float_linear",            false,                             true  },
	{ "OES_texture_stencil8",                     false,                             true  },
	{ "OES_texture_storage_multisample_2d_array", false,                             true  },
	{ "OES_vertex_array_object",                  false,                             !BX_PLATFORM_IOS },
	{ "OES_vertex_half_float",                    false,                             true  },
	{ "OES_vertex_type_10_10_10_2",               false,                             true  },

	{ "WEBGL_color_buffer_float",                 false,                             true  },
	{ "WEBGL_compressed_texture_etc1",            false,                             true  },
	{ "WEBGL_compressed_texture_s3tc",            false,                             true  },
	{ "WEBGL_compressed_texture_pvrtc",           false,                             true  },
	{ "WEBGL_depth_texture",                      false,                             true  },
	{ "WEBGL_draw_buffers",                       false,                             true  },

	{ "WEBKIT_EXT_texture_filter_anisotropic",    false,                             true  },
	{ "WEBKIT_WEBGL_compressed_texture_s3tc",     false,                             true  },
	{ "WEBKIT_WEBGL_depth_texture",               false,                             true  },
};
BX_STATIC_ASSERT(Extension::Count == BX_COUNTOF(OpenGLBase::s_extension));

const GLenum OpenGLBase::s_textureFilterMag[] =
{
	GL_LINEAR,
	GL_NEAREST,
	GL_LINEAR,
};

const GLenum OpenGLBase::s_textureFilterMin[][3] =
{
	{ GL_LINEAR,  GL_LINEAR_MIPMAP_LINEAR,  GL_LINEAR_MIPMAP_NEAREST  },
	{ GL_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_NEAREST_MIPMAP_NEAREST },
	{ GL_LINEAR,  GL_LINEAR_MIPMAP_LINEAR,  GL_LINEAR_MIPMAP_NEAREST  },
};

void OpenGLBase::getFilters(uint32_t _flags, bool _hasMips, GLenum& _magFilter, GLenum& _minFilter)
{
	const uint32_t mag = (_flags & BGFX_SAMPLER_MAG_MASK) >> BGFX_SAMPLER_MAG_SHIFT;
	const uint32_t min = (_flags & BGFX_SAMPLER_MIN_MASK) >> BGFX_SAMPLER_MIN_SHIFT;
	const uint32_t mip = (_flags & BGFX_SAMPLER_MIP_MASK) >> BGFX_SAMPLER_MIP_SHIFT;
	_magFilter = s_textureFilterMag[mag];
	_minFilter = s_textureFilterMin[min][_hasMips ? mip + 1 : 0];
}

void OpenGLBase::texSubImage(
	GLenum _target
	, GLint _level
	, GLint _xoffset
	, GLint _yoffset
	, GLint _zoffset
	, GLsizei _width
	, GLsizei _height
	, GLsizei _depth
	, GLenum _format
	, GLenum _type
	, const GLvoid* _data
)
{
	if (NULL == _data)
	{
		return;
	}

	if (_target == GL_TEXTURE_3D
		|| _target == GL_TEXTURE_2D_ARRAY
		|| _target == GL_TEXTURE_CUBE_MAP_ARRAY)
	{
		glTexSubImage3D(
			_target
			, _level
			, _xoffset
			, _yoffset
			, _zoffset
			, _width
			, _height
			, _depth
			, _format
			, _type
			, _data
		);
	}
	else if (_target == GL_TEXTURE_2D_MULTISAMPLE_ARRAY)
	{
	}
	else
	{
		BX_UNUSED(_zoffset, _depth);
		glTexSubImage2D(
			_target
			, _level
			, _xoffset
			, _yoffset
			, _width
			, _height
			, _format
			, _type
			, _data
		);
	}
}

void OpenGLBase::texImage(
	GLenum _target
	, uint32_t _msaaQuality
	, GLint _level
	, GLint _internalFormat
	, GLsizei _width
	, GLsizei _height
	, GLsizei _depth
	, GLint _border
	, GLenum _format
	, GLenum _type
	, const GLvoid* _data
)
{
	if (_target == GL_TEXTURE_3D)
	{
		glTexImage3D(
			_target
			, _level
			, _internalFormat
			, _width
			, _height
			, _depth
			, _border
			, _format
			, _type
			, _data
		);
	}
	else if (_target == GL_TEXTURE_2D_ARRAY
		|| _target == GL_TEXTURE_CUBE_MAP_ARRAY)
	{
		texSubImage(
			_target
			, _level
			, 0
			, 0
			, _depth
			, _width
			, _height
			, 1
			, _format
			, _type
			, _data
		);
	}
	else if (_target == GL_TEXTURE_2D_MULTISAMPLE_ARRAY)
	{
	}
	else if (_target == GL_TEXTURE_2D_MULTISAMPLE)
	{
		glTexImage2DMultisample(
			_target
			, _msaaQuality
			, _internalFormat
			, _width
			, _height
			, false
		);
	}
	else
	{
		glTexImage2D(
			_target
			, _level
			, _internalFormat
			, _width
			, _height
			, _border
			, _format
			, _type
			, _data
		);
	}

	BX_UNUSED(_msaaQuality, _depth, _border, _data);
}

void OpenGLBase::compressedTexSubImage(
	GLenum _target
	, GLint _level
	, GLint _xoffset
	, GLint _yoffset
	, GLint _zoffset
	, GLsizei _width
	, GLsizei _height
	, GLsizei _depth
	, GLenum _format
	, GLsizei _imageSize
	, const GLvoid* _data
)
{
	if (_target == GL_TEXTURE_3D
		|| _target == GL_TEXTURE_2D_ARRAY)
	{
		glCompressedTexSubImage3D(
			_target
			, _level
			, _xoffset
			, _yoffset
			, _zoffset
			, _width
			, _height
			, _depth
			, _format
			, _imageSize
			, _data
		);
	}
	else
	{
		BX_UNUSED(_zoffset, _depth);
		glCompressedTexSubImage2D(
			_target
			, _level
			, _xoffset
			, _yoffset
			, _width
			, _height
			, _format
			, _imageSize
			, _data
		);
	}
}

void OpenGLBase::compressedTexImage(
	GLenum _target
	, GLint _level
	, GLenum _internalformat
	, GLsizei _width
	, GLsizei _height
	, GLsizei _depth
	, GLint _border
	, GLsizei _imageSize
	, const GLvoid* _data
)
{
	if (_target == GL_TEXTURE_3D)
	{
		glCompressedTexImage3D(
			_target
			, _level
			, _internalformat
			, _width
			, _height
			, _depth
			, _border
			, _imageSize
			, _data
		);
	}
	else if (_target == GL_TEXTURE_2D_ARRAY
		|| _target == GL_TEXTURE_CUBE_MAP_ARRAY)
	{
		compressedTexSubImage(
			_target
			, _level
			, 0
			, 0
			, _depth
			, _width
			, _height
			, 1
			, _internalformat
			, _imageSize
			, _data
		);
	}
	else if (_target == GL_TEXTURE_2D_MULTISAMPLE_ARRAY)
	{
	}
	else
	{
		BX_UNUSED(_depth);
		glCompressedTexImage2D(
			_target
			, _level
			, _internalformat
			, _width
			, _height
			, _border
			, _imageSize
			, _data
		);
	}
}


void OpenGLBase::frameBufferValidate()
{
	GLenum complete = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	BX_ASSERT(GL_FRAMEBUFFER_COMPLETE == complete
		, "glCheckFramebufferStatus failed 0x%08x: %s"
		, complete
		, glEnumName(complete)
	);
	BX_UNUSED(complete);
}

void OpenGLBase::VertexAttribDivisor(GLuint _index, GLuint _divisor)
{
	if (glVertexAttribDivisor)
	{
		GL_CHECK(glVertexAttribDivisor(_index, _divisor));
	}
}

void OpenGLBase::DrawArraysInstanced(GLenum _mode, GLint _first, GLsizei _count, GLsizei _primcount)
{
	if (glDrawArraysInstanced)
	{
		GL_CHECK(glDrawArraysInstanced(_mode, _first, _count, _primcount));
	}
	else
	{
		GL_CHECK(glDrawArrays(_mode, _first, _count));
	}
}

void OpenGLBase::DrawElementsInstanced(GLenum _mode, GLsizei _count, GLenum _type, const GLvoid* _indices, GLsizei _primcount)
{

	if (glDrawElementsInstanced)
	{
		GL_CHECK(glDrawElementsInstanced(_mode, _count, _type, _indices, _primcount));
	}
	else
	{
		GL_CHECK(glDrawElements(_mode, _count, _type, _indices));
	}
}

void OpenGLBase::InsertEventMarker(GLsizei _length, const char* _marker)
{
	if (glInsertEventMarker)
	{
		GL_CHECK(glInsertEventMarker(_length, _marker));
	}
}

void OpenGLBase::PushDebugGroup(GLenum _source, GLuint _id, GLsizei _length, const char* _message)
{
	if (glPushDebugGroup)
	{
		GL_CHECK(glPushDebugGroup(_source, _id, _length, _message));
	}
}

void OpenGLBase::PopDebugGroup()
{
	if (glPopDebugGroup)
	{
		GL_CHECK(glPopDebugGroup());
	}
}

void OpenGLBase::ObjectLabel(GLenum _identifier, GLuint _name, GLsizei _length, const char* _label)
{
	if (glObjectLabel)
	{
		GL_CHECK(glObjectLabel(_identifier, _name, _length, _label));
	}
}

void OpenGLBase::InvalidateFramebuffer(GLenum _target, GLsizei _numAttachments, const GLenum* _attachments)
{
	if (glInvalidateFramebuffer)
	{
		GL_CHECK(glInvalidateFramebuffer(_target, _numAttachments, _attachments));
	}
}

void OpenGLBase::MultiDrawArraysIndirect(GLenum _mode, const void* _indirect, GLsizei _drawcount, GLsizei _stride)
{
	if (glMultiDrawArraysIndirect)
	{
		GL_CHECK(glMultiDrawArraysIndirect(_mode, _indirect, _drawcount, _stride));
	}
	else
	{
		const uint8_t* args = (const uint8_t*)_indirect;
		for (GLsizei ii = 0; ii < _drawcount; ++ii)
		{
			GL_CHECK(glDrawArraysIndirect(_mode, (void*)args));
			args += _stride;
		}
	}
}

void OpenGLBase::MultiDrawElementsIndirect(GLenum _mode, GLenum _type, const void* _indirect, GLsizei _drawcount, GLsizei _stride)
{
	if (glMultiDrawElementsIndirect)
	{
		GL_CHECK(glMultiDrawElementsIndirect(_mode, _type, _indirect, _drawcount, _stride));
	}
	else
	{
		const uint8_t* args = (const uint8_t*)_indirect;
		for (GLsizei ii = 0; ii < _drawcount; ++ii)
		{
			GL_CHECK(glDrawElementsIndirect(_mode, _type, (void*)args));
			args += _stride;
		}
	}
}

void OpenGLBase::PolygonMode(GLenum _face, GLenum _mode)
{
	if (glPolygonMode)
	{
		GL_CHECK(glPolygonMode(_face, _mode));
	}
}
