/*
 * Copyright 2011-2022 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx/blob/master/LICENSE
 */

#include "../bgfx_p.h"

#if (BGFX_CONFIG_RENDERER_OPENGLES || BGFX_CONFIG_RENDERER_OPENGL)
#	include "OpenGLDrv.h"
#	include <bx/timer.h>
#	include <bx/uint32_t.h>
#	include "../emscripten.h"

namespace bgfx { namespace gl
{
	static char s_viewName[BGFX_CONFIG_MAX_VIEWS][BGFX_CONFIG_MAX_VIEW_NAME];

	inline void setViewType(ViewId _view, const bx::StringView _str)
	{
		if (BX_ENABLED(BGFX_CONFIG_DEBUG_ANNOTATION || BGFX_CONFIG_PROFILER) )
		{
			bx::memCopy(&s_viewName[_view][3], _str.getPtr(), _str.getLength() );
		}
	}

	struct PrimInfo
	{
		GLenum m_type;
		uint32_t m_min;
		uint32_t m_div;
		uint32_t m_sub;
	};

	static const PrimInfo s_primInfo[] =
	{
		{ GL_TRIANGLES,      3, 3, 0 },
		{ GL_TRIANGLE_STRIP, 3, 1, 2 },
		{ GL_LINES,          2, 2, 0 },
		{ GL_LINE_STRIP,     2, 1, 1 },
		{ GL_POINTS,         1, 1, 0 },
		{ GL_ZERO,           0, 0, 0 },
	};
	BX_STATIC_ASSERT(Topology::Count == BX_COUNTOF(s_primInfo)-1);

	static const GLenum s_access[] =
	{
		GL_READ_ONLY,
		GL_WRITE_ONLY,
		GL_READ_WRITE,
	};
	BX_STATIC_ASSERT(Access::Count == BX_COUNTOF(s_access) );

	static const char* s_ARB_shader_texture_lod[] =
	{
		"texture2DLod",
		"texture2DArrayLod", // BK - interacts with ARB_texture_array.
		"texture2DProjLod",
		"texture2DGrad",
		"texture2DProjGrad",
		"texture3DLod",
		"texture3DProjLod",
		"texture3DGrad",
		"texture3DProjGrad",
		"textureCubeLod",
		"textureCubeGrad",
		"shadow2DLod",
		"shadow2DProjLod",
		NULL
		// "texture1DLod",
		// "texture1DProjLod",
		// "shadow1DLod",
		// "shadow1DProjLod",
	};

	static const char* s_EXT_shader_texture_lod[] =
	{
		"texture2DLod",
		"texture2DProjLod",
		"textureCubeLod",
		"texture2DGrad",
		"texture2DProjGrad",
		"textureCubeGrad",
		NULL
	};

	static const char* s_ARB_shader_viewport_layer_array[] =
	{
		"gl_ViewportIndex",
		"gl_Layer",
		NULL
	};

	static const char* s_EXT_shadow_samplers[] =
	{
		"shadow2D",
		"shadow2DProj",
		NULL
	};

	static const char* s_OES_standard_derivatives[] =
	{
		"dFdx",
		"dFdy",
		"fwidth",
		NULL
	};

	static const char* s_uisamplers[] =
	{
		"isampler2D",
		"usampler2D",
		"isampler3D",
		"usampler3D",
		"isamplerCube",
		"usamplerCube",
		NULL
	};

	static const char* s_uint[] =
	{
		"uint",
		"uvec2",
		"uvec3",
		"uvec4",
		NULL
	};

	static const char* s_texelFetch[] =
	{
		"texture",
		"textureLod",
		"textureGrad",
		"textureProj",
		"textureProjLod",
		"texelFetch",
		"texelFetchOffset",
		NULL
	};

	static const char* s_texture3D[] =
	{
		"sampler3D",
		"sampler3DArray",
		NULL
	};

	static const char* s_textureArray[] =
	{
		"sampler2DArray",
		"sampler2DMSArray",
		"samplerCubeArray",
		"sampler2DArrayShadow",
		NULL
	};

	static const char* s_ARB_texture_multisample[] =
	{
		"sampler2DMS",
		"isampler2DMS",
		"usampler2DMS",
		NULL
	};

	static const char* s_EXT_gpu_shader4[] =
	{
		"gl_VertexID",
		"gl_InstanceID",
		"uint",
		NULL
	};

	static const char* s_ARB_gpu_shader5[] =
	{
		"bitfieldReverse",
		"floatBitsToInt",
		"floatBitsToUint",
		"intBitsToFloat",
		"uintBitsToFloat",
		NULL
	};

	static const char* s_ARB_shading_language_packing[] =
	{
		"packHalf2x16",
		"unpackHalf2x16",
		NULL
	};

	static const char* s_intepolationQualifier[] =
	{
		"flat",
		"smooth",
		"noperspective",
		"centroid",
		NULL
	};

	static void GL_APIENTRY stubFramebufferTexture(GLenum _target, GLenum _attachment, GLuint _texture, GLint _level)
	{
		GL_CHECK(glFramebufferTextureLayer(_target
			, _attachment
			, _texture
			, _level
			, 0
			) );
	}

	typedef void (*PostSwapBuffersFn)(uint32_t _width, uint32_t _height);

	void flushGlError()
	{
		for (GLenum err = glGetError(); err != 0; err = glGetError() );
	}

	GLenum getGlError()
	{
		GLenum err = glGetError();
		flushGlError();
		return err;
	}

	void setTextureFormat(TextureFormat::Enum _format, GLenum _internalFmt, GLenum _fmt, GLenum _type = GL_ZERO)
	{
		TextureFormatInfo& tfi = OpenGL::s_textureFormat[_format];
		tfi.m_internalFmt = _internalFmt;
		tfi.m_fmt         = _fmt;
		tfi.m_fmtSrgb     = _fmt;
		tfi.m_type        = _type;
	}

	void setTextureFormatSrgb(TextureFormat::Enum _format, GLenum _internalFmtSrgb, GLenum _fmtSrgb)
	{
		TextureFormatInfo& tfi = OpenGL::s_textureFormat[_format];
		tfi.m_internalFmtSrgb = _internalFmtSrgb;
		tfi.m_fmtSrgb = _fmtSrgb;
	}

	GLenum initTestTexture(TextureFormat::Enum _format, bool _srgb, bool _mipmaps, bool _array, GLsizei _dim)
	{
		const TextureFormatInfo& tfi = OpenGL::s_textureFormat[_format];
		GLenum internalFmt = _srgb
			? tfi.m_internalFmtSrgb
			: tfi.m_internalFmt
			;
		GLenum fmt = _srgb
			? tfi.m_fmtSrgb
			: tfi.m_fmt
			;

		GLsizei bpp  = bimg::getBitsPerPixel(bimg::TextureFormat::Enum(_format) );
		GLsizei size = (_dim*_dim*bpp)/8;
		void* data = NULL;

		if (bimg::isDepth(bimg::TextureFormat::Enum(_format) ) )
		{
			_srgb    = false;
			_mipmaps = false;
			_array   = false;
		}
		else
		{
			data = bx::alignPtr(alloca(size+16), 0, 16);
		}

		flushGlError();
		GLenum err = 0;

		const GLenum target = _array
			? GL_TEXTURE_2D_ARRAY
			: GL_TEXTURE_2D
			;

		if (bimg::isCompressed(bimg::TextureFormat::Enum(_format) ) )
		{
			for (uint32_t ii = 0, dim = _dim; ii < (_mipmaps ? 5u : 1u) && 0 == err; ++ii, dim >>= 1)
			{
				dim = bx::uint32_max(1, dim);
				uint32_t block = bx::uint32_max(4, dim);
				size = (block*block*bpp)/8;
				OpenGL::compressedTexImage(target, ii, internalFmt, dim, dim, 0, 0, size, data);
				err |= getGlError();
			}
		}
		else
		{
			for (uint32_t ii = 0, dim = _dim; ii < (_mipmaps ? 5u : 1u) && 0 == err; ++ii, dim >>= 1)
			{
				dim = bx::uint32_max(1, dim);
				size = (dim*dim*bpp)/8;
				OpenGL::texImage(target, 0, ii, internalFmt, dim, dim, 0, 0, fmt, tfi.m_type, data);
				err |= getGlError();
			}
		}

		return err;
	}

#if BX_PLATFORM_EMSCRIPTEN
	static bool isTextureFormatValidPerSpec(
		  TextureFormat::Enum _format
		, bool _srgb
		, bool _mipAutogen
		)
	{
		// Avoid creating test textures for WebGL, that causes error noise in the browser
		// console; instead examine the supported texture formats from the spec.
		EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_get_current_context();

		EmscriptenWebGLContextAttributes attrs;
		EMSCRIPTEN_CHECK(emscripten_webgl_get_context_attributes(ctx, &attrs) );

		const int glesVersion = attrs.majorVersion + 1;

		switch(_format)
		{
			case TextureFormat::A8:
			case TextureFormat::R8: // Luminance
			case TextureFormat::R5G6B5:
			case TextureFormat::RGBA4:
			case TextureFormat::RGB5A1:
				// GLES2 formats without sRGB.
				return !_srgb;

			case TextureFormat::D16:
				// GLES2 formats without sRGB, depth textures do not support mipmaps.
				return !_srgb
					&& !_mipAutogen
					;

			case TextureFormat::R16F:
			case TextureFormat::R32F:
			case TextureFormat::RG8:
			case TextureFormat::RG16F:
			case TextureFormat::RG32F:
			case TextureFormat::RGB10A2:
			case TextureFormat::RG11B10F:
				// GLES3 formats without sRGB
				return !_srgb
					&& glesVersion >= 3
					;

			case TextureFormat::R8I:
			case TextureFormat::R8U:
			case TextureFormat::R16I:
			case TextureFormat::R16U:
			case TextureFormat::R32I:
			case TextureFormat::R32U:
			case TextureFormat::RG8I:
			case TextureFormat::RG8U:
			case TextureFormat::RG16I:
			case TextureFormat::RG16U:
			case TextureFormat::RG32I:
			case TextureFormat::RG32U:
			case TextureFormat::RGB8I:
			case TextureFormat::RGB8U:
			case TextureFormat::RGBA8I:
			case TextureFormat::RGBA8U:
			case TextureFormat::RGBA16I:
			case TextureFormat::RGBA16U:
			case TextureFormat::RGBA32I:
			case TextureFormat::RGBA32U:
			case TextureFormat::D32F:
			case TextureFormat::R8S:
			case TextureFormat::RG8S:
			case TextureFormat::RGB8S:
			case TextureFormat::RGBA8S:
			case TextureFormat::RGB9E5F:
				// GLES3 formats without sRGB that are not texture filterable or color renderable.
				return !_srgb && glesVersion >= 3
					&& !_mipAutogen
					;

			case TextureFormat::D24:
			case TextureFormat::D24S8:
			case TextureFormat::D32:
				// GLES3 formats without sRGB, depth textures do not support mipmaps.
				return !_srgb && !_mipAutogen
					&& (glesVersion >= 3 || emscripten_webgl_enable_extension(ctx, "WEBGL_depth_texture") )
					;

			case TextureFormat::D16F:
			case TextureFormat::D24F:
				// GLES3 depth formats without sRGB, depth textures do not support mipmaps.
				return !_srgb
					&& !_mipAutogen
					&& glesVersion >= 3
					;

			case TextureFormat::RGBA16F:
			case TextureFormat::RGBA32F:
				// GLES3 formats without sRGB
				return !_srgb
					&& (glesVersion >= 3 || emscripten_webgl_enable_extension(ctx, "OES_texture_half_float") )
					;

			case TextureFormat::RGB8:
			case TextureFormat::RGBA8:
				// sRGB formats
				return !_srgb
					|| glesVersion >= 3
					|| emscripten_webgl_enable_extension(ctx, "EXT_sRGB")
					;

			case TextureFormat::BC1:
			case TextureFormat::BC2:
			case TextureFormat::BC3:
				return            emscripten_webgl_enable_extension(ctx, "WEBGL_compressed_texture_s3tc")
					&& (!_srgb || emscripten_webgl_enable_extension(ctx, "WEBGL_compressed_texture_s3tc_srgb") )
					;

			case TextureFormat::PTC12:
			case TextureFormat::PTC14:
			case TextureFormat::PTC12A:
			case TextureFormat::PTC14A:
				return !_srgb
					&& emscripten_webgl_enable_extension(ctx, "WEBGL_compressed_texture_pvrtc")
					;

			case TextureFormat::ETC1:
				return !_srgb
					&& emscripten_webgl_enable_extension(ctx, "WEBGL_compressed_texture_etc1")
					;

			case TextureFormat::ETC2:
			case TextureFormat::ETC2A:
			case TextureFormat::ETC2A1:
				return emscripten_webgl_enable_extension(ctx, "WEBGL_compressed_texture_etc");

			case TextureFormat::ASTC4x4:
			case TextureFormat::ASTC5x5:
			case TextureFormat::ASTC6x6:
			case TextureFormat::ASTC8x5:
			case TextureFormat::ASTC8x6:
			case TextureFormat::ASTC10x5:
				return emscripten_webgl_enable_extension(ctx, "WEBGL_compressed_texture_astc");

			default:
				break;
		}

		return false;
	}
#endif // BX_PLATFORM_EMSCRIPTEN

	static bool isTextureFormatValid(
		  TextureFormat::Enum _format
		, bool _srgb = false
		, bool _mipAutogen = false
		, bool _array = false
		, GLsizei _dim = 16
		)
	{
#if BX_PLATFORM_EMSCRIPTEN
		// On web platform read the validity of textures based on the available GL context and extensions
		// to avoid developer unfriendly console error noise that would come from probing.
		BX_UNUSED(_array, _dim);
		return isTextureFormatValidPerSpec(_format, _srgb, _mipAutogen);
#else
		// On other platforms probe the supported textures.
		const TextureFormatInfo& tfi = OpenGL::s_textureFormat[_format];
		GLenum internalFmt = _srgb
			? tfi.m_internalFmtSrgb
			: tfi.m_internalFmt
			;
		if (GL_ZERO == internalFmt)
		{
			return false;
		}

		const GLenum target = _array
			? GL_TEXTURE_2D_ARRAY
			: GL_TEXTURE_2D
			;

		GLuint id;
		GL_CHECK(glGenTextures(1, &id) );
		GL_CHECK(glBindTexture(target, id) );

		GLenum err = 0;
		if (_array)
		{
			glTexStorage3D(target
				, 1 + GLsizei(bx::log2( (int32_t)_dim) )
				, internalFmt
				, _dim
				, _dim
				, _dim
				);
			err = getGlError();
		}

		if (0 == err)
		{
			err = initTestTexture(_format, _srgb, _mipAutogen, _array, _dim);
			BX_WARN(0 == err, "TextureFormat::%s %s%s%sis not supported (%x: %s)."
				, getName(_format)
				, _srgb       ? "+sRGB "       : ""
				, _mipAutogen ? "+mipAutoGen " : ""
				, _array      ? "+array "      : ""
				, err
				, glEnumName(err)
				);

			if (0 == err
			&&  _mipAutogen)
			{
				glGenerateMipmap(target);
				err = getGlError();
			}
		}

		GL_CHECK(glDeleteTextures(1, &id) );

		return 0 == err;
#endif
	}

	static bool isImageFormatValid(TextureFormat::Enum _format, GLsizei _dim = 16)
	{
		if (GL_ZERO == OpenGL::s_imageFormat[_format])
		{
			return false;
		}

		GLuint id;
		GL_CHECK(glGenTextures(1, &id) );
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, id) );

		flushGlError();
		GLenum err = 0;

		glTexStorage2D(GL_TEXTURE_2D, 1, OpenGL::s_imageFormat[_format], _dim, _dim);
		err |= getGlError();
		if (0 == err)
		{
			glBindImageTexture(0
				, id
				, 0
				, GL_FALSE
				, 0
				, GL_READ_WRITE
				, OpenGL::s_imageFormat[_format]
				);
			err |= getGlError();
		}

		GL_CHECK(glDeleteTextures(1, &id) );

		return 0 == err;
	}

#if BX_PLATFORM_EMSCRIPTEN
	static bool isFramebufferFormatValidPerSpec(
		  TextureFormat::Enum _format
		, bool _srgb
		, bool _writeOnly
		)
	{
		// Avoid creating test textures for WebGL, that causes error noise in the browser console; instead examine the supported texture formats from the spec.
		EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_get_current_context();

		EmscriptenWebGLContextAttributes attrs;
		EMSCRIPTEN_CHECK(emscripten_webgl_get_context_attributes(ctx, &attrs) );

		const int glesVersion = attrs.majorVersion + 1;

		switch(_format)
		{
			// GLES2 textures
			case TextureFormat::R5G6B5:
			case TextureFormat::RGBA4:
			case TextureFormat::RGB5A1:
			case TextureFormat::D16:
				return !_srgb;

			// GLES2 renderbuffers not a texture in GLES3
			case TextureFormat::D0S8:
				return !_srgb
					&& _writeOnly
					;

			// GLES2 textures that are not renderbuffers
			case TextureFormat::RGB8:
			case TextureFormat::RGBA8:
				return !_srgb
					&& (!_writeOnly || glesVersion >= 3)
					;

			// GLES3 EXT_color_buffer_float renderbuffer formats
			case TextureFormat::R16F:
			case TextureFormat::RG16F:
			case TextureFormat::R32F:
			case TextureFormat::RG32F:
			case TextureFormat::RG11B10F:
				if (_writeOnly)
				{
					return emscripten_webgl_enable_extension(ctx, "EXT_color_buffer_float");
				}

				return !_srgb && glesVersion >= 3;

			// GLES2 float extension:
			case TextureFormat::RGBA16F:
				if (_writeOnly && emscripten_webgl_enable_extension(ctx, "EXT_color_buffer_half_float") )
				{
					return true;
				}
				BX_FALLTHROUGH;

			case TextureFormat::RGBA32F:
				if (_writeOnly)
				{
					return emscripten_webgl_enable_extension(ctx, "EXT_color_buffer_float") || emscripten_webgl_enable_extension(ctx, "WEBGL_color_buffer_float");
				}

				// GLES3 formats without sRGB
				return !_srgb
					&& (glesVersion >= 3 || emscripten_webgl_enable_extension(ctx, "OES_texture_half_float") )
					;

			case TextureFormat::D24:
			case TextureFormat::D24S8:
				// GLES3 formats without sRGB, depth textures do not support mipmaps.
				return !_srgb
					&& (glesVersion >= 3 || (!_writeOnly && emscripten_webgl_enable_extension(ctx, "WEBGL_depth_texture") ) )
					;

			case TextureFormat::D32:
				// GLES3 formats without sRGB, depth textures do not support mipmaps.
				return !_srgb
					&& !_writeOnly
					&& (glesVersion >= 3 || emscripten_webgl_enable_extension(ctx, "WEBGL_depth_texture") )
					;

			// GLES3 textures
			case TextureFormat::R8:
			case TextureFormat::RG8:
			case TextureFormat::R8I:
			case TextureFormat::R8U:
			case TextureFormat::R16I:
			case TextureFormat::R16U:
			case TextureFormat::R32I:
			case TextureFormat::R32U:
			case TextureFormat::RG8I:
			case TextureFormat::RG8U:
			case TextureFormat::RGBA8I:
			case TextureFormat::RGBA8U:
			case TextureFormat::RG16I:
			case TextureFormat::RG16U:
			case TextureFormat::RG32I:
			case TextureFormat::RG32U:
			case TextureFormat::RGBA16I:
			case TextureFormat::RGBA16U:
			case TextureFormat::RGBA32I:
			case TextureFormat::RGBA32U:
			case TextureFormat::RGB10A2:
			case TextureFormat::D16F:
			case TextureFormat::D24F:
			case TextureFormat::D32F:
				return !_srgb
					&& glesVersion >= 3
					;

			case TextureFormat::BGRA8:
				return !_srgb
					&& _writeOnly
					&& glesVersion >= 3
					;

			default:
				break;
		}

		return false;
	}
#endif

	static bool isFramebufferFormatValid(
		  TextureFormat::Enum _format
		, bool _srgb = false
		, bool _writeOnly = false
		, GLsizei _dim = 16
		)
	{
#if BX_PLATFORM_EMSCRIPTEN
		// On web platform read the validity of framebuffers based on the available GL context and extensions
		// to avoid developer unfriendly console error noise that would come from probing.
		BX_UNUSED(_dim);
		return isFramebufferFormatValidPerSpec(_format, _srgb, _writeOnly);
#else
		// On other platforms probe the supported textures.
		const TextureFormatInfo& tfi = OpenGL::s_textureFormat[_format];
		GLenum internalFmt = _srgb
			? tfi.m_internalFmtSrgb
			: tfi.m_internalFmt
			;
		if (GL_ZERO == internalFmt)
		{
			return false;
		}

		if (_writeOnly)
		{
			GLuint rbo;
			glGenRenderbuffers(1, &rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, rbo);

			glRenderbufferStorage(GL_RENDERBUFFER
				, OpenGL::s_rboFormat[_format]
				, _dim
				, _dim
				);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			glDeleteRenderbuffers(1, &rbo);

			GLenum err = getGlError();
			return 0 == err;
		}

		GLuint fbo;
		GL_CHECK(glGenFramebuffers(1, &fbo) );
		GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, fbo) );

		GLuint id;
		GL_CHECK(glGenTextures(1, &id) );
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, id) );

		GLenum err = initTestTexture(_format, _srgb, false, false, _dim);

		GLenum attachment;
		if (bimg::isDepth(bimg::TextureFormat::Enum(_format) ) )
		{
			const bimg::ImageBlockInfo& info = bimg::getBlockInfo(bimg::TextureFormat::Enum(_format) );
			if (0 == info.depthBits)
			{
				attachment = GL_STENCIL_ATTACHMENT;
			}
			else if (0 == info.stencilBits)
			{
				attachment = GL_DEPTH_ATTACHMENT;
			}
			else
			{
				attachment = GL_DEPTH_STENCIL_ATTACHMENT;
			}
		}
		else
		{
			attachment = GL_COLOR_ATTACHMENT0;
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER
				, attachment
				, GL_TEXTURE_2D
				, id
				, 0
				);
		err = getGlError();

		if (0 == err)
		{
			err = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		}

		GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0) );
		GL_CHECK(glDeleteFramebuffers(1, &fbo) );

		GL_CHECK(glDeleteTextures(1, &id) );

		return GL_FRAMEBUFFER_COMPLETE == err;
#endif
	}

	void updateExtension(const bx::StringView& _name)
	{
		bx::StringView ext(_name);
		if (0 == bx::strCmp(ext, "GL_", 3) ) // skip GL_
		{
			ext.set(ext.getPtr()+3, ext.getTerm() );
		}

		bool supported = false;
		for (uint32_t ii = 0; ii < Extension::Count; ++ii)
		{
			Extension& extension = OpenGL::s_extension[ii];
			if (!extension.m_supported
			&&  extension.m_initialize)
			{
				if (0 == bx::strCmp(ext, extension.m_name) )
				{
#if BX_PLATFORM_EMSCRIPTEN
					EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_get_current_context();
					supported = emscripten_webgl_enable_extension(ctx, extension.m_name);
#else
					supported = true;
#endif
					extension.m_supported = supported;
					break;
				}
			}
		}

		BX_TRACE("GL_EXTENSION %s: %.*s", supported ? " (supported)" : "", _name.getLength(), _name.getPtr() );
		BX_UNUSED(supported);
	}

	struct VendorId
	{
		const char* name;
		uint16_t id;
	};

	static const VendorId s_vendorIds[] =
	{
		{ "NVIDIA Corporation",           BGFX_PCI_ID_NVIDIA },
		{ "Advanced Micro Devices, Inc.", BGFX_PCI_ID_AMD    },
		{ "Intel",                        BGFX_PCI_ID_INTEL  },
		{ "ATI Technologies Inc.",        BGFX_PCI_ID_AMD    },
		{ "ARM",                          BGFX_PCI_ID_ARM    },
	};

	const char* toString(GLenum _enum)
	{
		switch (_enum)
		{
		case GL_DEBUG_SOURCE_API:               return "API";
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:     return "WinSys";
		case GL_DEBUG_SOURCE_SHADER_COMPILER:   return "Shader";
		case GL_DEBUG_SOURCE_THIRD_PARTY:       return "3rdparty";
		case GL_DEBUG_SOURCE_APPLICATION:       return "Application";
		case GL_DEBUG_SOURCE_OTHER:             return "Other";
		case GL_DEBUG_TYPE_ERROR:               return "Error";
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "Deprecated behavior";
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  return "Undefined behavior";
		case GL_DEBUG_TYPE_PORTABILITY:         return "Portability";
		case GL_DEBUG_TYPE_PERFORMANCE:         return "Performance";
		case GL_DEBUG_TYPE_OTHER:               return "Other";
		case GL_DEBUG_SEVERITY_HIGH:            return "High";
		case GL_DEBUG_SEVERITY_MEDIUM:          return "Medium";
		case GL_DEBUG_SEVERITY_LOW:             return "Low";
		case GL_DEBUG_SEVERITY_NOTIFICATION:    return "SPAM";
		default:
			break;
		}

		return "<unknown>";
	}

	void GL_APIENTRY debugProcCb(GLenum _source, GLenum _type, GLuint _id, GLenum _severity, GLsizei /*_length*/, const GLchar* _message, const void* /*_userParam*/)
	{
		if (GL_DEBUG_SEVERITY_NOTIFICATION != _severity)
		{
			BX_TRACE("src %s, type %s, id %d, severity %s, '%s'"
				, toString(_source)
				, toString(_type)
				, _id
				, toString(_severity)
				, _message
			);
			BX_UNUSED(_source, _type, _id, _severity, _message);
		}
	}

	static uint32_t getGLStringHash(GLenum _name)
	{
		const char* str = (const char*)glGetString(_name);
		getGlError(); // ignore error if glGetString returns NULL.
		if (NULL != str)
		{
			return bx::hash<bx::HashMurmur2A>(str, (uint32_t)bx::strLen(str));
		}

		return 0;
	}

	static const char* getGLString(GLenum _name)
	{
		const char* str = (const char*)glGetString(_name);
		getGlError(); // ignore error if glGetString returns NULL.
		if (NULL != str)
		{
			return str;
		}

		return "<unknown>";
	}

	GLint glGet(GLenum _pname)
	{
		GLint result = 0;
		glGetIntegerv(_pname, &result);
		GLenum err = getGlError();
		BX_WARN(0 == err, "glGetIntegerv(0x%04x, ...) failed with GL error: 0x%04x.", _pname, err);
		return 0 == err ? result : 0;
	}
	
    RendererContextGL::RendererContextGL()
        : m_numWindows(1)
        , m_rtMsaa(false)
        , m_fbDiscard(BGFX_CLEAR_NONE)
        , m_capture(NULL)
        , m_captureSize(0)
        , m_maxAnisotropy(0.0f)
        , m_maxAnisotropyDefault(0.0f)
        , m_maxMsaa(0)
        , m_vao(0)
        , m_blitSupported(false)
        , m_readBackSupported(BX_ENABLED(BGFX_CONFIG_RENDERER_OPENGL))
        , m_vaoSupport(false)
        , m_samplerObjectSupport(false)
        , m_shadowSamplersSupport(false)
        , m_srgbWriteControlSupport(BX_ENABLED(BGFX_CONFIG_RENDERER_OPENGL))
        , m_borderColorSupport(BX_ENABLED(BGFX_CONFIG_RENDERER_OPENGL))
        , m_programBinarySupport(false)
        , m_textureSwizzleSupport(false)
        , m_depthTextureSupport(false)
        , m_timerQuerySupport(false)
        , m_occlusionQuerySupport(false)
        , m_atocSupport(false)
        , m_conservativeRasterSupport(false)
        , m_flip(false)
        , m_hash((BX_PLATFORM_WINDOWS << 1) | BX_ARCH_64BIT)
        , m_backBufferFbo(0)
        , m_msaaBackBufferFbo(0)
        , m_msaaBlitProgram(0)
        , m_clearQuadColor(BGFX_INVALID_HANDLE)
        , m_clearQuadDepth(BGFX_INVALID_HANDLE)
    {
        bx::memSet(m_msaaBackBufferRbos, 0, sizeof(m_msaaBackBufferRbos));
    }

    RendererContextGL::~RendererContextGL()
    {
    }

    bool RendererContextGL::init(const Init& _init)
    {
        struct ErrorState
        {
            enum Enum
            {
                Default,
            };
        };

        ErrorState::Enum errorState = ErrorState::Default;

        if (_init.debug
            || _init.profile)
        {
            m_renderdocdll = loadRenderDoc();
        }

        m_fbh.idx = kInvalidHandle;
        bx::memSet(m_uniforms, 0, sizeof(m_uniforms));
        bx::memSet(&m_resolution, 0, sizeof(m_resolution));

        setRenderContextSize(_init.resolution.width, _init.resolution.height);

        m_vendor = getGLString(GL_VENDOR);
        m_renderer = getGLString(GL_RENDERER);
        m_version = getGLString(GL_VERSION);
        m_glslVersion = getGLString(GL_SHADING_LANGUAGE_VERSION);

        {
            if (BX_ENABLED(BX_PLATFORM_EMSCRIPTEN))
            {
                int32_t majorGlVersion = 0;
                int32_t minorGlVersion = 0;
                const char* version = m_version;

                while (*version && !bx::isNumeric(*version))
                {
                    ++version;
                }

                bx::fromString(&majorGlVersion, version);
                bx::fromString(&minorGlVersion, version + 2);
                int32_t glVersion = majorGlVersion * 10 + minorGlVersion;

                BX_TRACE("WebGL context version %d (%d.%d).", glVersion, majorGlVersion, minorGlVersion);
            }
        }

        for (uint32_t ii = 0; ii < BX_COUNTOF(s_vendorIds); ++ii)
        {
            const VendorId& vendorId = s_vendorIds[ii];
            if (0 == bx::strCmp(vendorId.name, m_vendor, bx::strLen(vendorId.name)))
            {
                g_caps.vendorId = vendorId.id;
                break;
            }
        }

        m_workaround.reset();

        GLint numCmpFormats = 0;
        GL_CHECK(glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &numCmpFormats));
        BX_TRACE("GL_NUM_COMPRESSED_TEXTURE_FORMATS %d", numCmpFormats);

        GLint* cmpFormat = NULL;

        if (0 < numCmpFormats)
        {
            numCmpFormats = numCmpFormats > 256 ? 256 : numCmpFormats;
            cmpFormat = (GLint*)alloca(sizeof(GLint) * numCmpFormats);
            GL_CHECK(glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, cmpFormat));

            for (GLint ii = 0; ii < numCmpFormats; ++ii)
            {
                GLint internalFmt = cmpFormat[ii];
                uint32_t fmt = uint32_t(TextureFormat::Unknown);
                for (uint32_t jj = 0; jj < fmt; ++jj)
                {
                    if (OpenGL::s_textureFormat[jj].m_internalFmt == (GLenum)internalFmt)
                    {
                        OpenGL::s_textureFormat[jj].m_supported = true;
                        fmt = jj;
                    }
                }

                BX_TRACE("  %3d: %8x %s", ii, internalFmt, getName((TextureFormat::Enum)fmt));
            }
        }

        if (BX_ENABLED(BGFX_CONFIG_DEBUG))
        {
#define GL_GET(_pname, _min) BX_TRACE("  " #_pname " %d (min: %d)", glGet(_pname), _min)
            BX_TRACE("Defaults:");
#if BGFX_CONFIG_RENDERER_OPENGL >= 41 || BGFX_CONFIG_RENDERER_OPENGLES
            GL_GET(GL_MAX_FRAGMENT_UNIFORM_VECTORS, 16);
            GL_GET(GL_MAX_VERTEX_UNIFORM_VECTORS, 128);
            GL_GET(GL_MAX_VARYING_VECTORS, 8);
#else
            GL_GET(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, 16 * 4);
            GL_GET(GL_MAX_VERTEX_UNIFORM_COMPONENTS, 128 * 4);
            GL_GET(GL_MAX_VARYING_FLOATS, 8 * 4);
#endif // BGFX_CONFIG_RENDERER_OPENGL >= 41 || BGFX_CONFIG_RENDERER_OPENGLES
            GL_GET(GL_MAX_VERTEX_ATTRIBS, 8);
            GL_GET(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, 8);
            GL_GET(GL_MAX_CUBE_MAP_TEXTURE_SIZE, 16);
            GL_GET(GL_MAX_TEXTURE_IMAGE_UNITS, 8);
            GL_GET(GL_MAX_TEXTURE_SIZE, 64);
            GL_GET(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, 0);
            GL_GET(GL_MAX_RENDERBUFFER_SIZE, 1);
            GL_GET(GL_MAX_COLOR_ATTACHMENTS, 1);
            GL_GET(GL_MAX_DRAW_BUFFERS, 1);

#undef GL_GET

            BX_TRACE("      Vendor: %s", m_vendor);
            BX_TRACE("    Renderer: %s", m_renderer);
            BX_TRACE("     Version: %s", m_version);
            BX_TRACE("GLSL version: %s", m_glslVersion);
        }

		g_caps.limits.maxVertexTextureImage = glGet(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS);

        // Initial binary shader hash depends on driver version.
        m_hash = ((BX_PLATFORM_WINDOWS << 1) | BX_ARCH_64BIT)
            ^ (uint64_t(getGLStringHash(GL_VENDOR)) << 32)
            ^ (uint64_t(getGLStringHash(GL_RENDERER)) << 0)
            ^ (uint64_t(getGLStringHash(GL_VERSION)) << 16)
            ;

        if (OpenGL::Support_GLES3_1()
            && 0 == bx::strCmp(m_vendor, "Imagination Technologies")
            && !bx::strFind(m_version, "(SDK 3.5@3510720)").isEmpty())
        {
            // Skip initializing extensions that are broken in emulator.
			OpenGL::s_extension[Extension::ARB_program_interface_query].m_initialize =
				OpenGL::s_extension[Extension::ARB_shader_storage_buffer_object].m_initialize = false;
        }

        if (Platform::IsMobilePlatform()
            && 0 == bx::strCmp(m_vendor, "Imagination Technologies")
            && !bx::strFind(m_version, "1.8@905891").isEmpty())
        {
            m_workaround.m_detachShader = false;
        }

		bx::StringView renderStr(m_renderer);
		bool bIsNavidiaDevice = !bx::findIdentifierMatch(renderStr, "NVIDIA").isEmpty();
		bool bIsPowerVRDevice = !bx::findIdentifierMatch(renderStr, "PowerVR").isEmpty();
		bool bIsAdrenoDevice = !bx::findIdentifierMatch(renderStr, "Adreno").isEmpty();
		bool bIsAdreno5xxDevice = (bIsAdrenoDevice && !bx::findIdentifierMatch(renderStr, "(TM)5").isEmpty());
		bool bIsMaliDevice = !bx::findIdentifierMatch(renderStr, "Mali").isEmpty();

        if (BX_ENABLED(BGFX_CONFIG_RENDERER_USE_EXTENSIONS))
        {
            const char* extensions = (const char*)glGetString(GL_EXTENSIONS);
            getGlError(); // ignore error if glGetString returns NULL.
            if (NULL != extensions)
            {
                bx::StringView ext(extensions);
                uint32_t index = 0;
                while (!ext.isEmpty())
                {
                    const bx::StringView space = bx::strFind(ext, ' ');
                    const bx::StringView token = bx::StringView(ext.getPtr(), space.getPtr());
                    updateExtension(token);

                    ext.set(space.getPtr() + (space.isEmpty() ? 0 : 1), ext.getTerm());

                    ++index;
                }

				OpenGL::s_bSupportsColorBufferHalfFloat = (Platform::IsPCPlatform())
					|| (!bx::findIdentifierMatch(ext, "GL_EXT_color_buffer_half_float").isEmpty());
				OpenGL::s_bSupportsColorBufferFloat = (Platform::IsPCPlatform())
					|| (!bx::findIdentifierMatch(ext, "GL_EXT_color_buffer_float").isEmpty());

				OpenGL::s_FrameBufferFetchNeedDef = (!Platform::IsPCPlatform()) &&
					!bx::findIdentifierMatch(ext, "GL_EXT_shader_framebuffer_fetch").isEmpty();
                OpenGL::s_FrameBufferFetch = (OpenGL::s_FrameBufferFetchNeedDef
                    || (!bx::findIdentifierMatch(ext, "GL_NV_shader_framebuffer_fetch").isEmpty())
                    || (!bx::findIdentifierMatch(ext, "GL_ARM_shader_framebuffer_fetch").isEmpty())
                    );

				OpenGL::bSupportsShaderMRTFramebufferFetch = Platform::IsMobilePlatform() &&
					(!bx::findIdentifierMatch(ext, "GL_EXT_shader_framebuffer_fetch").isEmpty()
					|| !bx::findIdentifierMatch(ext, "GL_NV_shader_framebuffer_fetch").isEmpty());

				OpenGL::s_FrameBufferFetchDepthStencil = (!Platform::IsPCPlatform())
					&& (!bx::findIdentifierMatch(ext, "GL_ARM_shader_framebuffer_fetch_depth_stencil").isEmpty());

				OpenGL::s_ShaderTextureLod = Platform::IsPCPlatform()
					|| OpenGL::Support_GLES3_0()
					|| (!bx::findIdentifierMatch(ext, "GL_EXT_shader_texture_lod").isEmpty());

				OpenGL::OESTextureType oseType = OpenGL::None;
				bool hasImageExternal = (!bx::findIdentifierMatch(ext, "GL_OES_EGL_image_external").isEmpty());
				bool hasImageExternal3 = (!bx::findIdentifierMatch(ext, "OES_EGL_image_external_essl3").isEmpty());
				if (hasImageExternal || hasImageExternal3)
				{
					oseType = OpenGL::OESTextureType::OES100;

					DeviceFeatureLevel::Enum dlevel = OpenGL::GetCurrentFeatureLevel();
					if (dlevel == DeviceFeatureLevel::ES3_0)
					{
						if (hasImageExternal3 || bIsAdreno5xxDevice)
						{
							oseType = OpenGL::OESTextureType::OES300;
						}
					}

					if (bIsNavidiaDevice)
					{
						oseType = OpenGL::OESTextureType::OES100;
					}
				}
				OpenGL::s_OesType = oseType;
            }
            else if (NULL != glGetStringi)
            {
                GLint numExtensions = 0;
                glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
                getGlError(); // ignore error if glGetIntegerv returns NULL.

                for (GLint index = 0; index < numExtensions; ++index)
                {
                    const char* name = (const char*)glGetStringi(GL_EXTENSIONS, index);
                    updateExtension(name);
                }
            }

            BX_TRACE("Supported extensions:");
            for (uint32_t ii = 0; ii < Extension::Count; ++ii)
            {
                if (OpenGL::s_extension[ii].m_supported)
                {
                    BX_TRACE("\t%2d: %s", ii, OpenGL::s_extension[ii].m_name);
                }
            }
        }

        if (BX_ENABLED(BGFX_CONFIG_RENDERER_OPENGL)
            && !OpenGL::s_extension[Extension::ARB_framebuffer_object].m_supported)
        {
            BX_TRACE("Init error: ARB_framebuffer_object not supported.");
            goto error;
        }

        {
            // Allow all texture filters.
			bx::memSet(OpenGL::s_textureFilter, true, BX_COUNTOF(OpenGL::s_textureFilter));

            bool bc123Supported = 0
                || OpenGL::s_extension[Extension::EXT_texture_compression_s3tc].m_supported
                || OpenGL::s_extension[Extension::MOZ_WEBGL_compressed_texture_s3tc].m_supported
                || OpenGL::s_extension[Extension::WEBGL_compressed_texture_s3tc].m_supported
                || OpenGL::s_extension[Extension::WEBKIT_WEBGL_compressed_texture_s3tc].m_supported
                ;
            OpenGL::s_textureFormat[TextureFormat::BC1].m_supported |= bc123Supported
                || OpenGL::s_extension[Extension::ANGLE_texture_compression_dxt1].m_supported
                || OpenGL::s_extension[Extension::EXT_texture_compression_dxt1].m_supported
                ;

            if (!OpenGL::s_textureFormat[TextureFormat::BC1].m_supported
                && (OpenGL::s_textureFormat[TextureFormat::BC2].m_supported || OpenGL::s_textureFormat[TextureFormat::BC3].m_supported))
            {
                // If RGBA_S3TC_DXT1 is not supported, maybe RGB_S3TC_DXT1 is?
                for (GLint ii = 0; ii < numCmpFormats; ++ii)
                {
                    if (GL_COMPRESSED_RGB_S3TC_DXT1_EXT == cmpFormat[ii])
                    {
                        setTextureFormat(TextureFormat::BC1, GL_COMPRESSED_RGB_S3TC_DXT1_EXT, GL_COMPRESSED_RGB_S3TC_DXT1_EXT);
                        OpenGL::s_textureFormat[TextureFormat::BC1].m_supported = true;
                        break;
                    }
                }
            }

            OpenGL::s_textureFormat[TextureFormat::BC2].m_supported |= bc123Supported
                || OpenGL::s_extension[Extension::ANGLE_texture_compression_dxt3].m_supported
                || OpenGL::s_extension[Extension::CHROMIUM_texture_compression_dxt3].m_supported
                ;

            OpenGL::s_textureFormat[TextureFormat::BC3].m_supported |= bc123Supported
                || OpenGL::s_extension[Extension::ANGLE_texture_compression_dxt5].m_supported
                || OpenGL::s_extension[Extension::CHROMIUM_texture_compression_dxt5].m_supported
                ;

            if (OpenGL::s_extension[Extension::EXT_texture_compression_latc].m_supported)
            {
                setTextureFormat(TextureFormat::BC4, GL_COMPRESSED_LUMINANCE_LATC1_EXT, GL_COMPRESSED_LUMINANCE_LATC1_EXT);
                setTextureFormat(TextureFormat::BC5, GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT, GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT);
            }

            if (OpenGL::s_extension[Extension::ARB_texture_compression_rgtc].m_supported
                || OpenGL::s_extension[Extension::EXT_texture_compression_rgtc].m_supported)
            {
                setTextureFormat(TextureFormat::BC4, GL_COMPRESSED_RED_RGTC1, GL_COMPRESSED_RED_RGTC1);
                setTextureFormat(TextureFormat::BC5, GL_COMPRESSED_RG_RGTC2, GL_COMPRESSED_RG_RGTC2);
            }

            bool etc1Supported = 0
                || OpenGL::s_extension[Extension::OES_compressed_ETC1_RGB8_texture].m_supported
                || OpenGL::s_extension[Extension::WEBGL_compressed_texture_etc1].m_supported
                ;
            OpenGL::s_textureFormat[TextureFormat::ETC1].m_supported |= etc1Supported;

            bool etc2Supported = !!(OpenGL::Support_GLES3_0())
                || OpenGL::s_extension[Extension::ARB_ES3_compatibility].m_supported
                ;
            OpenGL::s_textureFormat[TextureFormat::ETC2].m_supported |= etc2Supported;
            OpenGL::s_textureFormat[TextureFormat::ETC2A].m_supported |= etc2Supported;
            OpenGL::s_textureFormat[TextureFormat::ETC2A1].m_supported |= etc2Supported;

            if (!OpenGL::s_textureFormat[TextureFormat::ETC1].m_supported
                && OpenGL::s_textureFormat[TextureFormat::ETC2].m_supported)
            {
                // When ETC2 is supported  ETC1 texture format settings.
                OpenGL::s_textureFormat[TextureFormat::ETC1].m_internalFmt = GL_COMPRESSED_RGB8_ETC2;
                OpenGL::s_textureFormat[TextureFormat::ETC1].m_fmt = GL_COMPRESSED_RGB8_ETC2;
                OpenGL::s_textureFormat[TextureFormat::ETC1].m_supported = true;
            }

            bool ptc1Supported = 0
                || OpenGL::s_extension[Extension::IMG_texture_compression_pvrtc].m_supported
                || OpenGL::s_extension[Extension::WEBGL_compressed_texture_pvrtc].m_supported
                ;
            OpenGL::s_textureFormat[TextureFormat::PTC12].m_supported |= ptc1Supported;
            OpenGL::s_textureFormat[TextureFormat::PTC14].m_supported |= ptc1Supported;
            OpenGL::s_textureFormat[TextureFormat::PTC12A].m_supported |= ptc1Supported;
            OpenGL::s_textureFormat[TextureFormat::PTC14A].m_supported |= ptc1Supported;

            bool ptc2Supported = OpenGL::s_extension[Extension::IMG_texture_compression_pvrtc2].m_supported;
            OpenGL::s_textureFormat[TextureFormat::PTC22].m_supported |= ptc2Supported;
            OpenGL::s_textureFormat[TextureFormat::PTC24].m_supported |= ptc2Supported;

            if (Platform::IsMobilePlatform())
            {
                if (OpenGL::Support_GLES3_0())
                {
                    setTextureFormat(TextureFormat::R16F, GL_R16F, GL_RED, 0x140B /* == GL_HALF_FLOAT, but bgfx overwrites it globally with GL_HALF_FLOAT_OES */);
                    setTextureFormat(TextureFormat::RG16F, GL_RG16F, GL_RG, 0x140B /* == GL_HALF_FLOAT, but bgfx overwrites it globally with GL_HALF_FLOAT_OES */);
                    setTextureFormat(TextureFormat::RGBA16F, GL_RGBA16F, GL_RGBA, 0x140B /* == GL_HALF_FLOAT, but bgfx overwrites it globally with GL_HALF_FLOAT_OES */);
                }
                else
                {
                    setTextureFormat(TextureFormat::RGBA16F, GL_RGBA, GL_RGBA, GL_HALF_FLOAT); // Note: this is actually GL_HALF_FLOAT_OES and not GL_HALF_FLOAT if compiling for GLES target.
                    setTextureFormat(TextureFormat::RGBA32F, GL_RGBA, GL_RGBA, GL_FLOAT);
                    // internalFormat and format must match:
                    // https://www.khronos.org/opengles/sdk/docs/man/xhtml/glTexImage2D.xml
                    setTextureFormat(TextureFormat::RGBA8, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
                    setTextureFormat(TextureFormat::R5G6B5, GL_RGB, GL_RGB, GL_UNSIGNED_SHORT_5_6_5_REV);
                    setTextureFormat(TextureFormat::RGBA4, GL_RGBA, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4_REV);
                    setTextureFormat(TextureFormat::RGB5A1, GL_RGBA, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV);

                    if (OpenGL::s_extension[Extension::EXT_sRGB].m_supported)
                    {
                        setTextureFormatSrgb(TextureFormat::RGBA8, GL_SRGB_ALPHA_EXT, GL_SRGB_ALPHA_EXT);
                        setTextureFormatSrgb(TextureFormat::RGB8, GL_SRGB_EXT, GL_SRGB_EXT);
                    }

                    if (OpenGL::s_extension[Extension::OES_texture_half_float].m_supported
                        || OpenGL::s_extension[Extension::OES_texture_float].m_supported)
                    {
                        // https://www.khronos.org/registry/gles/extensions/OES/OES_texture_float.txt
                        // When half/float is available via extensions texture will be marked as
                        // incomplete if it uses anything other than nearest filter.
                        const bool linear16F = OpenGL::s_extension[Extension::OES_texture_half_float_linear].m_supported;
                        const bool linear32F = OpenGL::s_extension[Extension::OES_texture_float_linear].m_supported;

                        OpenGL::s_textureFilter[TextureFormat::R16F] = linear16F;
                        OpenGL::s_textureFilter[TextureFormat::RG16F] = linear16F;
                        OpenGL::s_textureFilter[TextureFormat::RGBA16F] = linear16F;
                        OpenGL::s_textureFilter[TextureFormat::R32F] = linear32F;
                        OpenGL::s_textureFilter[TextureFormat::RG32F] = linear32F;
                        OpenGL::s_textureFilter[TextureFormat::RGBA32F] = linear32F;
                    }
                }

                if (BX_ENABLED(BX_PLATFORM_EMSCRIPTEN)
                    && (OpenGL::s_extension[Extension::WEBGL_depth_texture].m_supported
                        || OpenGL::s_extension[Extension::MOZ_WEBGL_depth_texture].m_supported
                        || OpenGL::s_extension[Extension::WEBKIT_WEBGL_depth_texture].m_supported))
                {
                    setTextureFormat(TextureFormat::D16, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT);
                    setTextureFormat(TextureFormat::D24, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT); // N.b. OpenGL ES does not guarantee that there are 24 bits available here, could be 16. See https://www.khronos.org/registry/webgl/extensions/WEBGL_depth_texture/
                    setTextureFormat(TextureFormat::D32, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT); // N.b. same as above.
                    setTextureFormat(TextureFormat::D24S8, GL_DEPTH_STENCIL, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8);
                }

                // OpenGL ES 3.0 depth formats.
                if (OpenGL::Support_GLES3_0())
                {
                    setTextureFormat(TextureFormat::D16, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT);
                    setTextureFormat(TextureFormat::D24, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT);
                    setTextureFormat(TextureFormat::D32, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT);
                    setTextureFormat(TextureFormat::D24S8, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8);
                    setTextureFormat(TextureFormat::D16F, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT); // GLES 3.0 does not have D16F, overshoot to D32F
                    setTextureFormat(TextureFormat::D24F, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT); // GLES 3.0 does not have D24F, overshoot to D32F
                    setTextureFormat(TextureFormat::D32F, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT);
                    setTextureFormat(TextureFormat::D0S8, GL_STENCIL_INDEX8, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE); // Only works as renderbuffer, not as texture
                }
            }

            if (BX_ENABLED(BGFX_CONFIG_RENDERER_OPENGL)
                || OpenGL::Support_GLES3_0())
            {
                setTextureFormat(TextureFormat::R8I, GL_R8I, GL_RED_INTEGER, GL_BYTE);
                setTextureFormat(TextureFormat::R8U, GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE);
                setTextureFormat(TextureFormat::R16I, GL_R16I, GL_RED_INTEGER, GL_SHORT);
                setTextureFormat(TextureFormat::R16U, GL_R16UI, GL_RED_INTEGER, GL_UNSIGNED_SHORT);
                //				setTextureFormat(TextureFormat::RG16,    GL_RG16UI,   GL_RG_INTEGER,   GL_UNSIGNED_SHORT);
                //				setTextureFormat(TextureFormat::RGBA16,  GL_RGBA16UI, GL_RGBA_INTEGER, GL_UNSIGNED_SHORT);
                setTextureFormat(TextureFormat::R32U, GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT);
                setTextureFormat(TextureFormat::RG32U, GL_RG32UI, GL_RG_INTEGER, GL_UNSIGNED_INT);
                setTextureFormat(TextureFormat::RGBA32U, GL_RGBA32UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT);
            }

            if (OpenGL::s_extension[Extension::EXT_texture_format_BGRA8888].m_supported
                || OpenGL::s_extension[Extension::EXT_bgra].m_supported
                || OpenGL::s_extension[Extension::IMG_texture_format_BGRA8888].m_supported
                || OpenGL::s_extension[Extension::APPLE_texture_format_BGRA8888].m_supported)
            {
                if (BX_ENABLED(BGFX_CONFIG_RENDERER_OPENGL))
                {
                    m_readPixelsFmt = GL_BGRA;
                }

                // Mixing GLES and GL extensions here. OpenGL EXT_bgra and
                // APPLE_texture_format_BGRA8888 wants
                // format to be BGRA but internal format to stay RGBA, but
                // EXT_texture_format_BGRA8888 wants both format and internal
                // format to be BGRA.
                //
                // Reference(s):
                // - https://web.archive.org/web/20181126035829/https://www.khronos.org/registry/OpenGL/extensions/EXT/EXT_texture_format_BGRA8888.txt
                // - https://web.archive.org/web/20181126035841/https://www.khronos.org/registry/OpenGL/extensions/EXT/EXT_bgra.txt
                // - https://web.archive.org/web/20181126035851/https://www.khronos.org/registry/OpenGL/extensions/APPLE/APPLE_texture_format_BGRA8888.txt
                //
                if (!OpenGL::s_extension[Extension::EXT_bgra].m_supported
                    && !OpenGL::s_extension[Extension::APPLE_texture_format_BGRA8888].m_supported)
                {
                    OpenGL::s_textureFormat[TextureFormat::BGRA8].m_internalFmt = GL_BGRA;
                }

                if (!isTextureFormatValid(TextureFormat::BGRA8))
                {
                    // Revert back to RGBA if texture can't be created.
                    setTextureFormat(TextureFormat::BGRA8, GL_RGBA, GL_BGRA, GL_UNSIGNED_BYTE);
                }
            }

            if (BX_ENABLED(BX_PLATFORM_EMSCRIPTEN))
            {
                // OpenGL ES does not have reversed BGRA4 and BGR5A1 support.
                setTextureFormat(TextureFormat::RGBA4, GL_RGBA, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4);
                setTextureFormat(TextureFormat::RGB5A1, GL_RGBA, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1);
                setTextureFormat(TextureFormat::R5G6B5, GL_RGB, GL_RGB, GL_UNSIGNED_SHORT_5_6_5);

                if (!OpenGL::Support_GLES3_0())
                {
                    // OpenGL ES 2.0 uses unsized internal formats.
                    OpenGL::s_textureFormat[TextureFormat::RGB8].m_internalFmt = GL_RGB;

                    // OpenGL ES 2.0 does not have R8 texture format, only L8. Open GL ES 2.0 extension https://www.khronos.org/registry/OpenGL/extensions/EXT/EXT_texture_rg.txt
                    // adds support for R8 to GLES 2.0 core contexts. For those use L8 instead.
                    if (!OpenGL::s_extension[Extension::EXT_texture_rg].m_supported)
                    {
                        OpenGL::s_textureFormat[TextureFormat::R8].m_internalFmt = GL_LUMINANCE;
                        OpenGL::s_textureFormat[TextureFormat::R8].m_fmt = GL_LUMINANCE;
                    }
                }
            }

            for (uint32_t ii = BX_ENABLED(BX_PLATFORM_IOS) ? TextureFormat::Unknown : 0 // skip test on iOS!
                ; ii < TextureFormat::Count
                ; ++ii
                )
            {
                if (TextureFormat::Unknown != ii
                    && TextureFormat::UnknownDepth != ii)
                {
                    OpenGL::s_textureFormat[ii].m_supported = isTextureFormatValid(TextureFormat::Enum(ii));
                }
            }

            if (BX_ENABLED(0))
            {
                // Disable all compressed texture formats. For testing only.
                for (uint32_t ii = 0; ii < TextureFormat::Unknown; ++ii)
                {
                    OpenGL::s_textureFormat[ii].m_supported = false;
                }
            }

            const bool computeSupport = false
                || !!(OpenGL::Support_GLES3_1())
                || OpenGL::s_extension[Extension::ARB_compute_shader].m_supported
                ;

            for (uint32_t ii = 0; ii < TextureFormat::Count; ++ii)
            {
                const TextureFormat::Enum fmt = TextureFormat::Enum(ii);

                uint16_t supported = BGFX_CAPS_FORMAT_TEXTURE_NONE;
                supported |= OpenGL::s_textureFormat[ii].m_supported
                    ? BGFX_CAPS_FORMAT_TEXTURE_2D
                    | BGFX_CAPS_FORMAT_TEXTURE_3D
                    | BGFX_CAPS_FORMAT_TEXTURE_CUBE
                    : BGFX_CAPS_FORMAT_TEXTURE_NONE
                    ;

                supported |= isTextureFormatValid(fmt, true)
                    ? BGFX_CAPS_FORMAT_TEXTURE_2D_SRGB
                    | BGFX_CAPS_FORMAT_TEXTURE_3D_SRGB
                    | BGFX_CAPS_FORMAT_TEXTURE_CUBE_SRGB
                    : BGFX_CAPS_FORMAT_TEXTURE_NONE
                    ;

                if (!bimg::isCompressed(bimg::TextureFormat::Enum(fmt)))
                {
                    supported |= isTextureFormatValid(fmt, false, true)
                        ? BGFX_CAPS_FORMAT_TEXTURE_MIP_AUTOGEN
                        : BGFX_CAPS_FORMAT_TEXTURE_NONE
                        ;
                }

                supported |= computeSupport
                    && isImageFormatValid(fmt)
                    ? (BGFX_CAPS_FORMAT_TEXTURE_IMAGE_READ | BGFX_CAPS_FORMAT_TEXTURE_IMAGE_WRITE)
                    : BGFX_CAPS_FORMAT_TEXTURE_NONE
                    ;

                supported |= isFramebufferFormatValid(fmt)
                    ? BGFX_CAPS_FORMAT_TEXTURE_FRAMEBUFFER
                    : BGFX_CAPS_FORMAT_TEXTURE_NONE
                    ;

                supported |= isFramebufferFormatValid(fmt, false, true)
                    ? BGFX_CAPS_FORMAT_TEXTURE_FRAMEBUFFER
                    : BGFX_CAPS_FORMAT_TEXTURE_NONE
                    ;

                if (NULL != glGetInternalformativ)
                {
                    GLint maxSamples;
                    glGetInternalformativ(GL_RENDERBUFFER
                        , OpenGL::s_textureFormat[ii].m_internalFmt
                        , GL_SAMPLES
                        , 1
                        , &maxSamples
                    );
                    GLenum err = getGlError();
                    supported |= 0 == err && maxSamples > 0
                        ? BGFX_CAPS_FORMAT_TEXTURE_FRAMEBUFFER_MSAA
                        : BGFX_CAPS_FORMAT_TEXTURE_NONE
                        ;

                    glGetInternalformativ(GL_TEXTURE_2D_MULTISAMPLE
                        , OpenGL::s_textureFormat[ii].m_internalFmt
                        , GL_SAMPLES
                        , 1
                        , &maxSamples
                    );
                    err = getGlError();
                    supported |= 0 == err && maxSamples > 0
                        ? BGFX_CAPS_FORMAT_TEXTURE_MSAA
                        : BGFX_CAPS_FORMAT_TEXTURE_NONE
                        ;
                }

                g_caps.formats[ii] = supported;
            }

            g_caps.supported |= !!(BGFX_CONFIG_RENDERER_OPENGL || OpenGL::Support_GLES3_0())
                || OpenGL::s_extension[Extension::OES_texture_3D].m_supported
                ? BGFX_CAPS_TEXTURE_3D
                : 0
                ;
            g_caps.supported |= !!(BGFX_CONFIG_RENDERER_OPENGL || OpenGL::Support_GLES3_0())
                || OpenGL::s_extension[Extension::EXT_shadow_samplers].m_supported
                ? BGFX_CAPS_TEXTURE_COMPARE_ALL
                : 0
                ;
            g_caps.supported |= !!(BGFX_CONFIG_RENDERER_OPENGL || OpenGL::Support_GLES3_0())
                || OpenGL::s_extension[Extension::OES_vertex_half_float].m_supported
                ? BGFX_CAPS_VERTEX_ATTRIB_HALF
                : 0
                ;
            g_caps.supported |= false
                || OpenGL::s_extension[Extension::ARB_vertex_type_2_10_10_10_rev].m_supported
                || OpenGL::s_extension[Extension::OES_vertex_type_10_10_10_2].m_supported
                ? BGFX_CAPS_VERTEX_ATTRIB_UINT10
                : 0
                ;
            g_caps.supported |= !!(BGFX_CONFIG_RENDERER_OPENGL || OpenGL::Support_GLES3_0())
                || OpenGL::s_extension[Extension::EXT_frag_depth].m_supported
                ? BGFX_CAPS_FRAGMENT_DEPTH
                : 0
                ;
            g_caps.supported |= OpenGL::s_extension[Extension::ARB_draw_buffers_blend].m_supported
                ? BGFX_CAPS_BLEND_INDEPENDENT
                : 0
                ;
            g_caps.supported |= OpenGL::s_extension[Extension::INTEL_fragment_shader_ordering].m_supported
                ? BGFX_CAPS_FRAGMENT_ORDERING
                : 0
                ;
            g_caps.supported |= !!(BGFX_CONFIG_RENDERER_OPENGL || OpenGL::Support_GLES3_0())
                || OpenGL::s_extension[Extension::OES_element_index_uint].m_supported
                ? BGFX_CAPS_INDEX32
                : 0
                ;

            const bool drawIndirectSupported = false
                || OpenGL::s_extension[Extension::AMD_multi_draw_indirect].m_supported
                || OpenGL::s_extension[Extension::ARB_draw_indirect].m_supported
                || OpenGL::s_extension[Extension::ARB_multi_draw_indirect].m_supported
                || OpenGL::s_extension[Extension::EXT_multi_draw_indirect].m_supported
                ;

            g_caps.supported |= drawIndirectSupported
                ? BGFX_CAPS_DRAW_INDIRECT
                : 0
                ;

            if (OpenGL::s_extension[Extension::ARB_copy_image].m_supported
                || OpenGL::s_extension[Extension::EXT_copy_image].m_supported
                || OpenGL::s_extension[Extension::NV_copy_image].m_supported
                || OpenGL::s_extension[Extension::OES_copy_image].m_supported)
            {
                m_blitSupported = NULL != glCopyImageSubData;
            }

            g_caps.supported |= m_blitSupported || BX_ENABLED(BGFX_GL_CONFIG_BLIT_EMULATION)
                ? BGFX_CAPS_TEXTURE_BLIT
                : 0
                ;

            g_caps.supported |= (m_readBackSupported || BX_ENABLED(BGFX_GL_CONFIG_TEXTURE_READ_BACK_EMULATION))
                ? BGFX_CAPS_TEXTURE_READ_BACK
                : 0
                ;

            g_caps.supported |= false
                || OpenGL::s_extension[Extension::EXT_texture_array].m_supported
                || OpenGL::s_extension[Extension::EXT_gpu_shader4].m_supported
                || (OpenGL::Support_GLES3_0() && !BX_ENABLED(BX_PLATFORM_EMSCRIPTEN))
                ? BGFX_CAPS_TEXTURE_2D_ARRAY
                : 0
                ;

            g_caps.supported |= false
                || OpenGL::s_extension[Extension::EXT_gpu_shader4].m_supported
                || (OpenGL::Support_GLES3_0() && !BX_ENABLED(BX_PLATFORM_EMSCRIPTEN))
                ? BGFX_CAPS_VERTEX_ID
                : 0
                ;

            g_caps.supported |= false
                || OpenGL::s_extension[Extension::ARB_texture_cube_map_array].m_supported
                || OpenGL::s_extension[Extension::EXT_texture_cube_map_array].m_supported
                ? BGFX_CAPS_TEXTURE_CUBE_ARRAY
                : 0
                ;

			g_caps.supported2 |= (!Platform::IsPCPlatform() && OpenGL::SupportFrameBufferFetch()
				? BGFX_CAPS_FRAMEBUFFER_FETCH
				: 0);
			g_caps.supported2 |= (!Platform::IsPCPlatform() && OpenGL::SupportsShaderMRTFramebufferFetch()
				? BGFX_CAPS_MRT_FRAMEBUFFER_FETCH
				: 0);

			g_caps.supported2 |= (OpenGL::SupportsColorBufferHalfFloat()
				? BGFX_CAPS_FRAMEBUFFER_HALF
				: 0);

			g_caps.supported2 |= (OpenGL::SupportsColorBufferFloat()
				? BGFX_CAPS_FRAMEBUFFER_FLOAT
				: 0);

			g_caps.supported2 |= (!Platform::IsPCPlatform() && OpenGL::SupportFrameBufferFetchDepthStencil()
				? BGFX_CAPS_FRAMEBUFFER_FETCH_DEPTH_STENCIL
				: 0);

			g_caps.supported2 |= (m_srgbWriteControlSupport ? BGFX_CAPS_SRGB : 0);

            g_caps.limits.maxTextureSize = uint16_t(glGet(GL_MAX_TEXTURE_SIZE));
            g_caps.limits.maxTextureLayers = BX_ENABLED(BGFX_CONFIG_RENDERER_OPENGL >= 30) || OpenGL::Support_GLES3_0() || OpenGL::s_extension[Extension::EXT_texture_array].m_supported ? uint16_t(bx::max(glGet(GL_MAX_ARRAY_TEXTURE_LAYERS), 1)) : 1;
            g_caps.limits.maxComputeBindings = computeSupport ? BGFX_MAX_COMPUTE_BINDINGS : 0;
            g_caps.limits.maxVertexStreams = BGFX_CONFIG_MAX_VERTEX_STREAMS;

            if (BX_ENABLED(BGFX_CONFIG_RENDERER_OPENGL)
                || OpenGL::Support_GLES3_0()
                || OpenGL::s_extension[Extension::EXT_draw_buffers].m_supported
                || OpenGL::s_extension[Extension::WEBGL_draw_buffers].m_supported)
            {
                g_caps.limits.maxFBAttachments = uint8_t(bx::uint32_clamp(
                    glGet(GL_MAX_DRAW_BUFFERS)
                    , 1
                    , BGFX_CONFIG_MAX_FRAME_BUFFER_ATTACHMENTS)
                );
            }

            //				if (s_extension[Extension::ARB_clip_control].m_supported)
            //				{
            //					GL_CHECK(glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE) );
            //					g_caps.originBottomLeft = true;
            //				}
            //				else
            {
                g_caps.homogeneousDepth = true;
                g_caps.originBottomLeft = true;
            }

            m_vaoSupport = !BX_ENABLED(BX_PLATFORM_EMSCRIPTEN)
                && (OpenGL::Support_GLES3_0()
                    || OpenGL::s_extension[Extension::ARB_vertex_array_object].m_supported
                    || OpenGL::s_extension[Extension::OES_vertex_array_object].m_supported
                    );

            if (m_vaoSupport)
            {
                GL_CHECK(glGenVertexArrays(1, &m_vao));
            }

            m_samplerObjectSupport = !BX_ENABLED(BX_PLATFORM_EMSCRIPTEN)
                && (OpenGL::Support_GLES3_0()
                    || OpenGL::s_extension[Extension::ARB_sampler_objects].m_supported
                    );

            m_shadowSamplersSupport = !!(BGFX_CONFIG_RENDERER_OPENGL || OpenGL::Support_GLES3_0())
                || OpenGL::s_extension[Extension::EXT_shadow_samplers].m_supported
                ;

            m_programBinarySupport = !BX_ENABLED(BX_PLATFORM_EMSCRIPTEN)
                && (OpenGL::Support_GLES3_0()
                    || OpenGL::s_extension[Extension::ARB_get_program_binary].m_supported
                    || OpenGL::s_extension[Extension::OES_get_program_binary].m_supported
                    || OpenGL::s_extension[Extension::IMG_shader_binary].m_supported
                    );

            m_textureSwizzleSupport = false
                || OpenGL::s_extension[Extension::ARB_texture_swizzle].m_supported
                || OpenGL::s_extension[Extension::EXT_texture_swizzle].m_supported
                ;

            m_depthTextureSupport = !!(BGFX_CONFIG_RENDERER_OPENGL || OpenGL::Support_GLES3_0())
                || OpenGL::s_extension[Extension::ANGLE_depth_texture].m_supported
                || OpenGL::s_extension[Extension::CHROMIUM_depth_texture].m_supported
                || OpenGL::s_extension[Extension::GOOGLE_depth_texture].m_supported
                || OpenGL::s_extension[Extension::OES_depth_texture].m_supported
                || OpenGL::s_extension[Extension::MOZ_WEBGL_depth_texture].m_supported
                || OpenGL::s_extension[Extension::WEBGL_depth_texture].m_supported
                || OpenGL::s_extension[Extension::WEBKIT_WEBGL_depth_texture].m_supported
                ;

            m_timerQuerySupport = false
                || OpenGL::s_extension[Extension::ANGLE_timer_query].m_supported
                || OpenGL::s_extension[Extension::ARB_timer_query].m_supported
                || OpenGL::s_extension[Extension::EXT_disjoint_timer_query].m_supported
                || OpenGL::s_extension[Extension::EXT_timer_query].m_supported
                ;

            m_timerQuerySupport &= true
                && NULL != glQueryCounter
                && NULL != glGetQueryObjectiv
                && NULL != glGetQueryObjectui64v
                ;

            m_occlusionQuerySupport = false
                || OpenGL::s_extension[Extension::ARB_occlusion_query].m_supported
                || OpenGL::s_extension[Extension::ARB_occlusion_query2].m_supported
                || OpenGL::s_extension[Extension::EXT_occlusion_query_boolean].m_supported
                || OpenGL::s_extension[Extension::NV_occlusion_query].m_supported
                ;

            m_occlusionQuerySupport &= true
                && NULL != glGenQueries
                && NULL != glDeleteQueries
                && NULL != glBeginQuery
                && NULL != glEndQuery
                ;

            m_atocSupport = OpenGL::s_extension[Extension::ARB_multisample].m_supported;
            m_conservativeRasterSupport = OpenGL::s_extension[Extension::NV_conservative_raster].m_supported;

            m_imageLoadStoreSupport = false
                || OpenGL::s_extension[Extension::ARB_shader_image_load_store].m_supported
                || OpenGL::s_extension[Extension::EXT_shader_image_load_store].m_supported
                ;

            g_caps.supported |= 0
                | (m_atocSupport ? BGFX_CAPS_ALPHA_TO_COVERAGE : 0)
                | (m_conservativeRasterSupport ? BGFX_CAPS_CONSERVATIVE_RASTER : 0)
                | (m_occlusionQuerySupport ? BGFX_CAPS_OCCLUSION_QUERY : 0)
                | (m_depthTextureSupport ? BGFX_CAPS_TEXTURE_COMPARE_LEQUAL : 0)
                | (computeSupport ? BGFX_CAPS_COMPUTE : 0)
                | (m_imageLoadStoreSupport ? BGFX_CAPS_IMAGE_RW : 0)
                ;

            g_caps.supported |= m_glctx.getCaps();

            if (Platform::IsMobilePlatform())
            {
                m_srgbWriteControlSupport = OpenGL::s_extension[Extension::EXT_sRGB_write_control].m_supported;

                m_borderColorSupport = OpenGL::s_extension[Extension::NV_texture_border_clamp].m_supported;
                OpenGL::s_textureAddress[BGFX_SAMPLER_U_BORDER >> BGFX_SAMPLER_U_SHIFT] = OpenGL::s_extension[Extension::NV_texture_border_clamp].m_supported
                    ? GL_CLAMP_TO_BORDER
                    : GL_CLAMP_TO_EDGE
                    ;
            }

            if (OpenGL::s_extension[Extension::EXT_texture_filter_anisotropic].m_supported)
            {
                GL_CHECK(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &m_maxAnisotropyDefault));
            }

            if (OpenGL::s_extension[Extension::ARB_texture_multisample].m_supported
                || OpenGL::s_extension[Extension::ANGLE_framebuffer_multisample].m_supported
                || OpenGL::s_extension[Extension::EXT_multisampled_render_to_texture].m_supported)
            {
                GL_CHECK(glGetIntegerv(GL_MAX_SAMPLES, &m_maxMsaa));
            }

			if (OpenGL::IsLowEndMobile())
			{
				if (!m_maxMsaa && OpenGL::s_extension[Extension::IMG_multisampled_render_to_texture].m_supported) {
					GL_CHECK(glGetIntegerv(GL_MAX_SAMPLES_IMG, &m_maxMsaa));
				}
			}

            if (OpenGL::s_extension[Extension::OES_read_format].m_supported
                && (OpenGL::s_extension[Extension::IMG_read_format].m_supported || OpenGL::s_extension[Extension::EXT_read_format_bgra].m_supported))
            {
                m_readPixelsFmt = GL_BGRA;
            }
            else
            {
                m_readPixelsFmt = GL_RGBA;
            }

            if (OpenGL::Support_GLES3_0())
            {
                g_caps.supported |= BGFX_CAPS_INSTANCING;
            }

            g_caps.supported |= OpenGL::s_extension[Extension::ARB_shader_viewport_layer_array].m_supported
                ? BGFX_CAPS_VIEWPORT_LAYER_ARRAY
                : 0
                ;

            if (OpenGL::s_extension[Extension::ARB_debug_output].m_supported
                || OpenGL::s_extension[Extension::KHR_debug].m_supported)
            {
                if (NULL != glDebugMessageControl
                    && NULL != glDebugMessageInsert
                    && NULL != glDebugMessageCallback
                    && NULL != glGetDebugMessageLog)
                {
                    GL_CHECK(glDebugMessageCallback(debugProcCb, NULL));
                    GL_CHECK(glDebugMessageControl(GL_DONT_CARE
                        , GL_DONT_CARE
                        , GL_DEBUG_SEVERITY_MEDIUM
                        , 0
                        , NULL
                        , GL_TRUE
                    ));
                }
            }

            if (OpenGL::s_extension[Extension::ARB_seamless_cube_map].m_supported)
            {
                GL_CHECK(glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS));
            }

            if (NULL != glProvokingVertex
                && OpenGL::s_extension[Extension::ARB_provoking_vertex].m_supported)
            {
                GL_CHECK(glProvokingVertex(GL_FIRST_VERTEX_CONVENTION));
            }

            m_maxLabelLen = OpenGL::Support_GLES3_2() || BX_ENABLED(BGFX_CONFIG_RENDERER_OPENGL >= 43) || OpenGL::s_extension[Extension::KHR_debug].m_supported ? uint16_t(glGet(GL_MAX_LABEL_LENGTH)) : 0;

            setGraphicsDebuggerPresent(OpenGL::s_extension[Extension::EXT_debug_tool].m_supported);

            if (NULL == glFramebufferTexture)
            {
                glFramebufferTexture = stubFramebufferTexture;
            }

            if (m_timerQuerySupport)
            {
                m_gpuTimer.create();
            }

            if (m_occlusionQuerySupport)
            {
                m_occlusionQuery.create();
            }

            // Init reserved part of view name.
            for (uint32_t ii = 0; ii < BGFX_CONFIG_MAX_VIEWS; ++ii)
            {
                bx::snprintf(s_viewName[ii], BGFX_CONFIG_MAX_VIEW_NAME_RESERVED + 1, "%3d   ", ii);
            }

            m_needPresent = false;
        }

        initializeCachedContext();

        return true;

    error:
        switch (errorState)
        {
        case ErrorState::Default:
            break;
        }

        m_glctx.destroy();

        unloadRenderDoc(m_renderdocdll);
        return false;
    }

    void RendererContextGL::shutdown()
    {
        if (m_vaoSupport)
        {
            GL_CHECK(glBindVertexArray(0));
            GL_CHECK(glDeleteVertexArrays(1, &m_vao));
            m_vao = 0;
        }

        clearCachedContext();

        captureFinish();

        invalidateCache();

        if (m_timerQuerySupport)
        {
            m_gpuTimer.destroy();
        }

        if (m_occlusionQuerySupport)
        {
            m_occlusionQuery.destroy();
        }

        destroyMsaaFbo();
        m_glctx.destroy();

        m_flip = false;

        unloadRenderDoc(m_renderdocdll);
    }

    RendererType::Enum RendererContextGL::getRendererType() const 
    {
        if (BX_ENABLED(BGFX_CONFIG_RENDERER_OPENGL))
        {
            return RendererType::OpenGL;
        }

        return RendererType::OpenGLES;
	}

	DeviceFeatureLevel::Enum RendererContextGL::getDeviceFeatureLevel() const
	{
		return OpenGL::GetCurrentFeatureLevel();
	}

    const char* RendererContextGL::getRendererName() const 
    {
        return BGFX_RENDERER_OPENGL_NAME;
    }

    bool RendererContextGL::isDeviceRemoved() 
    {
        return false;
    }

    void RendererContextGL::flip() 
    {
        if (m_flip)
        {
            for (uint32_t ii = 1, num = m_numWindows; ii < num; ++ii)
            {
                FrameBufferGL& frameBuffer = m_frameBuffers[m_windows[ii].idx];
                if (frameBuffer.m_needPresent)
                {
                    m_glctx.swap(frameBuffer.m_swapChain);
                    frameBuffer.m_needPresent = false;
                }
            }

            if (m_needPresent)
            {
                // Ensure the back buffer is bound as the source of the flip
                GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m_backBufferFbo));

                m_glctx.swap();
                m_needPresent = false;
            }
        }

        m_glctx.makeCurrent();
    }

    void RendererContextGL::createIndexBuffer(IndexBufferHandle _handle, const Memory* _mem, uint16_t _flags) 
    {
        m_indexBuffers[_handle.idx].create(_mem->size, _mem->data, _flags);
    }

    void RendererContextGL::destroyIndexBuffer(IndexBufferHandle _handle) 
    {
        m_indexBuffers[_handle.idx].destroy();
    }

    void RendererContextGL::createVertexLayout(VertexLayoutHandle _handle, const VertexLayout& _layout) 
    {
        VertexLayout& layout = m_vertexLayouts[_handle.idx];
        bx::memCopy(&layout, &_layout, sizeof(VertexLayout));
        dump(layout);
    }

    void RendererContextGL::destroyVertexLayout(VertexLayoutHandle /*_handle*/) 
    {
    }

    void RendererContextGL::createVertexBuffer(VertexBufferHandle _handle, const Memory* _mem, VertexLayoutHandle _layoutHandle, uint16_t _flags) 
    {
        m_vertexBuffers[_handle.idx].create(_mem->size, _mem->data, _layoutHandle, _flags);
    }

    void RendererContextGL::destroyVertexBuffer(VertexBufferHandle _handle) 
    {
        m_vertexBuffers[_handle.idx].destroy();
    }

    void RendererContextGL::createDynamicIndexBuffer(IndexBufferHandle _handle, uint32_t _size, uint16_t _flags) 
    {
        m_indexBuffers[_handle.idx].create(_size, NULL, _flags);
    }

    void RendererContextGL::updateDynamicIndexBuffer(IndexBufferHandle _handle, uint32_t _offset, uint32_t _size, const Memory* _mem) 
    {
        m_indexBuffers[_handle.idx].update(_offset, bx::uint32_min(_size, _mem->size), _mem->data);
    }

    void RendererContextGL::destroyDynamicIndexBuffer(IndexBufferHandle _handle) 
    {
        m_indexBuffers[_handle.idx].destroy();
    }

    void RendererContextGL::createDynamicVertexBuffer(VertexBufferHandle _handle, uint32_t _size, uint16_t _flags) 
    {
        VertexLayoutHandle layoutHandle = BGFX_INVALID_HANDLE;
        m_vertexBuffers[_handle.idx].create(_size, NULL, layoutHandle, _flags);
    }

    void RendererContextGL::updateDynamicVertexBuffer(VertexBufferHandle _handle, uint32_t _offset, uint32_t _size, const Memory* _mem) 
    {
        m_vertexBuffers[_handle.idx].update(_offset, bx::uint32_min(_size, _mem->size), _mem->data);
    }

    void RendererContextGL::destroyDynamicVertexBuffer(VertexBufferHandle _handle) 
    {
        m_vertexBuffers[_handle.idx].destroy();
    }

    void RendererContextGL::createShader(ShaderHandle _handle, const Memory* _mem) 
    {
        m_shaders[_handle.idx].create(_mem);
    }

    void RendererContextGL::destroyShader(ShaderHandle _handle) 
    {
        m_shaders[_handle.idx].destroy();
    }

    void RendererContextGL::createProgram(ProgramHandle _handle, ShaderHandle _vsh, ShaderHandle _fsh) 
    {
        ShaderGL dummyFragmentShader;
        m_program[_handle.idx].create(m_shaders[_vsh.idx], isValid(_fsh) ? m_shaders[_fsh.idx] : dummyFragmentShader);
    }

    void RendererContextGL::destroyProgram(ProgramHandle _handle) 
    {
        m_program[_handle.idx].destroy();
    }

    void* RendererContextGL::createTexture(TextureHandle _handle, const Memory* _mem, uint64_t _flags, uint8_t _skip) 
    {
        m_textures[_handle.idx].create(_mem, _flags, _skip);
        return NULL;
    }

    void RendererContextGL::updateTextureBegin(TextureHandle /*_handle*/, uint8_t /*_side*/, uint8_t /*_mip*/) 
    {
    }

    void RendererContextGL::updateTexture(TextureHandle _handle, uint8_t _side, uint8_t _mip, const Rect& _rect, uint16_t _z, uint16_t _depth, uint16_t _pitch, const Memory* _mem) 
    {
        m_textures[_handle.idx].update(_side, _mip, _rect, _z, _depth, _pitch, _mem);
    }

    void RendererContextGL::updateTextureEnd() 
    {
    }

    void RendererContextGL::readTexture(TextureHandle _handle, void* _data, uint8_t _mip) 
    {
        if (m_readBackSupported)
        {
            const TextureGL& texture = m_textures[_handle.idx];
            const bool compressed = bimg::isCompressed(bimg::TextureFormat::Enum(texture.m_textureFormat));

            GL_CHECK(glBindTexture(texture.m_target, texture.m_id));

            if (compressed)
            {
                GL_CHECK(glGetCompressedTexImage(texture.m_target
                    , _mip
                    , _data
                ));
            }
            else
            {
                GL_CHECK(glGetTexImage(texture.m_target
                    , _mip
                    , texture.m_fmt
                    , texture.m_type
                    , _data
                ));
            }

            GL_CHECK(glBindTexture(texture.m_target, 0));
        }
        else if (BX_ENABLED(BGFX_GL_CONFIG_TEXTURE_READ_BACK_EMULATION))
        {
            const TextureGL& texture = m_textures[_handle.idx];
            const bool compressed = bimg::isCompressed(bimg::TextureFormat::Enum(texture.m_textureFormat));

            if (!compressed)
            {
                Attachment at[1];
                at[0].init(_handle);

                FrameBufferGL frameBuffer;
                frameBuffer.create(BX_COUNTOF(at), at);
                GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.m_fbo[0]));
                GL_CHECK(glFramebufferTexture2D(
                    GL_FRAMEBUFFER
                    , GL_COLOR_ATTACHMENT0
                    , GL_TEXTURE_2D
                    , texture.m_id
                    , at[0].mip
                ));

                if (BX_ENABLED(BGFX_CONFIG_RENDERER_OPENGL) || OpenGL::Support_GLES3_0())
                {
                    GL_CHECK(glReadBuffer(GL_COLOR_ATTACHMENT0));
                }

                if (GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER))
                {
                    GL_CHECK(glReadPixels(
                        0
                        , 0
                        , texture.m_width
                        , texture.m_height
                        , m_readPixelsFmt
                        , GL_UNSIGNED_BYTE
                        , _data
                    ));
                }

                frameBuffer.destroy();
            }
        }
    }

    void RendererContextGL::resizeTexture(TextureHandle _handle, uint16_t _width, uint16_t _height, uint8_t _numMips, uint16_t _numLayers, bool autoGenMipmap) 
    {
        TextureGL& texture = m_textures[_handle.idx];

        uint32_t size = sizeof(uint32_t) + sizeof(TextureCreate);
        const Memory* mem = alloc(size);

        bx::StaticMemoryBlockWriter writer(mem->data, mem->size);
        uint32_t magic = BGFX_CHUNK_MAGIC_TEX;
        bx::write(&writer, magic, bx::ErrorAssert{});

        TextureCreate tc;
        tc.m_width = _width;
        tc.m_height = _height;
        tc.m_depth = 0;
        tc.m_numLayers = _numLayers;
        tc.m_numMips = _numMips;
        tc.m_format = TextureFormat::Enum(texture.m_requestedFormat);
        tc.m_cubeMap = false;
        tc.m_mem = NULL;
        tc.m_autoGenMipmap = autoGenMipmap;
        bx::write(&writer, tc, bx::ErrorAssert{});

        texture.destroy();
        texture.create(mem, texture.m_flags, 0);

        release(mem);
    }

    void RendererContextGL::overrideInternal(TextureHandle _handle, uintptr_t _ptr) 
    {
        m_textures[_handle.idx].overrideInternal(_ptr);
    }

    uintptr_t RendererContextGL::getInternal(TextureHandle _handle) 
    {
        return uintptr_t(m_textures[_handle.idx].m_id);
    }

    void RendererContextGL::destroyTexture(TextureHandle _handle) 
    {
        m_textures[_handle.idx].destroy();
    }

    void RendererContextGL::createFrameBuffer(FrameBufferHandle _handle, uint8_t _num, const Attachment* _attachment) 
    {
        m_frameBuffers[_handle.idx].create(_num, _attachment);
    }

    void RendererContextGL::createFrameBuffer(FrameBufferHandle _handle, void* _nwh, uint32_t _width, uint32_t _height, TextureFormat::Enum _format, TextureFormat::Enum _depthFormat) 
    {
        uint16_t denseIdx = m_numWindows++;
        m_windows[denseIdx] = _handle;
        m_frameBuffers[_handle.idx].create(denseIdx, _nwh, _width, _height, _format, _depthFormat);
    }

    void RendererContextGL::createFrameBuffer(FrameBufferHandle _handle, int32_t refHandle, uint32_t _width, uint32_t _height, TextureFormat::Enum _format, TextureFormat::Enum _depthFormat) 
    {
        m_frameBuffers[_handle.idx].create(refHandle, _width, _height, _format, _depthFormat);
    }

    void RendererContextGL::destroyFrameBuffer(FrameBufferHandle _handle) 
    {
        uint16_t denseIdx = m_frameBuffers[_handle.idx].destroy();
        if (UINT16_MAX != denseIdx)
        {
            --m_numWindows;
            if (m_numWindows > 1)
            {
                FrameBufferHandle handle = m_windows[m_numWindows];
                m_windows[m_numWindows] = { kInvalidHandle };
                if (m_numWindows != denseIdx)
                {
                    m_windows[denseIdx] = handle;
                    m_frameBuffers[handle.idx].m_denseIdx = denseIdx;
                }
            }
        }
    }

    void RendererContextGL::createUniform(UniformHandle _handle, UniformType::Enum _type, uint16_t _num, const char* _name) 
    {
        if (NULL != m_uniforms[_handle.idx])
        {
            BX_FREE(g_allocator, m_uniforms[_handle.idx]);
        }

        uint32_t size = g_uniformTypeSize[_type] * _num;
        void* data = BX_ALLOC(g_allocator, size);
        bx::memSet(data, 0, size);
        m_uniforms[_handle.idx] = data;
        m_uniformReg.add(_handle, _name, _type);
    }

    void RendererContextGL::destroyUniform(UniformHandle _handle) 
    {
        BX_FREE(g_allocator, m_uniforms[_handle.idx]);
        m_uniforms[_handle.idx] = NULL;
        m_uniformReg.remove(_handle);
    }

    void RendererContextGL::requestScreenShot(FrameBufferHandle _handle, const char* _filePath) 
    {
        SwapChainGL* swapChain = NULL;
        uint32_t width = m_resolution.width;
        uint32_t height = m_resolution.height;

        if (isValid(_handle))
        {
            const FrameBufferGL& frameBuffer = m_frameBuffers[_handle.idx];
            swapChain = frameBuffer.m_swapChain;
            width = frameBuffer.m_width;
            height = frameBuffer.m_height;
        }

        m_glctx.makeCurrent(swapChain);

        uint32_t length = width * height * 4;
        uint8_t* data = (uint8_t*)BX_ALLOC(g_allocator, length);

        GL_CHECK(glReadPixels(0
            , 0
            , width
            , height
            , m_readPixelsFmt
            , GL_UNSIGNED_BYTE
            , data
        ));

        if (GL_RGBA == m_readPixelsFmt)
        {
            bimg::imageSwizzleBgra8(data, width * 4, width, height, data, width * 4);
        }

        g_callback->screenShot(_filePath
            , width
            , height
            , width * 4
            , data
            , length
            , true
        );
        BX_FREE(g_allocator, data);
    }

    void RendererContextGL::updateViewName(ViewId _id, const char* _name) 
    {
        bx::strCopy(&s_viewName[_id][BGFX_CONFIG_MAX_VIEW_NAME_RESERVED]
            , BX_COUNTOF(s_viewName[0]) - BGFX_CONFIG_MAX_VIEW_NAME_RESERVED
            , _name
        );
    }

    void RendererContextGL::updateUniform(uint16_t _loc, const void* _data, uint32_t _size) 
    {
        bx::memCopy(m_uniforms[_loc], _data, _size);
    }

    void RendererContextGL::invalidateOcclusionQuery(OcclusionQueryHandle _handle) 
    {
        m_occlusionQuery.invalidate(_handle);
    }

    void RendererContextGL::setMarker(const char* _marker, uint16_t _len) 
    {
        GL_CHECK(OpenGL::InsertEventMarker(_len, _marker));
    }

    void RendererContextGL::setName(Handle _handle, const char* _name, uint16_t _len) 
    {
        uint16_t len = bx::min(_len, m_maxLabelLen);

        switch (_handle.type)
        {
        case Handle::IndexBuffer:
            GL_CHECK(OpenGL::ObjectLabel(GL_BUFFER, m_indexBuffers[_handle.idx].m_id, len, _name));
            break;

        case Handle::Shader:
            GL_CHECK(OpenGL::ObjectLabel(GL_SHADER, m_shaders[_handle.idx].m_id, len, _name));
            break;

        case Handle::Texture:
        {
            GLint id = m_textures[_handle.idx].m_id;
            if (0 != id)
            {
                GL_CHECK(OpenGL::ObjectLabel(GL_TEXTURE, id, len, _name));
            }
            else
            {
                GL_CHECK(OpenGL::ObjectLabel(GL_RENDERBUFFER, m_textures[_handle.idx].m_rbo, len, _name));
            }
        }
        break;

        case Handle::VertexBuffer:
            GL_CHECK(OpenGL::ObjectLabel(GL_BUFFER, m_vertexBuffers[_handle.idx].m_id, len, _name));
            break;

        default:
            BX_ASSERT(false, "Invalid handle type?! %d", _handle.type);
            break;
        }
    }

    void RendererContextGL::blitSetup(TextVideoMemBlitter& _blitter) 
    {
        
    }

    void RendererContextGL::blitRender(TextVideoMemBlitter& _blitter, uint32_t _numIndices) 
    {
        const uint32_t numVertices = _numIndices * 4 / 6;
        if (0 < numVertices)
        {
            m_indexBuffers[_blitter.m_ib->handle.idx].update(0, _numIndices * 2, _blitter.m_ib->data);
            m_vertexBuffers[_blitter.m_vb->handle.idx].update(0, numVertices * _blitter.m_layout.m_stride, _blitter.m_vb->data);

            VertexBufferGL& vb = m_vertexBuffers[_blitter.m_vb->handle.idx];
            GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vb.m_id));

            IndexBufferGL& ib = m_indexBuffers[_blitter.m_ib->handle.idx];
            GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib.m_id));

            ProgramGL& program = m_program[_blitter.m_program.idx];
            program.bindAttributesBegin();
            program.bindAttributes(_blitter.m_layout, 0);
            program.bindAttributesEnd();

            GL_CHECK(glDrawElements(GL_TRIANGLES
                , _numIndices
                , GL_UNSIGNED_SHORT
                , (void*)0
            ));
        }
    }

    void RendererContextGL::updateResolution(const Resolution& _resolution)
    {
        m_maxAnisotropy = !!(_resolution.reset & BGFX_RESET_MAXANISOTROPY)
            ? m_maxAnisotropyDefault
            : 0.0f
            ;

        if (OpenGL::s_extension[Extension::ARB_depth_clamp].m_supported)
        {
            if (!!(_resolution.reset & BGFX_RESET_DEPTH_CLAMP))
            {
                GL_CHECK(glEnable(GL_DEPTH_CLAMP));
            }
            else
            {
                GL_CHECK(glDisable(GL_DEPTH_CLAMP));
            }
        }

        const uint32_t maskFlags = ~(0
            | BGFX_RESET_MAXANISOTROPY
            | BGFX_RESET_DEPTH_CLAMP
            | BGFX_RESET_SUSPEND
            );

        if (m_resolution.width != _resolution.width
            || m_resolution.height != _resolution.height
            || (m_resolution.reset & maskFlags) != (_resolution.reset & maskFlags))
        {
            uint32_t flags = _resolution.reset & (~BGFX_RESET_INTERNAL_FORCE);

            m_resolution = _resolution;
            m_resolution.reset = flags;

            m_textVideoMem.resize(false, _resolution.width, _resolution.height);
            m_textVideoMem.clear();

            setRenderContextSize(m_resolution.width
                , m_resolution.height
                , flags
            );
            updateCapture();

            for (uint32_t ii = 0; ii < BX_COUNTOF(m_frameBuffers); ++ii)
            {
                m_frameBuffers[ii].postReset();
            }

            m_currentFbo = 0;

            GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m_currentFbo));
        }
    }

    void RendererContextGL::setShaderUniform4f(uint8_t /*_flags*/, uint32_t _regIndex, const void* _val, uint32_t _numRegs)
    {
        setUniform4fv(_regIndex
            , _numRegs
            , (const GLfloat*)_val
        );
    }

    void RendererContextGL::setShaderUniform4x4f(uint8_t /*_flags*/, uint32_t _regIndex, const void* _val, uint32_t _numRegs)
    {
        setUniformMatrix4fv(_regIndex
            , _numRegs
            , GL_FALSE
            , (const GLfloat*)_val
        );
    }

    uint32_t RendererContextGL::setFrameBuffer(FrameBufferHandle _fbh, uint32_t _height, uint16_t _discard, bool _msaa)
    {
        if (isValid(m_fbh)
            && m_fbh.idx != _fbh.idx)
        {
            FrameBufferGL& frameBuffer = m_frameBuffers[m_fbh.idx];
            frameBuffer.resolve();

            if (BGFX_CLEAR_NONE != m_fbDiscard)
            {
                frameBuffer.discard(m_fbDiscard);
                m_fbDiscard = BGFX_CLEAR_NONE;
            }
        }

        m_glctx.makeCurrent(NULL);

        if (!isValid(_fbh))
        {
            m_needPresent |= true;

            m_currentFbo = m_msaaBackBufferFbo;

            if (m_srgbWriteControlSupport)
            {
                if (0 != (m_resolution.reset & BGFX_RESET_SRGB_BACKBUFFER))
                {
                    GL_CHECK(glEnable(GL_FRAMEBUFFER_SRGB));
                }
                else
                {
                    GL_CHECK(glDisable(GL_FRAMEBUFFER_SRGB));
                }
            }
        }
        else
        {
            FrameBufferGL& frameBuffer = m_frameBuffers[_fbh.idx];
            _height = frameBuffer.m_height;
            if (UINT16_MAX != frameBuffer.m_denseIdx)
            {
                m_glctx.makeCurrent(frameBuffer.m_swapChain);
                GL_CHECK(glFrontFace(GL_CW));

                frameBuffer.m_needPresent = true;
                m_currentFbo = 0;
            }
            else
            {
                m_glctx.makeCurrent(NULL);
                m_currentFbo = frameBuffer.m_fbo[0];
            }
        }

        GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m_currentFbo));

        m_fbh = _fbh;
        m_fbDiscard = _discard;
        m_rtMsaa = _msaa;

        return _height;
    }

    uint32_t RendererContextGL::getNumRt() const
    {
        if (isValid(m_fbh))
        {
            const FrameBufferGL& frameBuffer = m_frameBuffers[m_fbh.idx];
            return frameBuffer.m_num;
        }

        return 1;
    }

    void RendererContextGL::createMsaaFbo(uint32_t _width, uint32_t _height, uint32_t _msaa)
    {
        if (0 == m_msaaBackBufferFbo // iOS
            && 1 < _msaa)
        {
            GLenum storageFormat = (m_resolution.reset & BGFX_RESET_SRGB_BACKBUFFER)
                ? GL_SRGB8_ALPHA8
                : GL_RGBA8
                ;
            GLenum attachment = BX_ENABLED(BGFX_CONFIG_RENDERER_OPENGL) || OpenGL::Support_GLES3_0()
                ? GL_DEPTH_STENCIL_ATTACHMENT
                : GL_DEPTH_ATTACHMENT;

            GL_CHECK(glGenFramebuffers(1, &m_msaaBackBufferFbo));
            GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m_msaaBackBufferFbo));
            if (OpenGL::Support_GLES3_0())
            {
                GL_CHECK(glGenTextures(BX_COUNTOF(m_msaaBackBufferTextures), m_msaaBackBufferTextures));
                GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_msaaBackBufferTextures[0]));
                GL_CHECK(glTexStorage2D(GL_TEXTURE_2D, 1, storageFormat, _width, _height));
                GL_CHECK(glFramebufferTexture2DMultisampleEXT(GL_FRAMEBUFFER,
                    GL_COLOR_ATTACHMENT0,
                    GL_TEXTURE_2D,
                    m_msaaBackBufferTextures[0], 0,
                    _msaa));
                GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m_msaaBackBufferFbo));

                GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_msaaBackBufferTextures[1]));
                GL_CHECK(glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, _width, _height));
                GL_CHECK(glFramebufferTexture2DMultisampleEXT(GL_FRAMEBUFFER,
                    attachment,
                    GL_TEXTURE_2D,
                    m_msaaBackBufferTextures[1], 0,
                    _msaa));
                GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));

                BX_ASSERT(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER),
                    "glCheckFramebufferStatus failed 0x%08x",
                    glCheckFramebufferStatus(GL_FRAMEBUFFER)
                );

                if (0 == m_msaaBlitProgram) {
                    static const char msaa_blit_vs[]{ R"(#version 300 es
                        precision highp float;
                        out vec2 UV;
                        void main() {
                            float x = -1.0 + float((gl_VertexID & 1) << 2);
                            float y = -1.0 + float((gl_VertexID & 2) << 1);
                            gl_Position = vec4(x, y, 0, 1);
                            UV = vec2(gl_Position.x + 1.0, gl_Position.y + 1.0) * 0.5;
                        }
                    )" };

                    static const char msaa_blit_fs[]{ R"(#version 300 es
                        precision mediump float;
                        in vec2 UV;
                        uniform sampler2D msaaTexture;
                        out vec4 oFragColor;
                        void main() {
                            oFragColor = texture(msaaTexture, UV);
                        }
                    )" };

                    const GLchar* const vs = msaa_blit_vs;
                    const GLchar* const fs = msaa_blit_fs;
                    GLuint shader_vs = glCreateShader(GL_VERTEX_SHADER);
                    BX_WARN(0 != shader_vs, "Failed to create msaa Blit Vertex shader.");
                    GL_CHECK(glShaderSource(shader_vs, 1, &vs, nullptr));
                    GL_CHECK(glCompileShader(shader_vs));
                    GLint compiled = 0;
                    GL_CHECK(glGetShaderiv(shader_vs, GL_COMPILE_STATUS, &compiled));
                    BX_WARN(0 == shader_vs, "Unable to compile msaa Blit Vertex shader.");

                    GLuint shader_fs = glCreateShader(GL_FRAGMENT_SHADER);
                    BX_WARN(0 != shader_fs, "Failed to create msaa Blit Fragment shader.");
                    GL_CHECK(glShaderSource(shader_fs, 1, &fs, nullptr));
                    GL_CHECK(glCompileShader(shader_fs));
                    compiled = 0;
                    GL_CHECK(glGetShaderiv(shader_fs, GL_COMPILE_STATUS, &compiled));
                    BX_WARN(0 == shader_vs, "Unable to compile msaa Blit Fragment shader.");

                    m_msaaBlitProgram = glCreateProgram();
                    if (m_msaaBlitProgram)
                    {
                        GL_CHECK(glAttachShader(m_msaaBlitProgram, shader_vs));
                        GL_CHECK(glAttachShader(m_msaaBlitProgram, shader_fs));

                        GL_CHECK(glLinkProgram(m_msaaBlitProgram));
                        GLint linked = 0;
                        glGetProgramiv(m_msaaBlitProgram, GL_LINK_STATUS, &linked);
                        if (0 == linked) {
                            char log[1024];
                            GL_CHECK(glGetProgramInfoLog(m_msaaBlitProgram, sizeof(log), NULL,
                                log));
                            BX_TRACE("%d: %s", linked, log);
                        }

                        GL_CHECK(glDetachShader(m_msaaBlitProgram, shader_vs));
                        GL_CHECK(glDeleteShader(shader_vs));
                        GL_CHECK(glDetachShader(m_msaaBlitProgram, shader_fs));
                        GL_CHECK(glDeleteShader(shader_fs));
                    }
                }
            }
            else
            {
                GL_CHECK(glGenRenderbuffers(BX_COUNTOF(m_msaaBackBufferRbos), m_msaaBackBufferRbos));
                GL_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, m_msaaBackBufferRbos[0]));
                GL_CHECK(glRenderbufferStorageMultisample(GL_RENDERBUFFER, _msaa, storageFormat, _width, _height));
                GL_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, m_msaaBackBufferRbos[1]));
                GL_CHECK(glRenderbufferStorageMultisample(GL_RENDERBUFFER, _msaa, GL_DEPTH24_STENCIL8, _width, _height));
                GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_msaaBackBufferRbos[0]));

                GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, m_msaaBackBufferRbos[1]));

                BX_ASSERT(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER)
                    , "glCheckFramebufferStatus failed 0x%08x"
                    , glCheckFramebufferStatus(GL_FRAMEBUFFER)
                );
            }
        }
    }

    void RendererContextGL::destroyMsaaFbo()
    {
        if (m_backBufferFbo != m_msaaBackBufferFbo // iOS
            && 0 != m_msaaBackBufferFbo)
        {
            GL_CHECK(glDeleteFramebuffers(1, &m_msaaBackBufferFbo));
            m_msaaBackBufferFbo = 0;

            if (OpenGL::Support_GLES3_0())
            {
                if (0 != m_msaaBackBufferTextures[0])
                {
                    GL_CHECK(glDeleteTextures(BX_COUNTOF(m_msaaBackBufferTextures), m_msaaBackBufferTextures));
                    m_msaaBackBufferTextures[0] = 0;
                    m_msaaBackBufferTextures[1] = 0;
                }
                if (0 != m_msaaBlitProgram)
                {
                    GL_CHECK(glDeleteProgram(m_msaaBlitProgram));
                    m_msaaBlitProgram = 0;
                }
            }
            else
            {
                if (0 != m_msaaBackBufferRbos[0])
                {
                    GL_CHECK(glDeleteRenderbuffers(BX_COUNTOF(m_msaaBackBufferRbos), m_msaaBackBufferRbos));
                    m_msaaBackBufferRbos[0] = 0;
                    m_msaaBackBufferRbos[1] = 0;
                }
            }
        }
    }

    void RendererContextGL::blitMsaaFbo()
    {
        if (m_backBufferFbo != m_msaaBackBufferFbo // iOS
            && 0 != m_msaaBackBufferFbo)
        {
            GL_CHECK(glDisable(GL_SCISSOR_TEST));
            GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m_backBufferFbo));
            GL_CHECK(glBindFramebuffer(GL_READ_FRAMEBUFFER, m_msaaBackBufferFbo));
            GL_CHECK(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
            uint32_t width = m_resolution.width;
            uint32_t height = m_resolution.height;
            GLenum filter = BX_ENABLED(BGFX_CONFIG_RENDERER_OPENGL) || !OpenGL::Support_GLES3_0()
                ? GL_NEAREST
                : GL_LINEAR
                ;
            if (OpenGL::Support_GLES3_0())
            {
                GL_CHECK(glUseProgram(m_msaaBlitProgram));
                GL_CHECK(glActiveTexture(GL_TEXTURE0));
                GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_msaaBackBufferTextures[0]));
                GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 3));
            }
            else
            {
                GL_CHECK(glBlitFramebuffer(0
                    , 0
                    , width
                    , height
                    , 0
                    , 0
                    , width
                    , height
                    , GL_COLOR_BUFFER_BIT
                    , filter
                ));
            }

            GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m_backBufferFbo));
        }
    }

    void RendererContextGL::setRenderContextSize(uint32_t _width, uint32_t _height, uint32_t _flags)
    {
        if (_width != 0
            || _height != 0)
        {
            if (!m_glctx.isValid())
            {
                m_glctx.create(_width, _height);

#if BX_PLATFORM_IOS
                // iOS: need to figure out how to deal with FBO created by context.
                m_backBufferFbo = m_msaaBackBufferFbo = m_glctx.getFbo();
#endif // BX_PLATFORM_IOS
            }
            else
            {
                destroyMsaaFbo();

                m_glctx.resize(_width, _height, _flags);

                uint32_t msaa = (_flags & BGFX_RESET_MSAA_MASK) >> BGFX_RESET_MSAA_SHIFT;
                msaa = bx::uint32_min(m_maxMsaa, msaa == 0 ? 0 : 1 << msaa);

                createMsaaFbo(_width, _height, msaa);
            }
        }

        m_flip = true;
    }

    void RendererContextGL::invalidateCache()
    {
        if ((BX_ENABLED(BGFX_CONFIG_RENDERER_OPENGL) || OpenGL::Support_GLES3_0())
            && m_samplerObjectSupport)
        {
            m_samplerStateCache.invalidate();
        }
    }

    void RendererContextGL::setSamplerState(uint32_t _stage, uint32_t _numMips, uint32_t _flags, const float _rgba[4])
    {
        if ((BX_ENABLED(BGFX_CONFIG_RENDERER_OPENGL) || OpenGL::Support_GLES3_0())
            && m_samplerObjectSupport)
        {
            if (0 == (BGFX_SAMPLER_INTERNAL_DEFAULT & _flags))
            {
                const uint32_t index = (_flags & BGFX_SAMPLER_BORDER_COLOR_MASK) >> BGFX_SAMPLER_BORDER_COLOR_SHIFT;

                _flags &= ~BGFX_SAMPLER_RESERVED_MASK;
                _flags &= BGFX_SAMPLER_BITS_MASK;
                _flags |= _numMips << BGFX_SAMPLER_RESERVED_SHIFT;

                GLuint sampler;

                bool hasBorderColor = false;
                bx::HashMurmur2A murmur;
                uint32_t hash;

                murmur.begin();
                murmur.add(_flags);
                if (!needBorderColor(_flags))
                {
                    murmur.add(-1);
                    hash = murmur.end();

                    sampler = m_samplerStateCache.find(hash);
                }
                else
                {
                    murmur.add(index);
                    hash = murmur.end();

                    if (NULL != _rgba)
                    {
                        hasBorderColor = true;
                        sampler = UINT32_MAX;
                    }
                    else
                    {
                        sampler = m_samplerStateCache.find(hash);
                    }
                }

                if (UINT32_MAX == sampler)
                {
                    sampler = m_samplerStateCache.add(hash);

                    GL_CHECK(glSamplerParameteri(sampler
                        , GL_TEXTURE_WRAP_S
                        , OpenGL::s_textureAddress[(_flags & BGFX_SAMPLER_U_MASK) >> BGFX_SAMPLER_U_SHIFT]
                    ));
                    GL_CHECK(glSamplerParameteri(sampler
                        , GL_TEXTURE_WRAP_T
                        , OpenGL::s_textureAddress[(_flags & BGFX_SAMPLER_V_MASK) >> BGFX_SAMPLER_V_SHIFT]
                    ));
                    GL_CHECK(glSamplerParameteri(sampler
                        , GL_TEXTURE_WRAP_R
                        , OpenGL::s_textureAddress[(_flags & BGFX_SAMPLER_W_MASK) >> BGFX_SAMPLER_W_SHIFT]
                    ));

                    GLenum minFilter;
                    GLenum magFilter;
					OpenGL::getFilters(_flags, 1 < _numMips, magFilter, minFilter);
                    GL_CHECK(glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, magFilter));
                    GL_CHECK(glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, minFilter));

                    if (BX_ENABLED(BGFX_CONFIG_RENDERER_OPENGL))
                    {
                        GL_CHECK(glSamplerParameterf(sampler, GL_TEXTURE_LOD_BIAS, float(BGFX_CONFIG_MIP_LOD_BIAS)));
                    }

                    if (m_borderColorSupport
                        && hasBorderColor)
                    {
                        GL_CHECK(glSamplerParameterfv(sampler, GL_TEXTURE_BORDER_COLOR, _rgba));
                    }

                    if (0 != (_flags & (BGFX_SAMPLER_MIN_ANISOTROPIC | BGFX_SAMPLER_MAG_ANISOTROPIC))
                        && 0.0f < m_maxAnisotropy)
                    {
                        GL_CHECK(glSamplerParameterf(sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, m_maxAnisotropy));
                    }

                    if (OpenGL::Support_GLES3_0()
                        || m_shadowSamplersSupport)
                    {
                        const uint32_t cmpFunc = (_flags & BGFX_SAMPLER_COMPARE_MASK) >> BGFX_SAMPLER_COMPARE_SHIFT;
                        if (0 == cmpFunc)
                        {
                            GL_CHECK(glSamplerParameteri(sampler, GL_TEXTURE_COMPARE_MODE, GL_NONE));
                        }
                        else
                        {
                            GL_CHECK(glSamplerParameteri(sampler, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE));
                            GL_CHECK(glSamplerParameteri(sampler, GL_TEXTURE_COMPARE_FUNC, OpenGL::s_cmpFunc[cmpFunc]));
                        }
                    }
                }

                GL_CHECK(glBindSampler(_stage, sampler));
            }
            else
            {
                GL_CHECK(glBindSampler(_stage, 0));
            }
        }
    }

    bool RendererContextGL::isVisible(Frame* _render, OcclusionQueryHandle _handle, bool _visible)
    {
        m_occlusionQuery.resolve(_render);
        return _visible == (0 != _render->m_occlusion[_handle.idx]);
    }

    void RendererContextGL::updateCapture()
    {
        if (m_resolution.reset & BGFX_RESET_CAPTURE)
        {
            m_captureSize = m_resolution.width * m_resolution.height * 4;
            m_capture = BX_REALLOC(g_allocator, m_capture, m_captureSize);
            g_callback->captureBegin(m_resolution.width, m_resolution.height, m_resolution.width * 4, TextureFormat::BGRA8, true);
        }
        else
        {
            captureFinish();
        }
    }

    void RendererContextGL::capture()
    {
        if (NULL != m_capture)
        {
            GL_CHECK(glReadPixels(0
                , 0
                , m_resolution.width
                , m_resolution.height
                , m_readPixelsFmt
                , GL_UNSIGNED_BYTE
                , m_capture
            ));

            if (GL_RGBA == m_readPixelsFmt)
            {
                bimg::imageSwizzleBgra8(
                    m_capture
                    , m_resolution.width * 4
                    , m_resolution.width
                    , m_resolution.height
                    , m_capture
                    , m_resolution.width * 4
                );
            }

            g_callback->captureFrame(m_capture, m_captureSize);
        }
    }

    void RendererContextGL::captureFinish()
    {
        if (NULL != m_capture)
        {
            g_callback->captureEnd();
            BX_FREE(g_allocator, m_capture);
            m_capture = NULL;
            m_captureSize = 0;
        }
    }

    bool RendererContextGL::programFetchFromCache(GLuint programId, uint64_t _id)
    {
        _id ^= m_hash;

        bool cached = false;

        if (m_programBinarySupport)
        {
            uint32_t length = g_callback->cacheReadSize(_id);
            cached = length > 0;

            if (cached)
            {
                void* data = BX_ALLOC(g_allocator, length);
                if (g_callback->cacheRead(_id, data, length))
                {
                    bx::Error err;
                    bx::MemoryReader reader(data, length);

                    GLenum format;
                    bx::read(&reader, format, &err);

                    GL_CHECK(glProgramBinary(programId, format, reader.getDataPtr(), (GLsizei)reader.remaining()));
                }

                BX_FREE(g_allocator, data);
            }

#if BGFX_CONFIG_RENDERER_OPENGL
            GL_CHECK(glProgramParameteri(programId, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE));
#endif // BGFX_CONFIG_RENDERER_OPENGL
        }

        return cached;
    }

    void RendererContextGL::programCache(GLuint programId, uint64_t _id)
    {
        _id ^= m_hash;

        if (m_programBinarySupport)
        {
            GLint programLength;
            GLenum format;
            GL_CHECK(glGetProgramiv(programId, GL_PROGRAM_BINARY_LENGTH, &programLength));

            if (0 < programLength)
            {
                uint32_t length = programLength + 4;
                uint8_t* data = (uint8_t*)BX_ALLOC(g_allocator, length);
                GL_CHECK(glGetProgramBinary(programId, programLength, NULL, &format, &data[4]));
                *(uint32_t*)data = format;

                g_callback->cacheWrite(_id, data, length);

                BX_FREE(g_allocator, data);
            }
        }
    }

    void RendererContextGL::commit(UniformBuffer& _uniformBuffer)
    {
        _uniformBuffer.reset();

        for (;;)
        {
            uint32_t opcode = _uniformBuffer.read();

            if (UniformType::End == opcode)
            {
                break;
            }

            UniformType::Enum type;
            uint16_t ignore;
            uint16_t num;
            uint16_t copy;
            UniformBuffer::decodeOpcode(opcode, type, ignore, num, copy);

            const char* data;
            if (copy)
            {
                data = _uniformBuffer.read(g_uniformTypeSize[type] * num);
            }
            else
            {
                UniformHandle handle;
                bx::memCopy(&handle, _uniformBuffer.read(sizeof(UniformHandle)), sizeof(UniformHandle));
                data = (const char*)m_uniforms[handle.idx];
            }

            uint32_t loc = _uniformBuffer.read();

            switch (type)
            {
#if BX_PLATFORM_EMSCRIPTEN
                // For WebAssembly the array forms glUniform1iv/glUniform4fv are much slower compared to glUniform1i/glUniform4f
                // since they need to marshal an array over from Wasm to JS, so optimize the case when there is exactly one
                // uniform to upload.
            case UniformType::Sampler:
                if (num > 1)
                {
                    setUniform1iv(loc, num, (int32_t*)data);
                }
                else
                {
                    setUniform1i(loc, *(int32_t*)data);
                }
                break;
            case UniformType::Vec4:
                if (num > 1)
                {
                    setUniform4fv(loc, num, (float*)data);
                }
                else
                {
                    float* vec4 = (float*)data;
                    setUniform4f(loc, vec4[0], vec4[1], vec4[2], vec4[3]);
                }
                break;
#else
            case UniformType::Sampler:
                setUniform1iv(loc, num, (int32_t*)data);
                break;

            case UniformType::Float:
                setUniform1fv(loc, num, (float*)data);
                break;

            case UniformType::Vec2:
                setUniform2fv(loc, num, (float*)data);
                break;

            case UniformType::Vec3:
                setUniform3fv(loc, num, (float*)data);
                break;

            case UniformType::Vec4:
                setUniform4fv(loc, num, (float*)data);
                break;
#endif
            case UniformType::Mat3:
                setUniformMatrix3fv(loc, num, GL_FALSE, (float*)data);
                break;

            case UniformType::Mat4:
                setUniformMatrix4fv(loc, num, GL_FALSE, (float*)data);
                break;

            case UniformType::End:
                break;

            default:
                BX_TRACE("%4d: INVALID 0x%08x, t %d, l %d, n %d, c %d", _uniformBuffer.getPos(), opcode, type, loc, num, copy);
                break;
            }
        }
    }

    void RendererContextGL::clearQuad(const Rect& _rect, const Clear& _clear, uint32_t _height, const float _palette[][4])
    {
        uint32_t numMrt = 1;
        FrameBufferHandle fbh = m_fbh;
        if (isValid(fbh))
        {
            const FrameBufferGL& fb = m_frameBuffers[fbh.idx];
            numMrt = bx::uint32_max(1, fb.m_num);
        }

        if (1 == numMrt)
        {
            GLuint flags = 0;
            if (BGFX_CLEAR_COLOR & _clear.m_flags)
            {
                if (BGFX_CLEAR_COLOR_USE_PALETTE & _clear.m_flags)
                {
                    uint8_t index = (uint8_t)bx::uint32_min(BGFX_CONFIG_MAX_COLOR_PALETTE - 1, _clear.m_index[0]);
                    const float* rgba = _palette[index];
                    const float rr = rgba[0];
                    const float gg = rgba[1];
                    const float bb = rgba[2];
                    const float aa = rgba[3];
                    GL_CHECK(glClearColor(rr, gg, bb, aa));
                }
                else
                {
                    float rr = _clear.m_index[0] * 1.0f / 255.0f;
                    float gg = _clear.m_index[1] * 1.0f / 255.0f;
                    float bb = _clear.m_index[2] * 1.0f / 255.0f;
                    float aa = _clear.m_index[3] * 1.0f / 255.0f;
                    GL_CHECK(glClearColor(rr, gg, bb, aa));
                }

                flags |= GL_COLOR_BUFFER_BIT;
                GL_CHECK(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
            }

            if (BGFX_CLEAR_DEPTH & _clear.m_flags)
            {
                flags |= GL_DEPTH_BUFFER_BIT;
                GL_CHECK(glClearDepth(_clear.m_depth));
                GL_CHECK(glDepthMask(GL_TRUE));
            }

            if (BGFX_CLEAR_STENCIL & _clear.m_flags)
            {
                flags |= GL_STENCIL_BUFFER_BIT;
                GL_CHECK(glClearStencil(_clear.m_stencil));
            }

            if (0 != flags)
            {
                GL_CHECK(glEnable(GL_SCISSOR_TEST));
                GL_CHECK(glScissor(_rect.m_x, _height - _rect.m_height - _rect.m_y, _rect.m_width, _rect.m_height));
                GL_CHECK(glClear(flags));
                GL_CHECK(glDisable(GL_SCISSOR_TEST));
            }
        }
    }

    void RendererContextGL::setProgram(GLuint program)
    {
        m_uniformStateCache.saveCurrentProgram(program);
        GL_CHECK(glUseProgram(program));
    }

    // Cache uniform uploads to avoid redundant uploading of state that is
    // already set to a shader program
    void RendererContextGL::setUniform1i(uint32_t loc, int value)
    {
        if (m_uniformStateCache.updateUniformCache(loc, value))
        {
            GL_CHECK(glUniform1i(loc, value));
        }
    }

    void RendererContextGL::setUniform1iv(uint32_t loc, int num, const int* data)
    {
        bool changed = false;
        for (int i = 0; i < num; ++i)
        {
            if (m_uniformStateCache.updateUniformCache(loc + i, data[i]))
            {
                changed = true;
            }
        }
        if (changed)
        {
            GL_CHECK(glUniform1iv(loc, num, data));
        }
    }

    void RendererContextGL::setUniform4f(uint32_t loc, float x, float y, float z, float w)
    {
        UniformStateCache::f4 f; f.val[0] = x; f.val[1] = y; f.val[2] = z; f.val[3] = w;
        if (m_uniformStateCache.updateUniformCache(loc, f))
        {
            GL_CHECK(glUniform4f(loc, x, y, z, w));
        }
    }

    void RendererContextGL::setUniform1fv(uint32_t loc, int num, const float* data)
    {
        bool changed = false;
        for (int i = 0; i < num; ++i)
        {
            if (m_uniformStateCache.updateUniformCache(loc + i, *(const UniformStateCache::f1*)&data[i]))
            {
                changed = true;
            }
        }
        if (changed)
        {
            GL_CHECK(glUniform1fv(loc, num, data));
        }
    }

    void RendererContextGL::setUniform2fv(uint32_t loc, int num, const float* data)
    {
        bool changed = false;
        for (int i = 0; i < num; ++i)
        {
            if (m_uniformStateCache.updateUniformCache(loc + i, *(const UniformStateCache::f2*)&data[2 * i]))
            {
                changed = true;
            }
        }
        if (changed)
        {
            GL_CHECK(glUniform2fv(loc, num, data));
        }
    }

    void RendererContextGL::setUniform3fv(uint32_t loc, int num, const float* data)
    {
        bool changed = false;
        for (int i = 0; i < num; ++i)
        {
            if (m_uniformStateCache.updateUniformCache(loc + i, *(const UniformStateCache::f3*)&data[3 * i]))
            {
                changed = true;
            }
        }
        if (changed)
        {
            GL_CHECK(glUniform3fv(loc, num, data));
        }
    }

    void RendererContextGL::setUniform4fv(uint32_t loc, int num, const float* data)
    {
        bool changed = false;
        for (int i = 0; i < num; ++i)
        {
            if (m_uniformStateCache.updateUniformCache(loc + i, *(const UniformStateCache::f4*)&data[4 * i]))
            {
                changed = true;
            }
        }
        if (changed)
        {
            GL_CHECK(glUniform4fv(loc, num, data));
        }
    }

    void RendererContextGL::setUniformMatrix3fv(uint32_t loc, int num, GLboolean transpose, const float* data)
    {
        bool changed = false;
        for (int i = 0; i < num; ++i)
        {
            if (m_uniformStateCache.updateUniformCache(loc + i, *(const UniformStateCache::f3x3*)&data[9 * i]))
            {
                changed = true;
            }
        }
        if (changed)
        {
            GL_CHECK(glUniformMatrix3fv(loc, num, transpose, data));
        }
    }

    void RendererContextGL::setUniformMatrix4fv(uint32_t loc, int num, GLboolean transpose, const float* data)
    {
        bool changed = false;
        for (int i = 0; i < num; ++i)
        {
            if (m_uniformStateCache.updateUniformCache(loc + i, *(const UniformStateCache::f4x4*)&data[16 * i]))
            {
                changed = true;
            }
        }
        if (changed)
        {
            GL_CHECK(glUniformMatrix4fv(loc, num, transpose, data));
        }
    }



	RendererContextGL* s_renderGL = NULL;

	RendererContextI* rendererCreate(const Init& _init)
	{
		s_renderGL = BX_NEW(g_allocator, RendererContextGL);
		if (!s_renderGL->init(_init) )
		{
			BX_DELETE(g_allocator, s_renderGL);
			s_renderGL = NULL;
		}
		return s_renderGL;
	}

	void rendererDestroy()
	{
		s_renderGL->shutdown();
		BX_DELETE(g_allocator, s_renderGL);
		s_renderGL = NULL;
	}

	void RendererContextGL::submitBlit(BlitState& _bs, uint16_t _view)
	{
		if (m_blitSupported)
		{
			while (_bs.hasItem(_view) )
			{
				const BlitItem& bi = _bs.advance();

				const TextureGL& src = m_textures[bi.m_src.idx];
				const TextureGL& dst = m_textures[bi.m_dst.idx];

				GL_CHECK(glCopyImageSubData(src.m_id
					, src.m_target
					, bi.m_srcMip
					, bi.m_srcX
					, bi.m_srcY
					, bi.m_srcZ
					, dst.m_id
					, dst.m_target
					, bi.m_dstMip
					, bi.m_dstX
					, bi.m_dstY
					, bi.m_dstZ
					, bi.m_width
					, bi.m_height
					, bx::uint32_imax(bi.m_depth, 1)
					) );
				}
		}
		else if (BX_ENABLED(BGFX_GL_CONFIG_BLIT_EMULATION) )
		{
			while (_bs.hasItem(_view) )
			{
				const BlitItem& bi = _bs.advance();

				const TextureGL& src = m_textures[bi.m_src.idx];
				const TextureGL& dst = m_textures[bi.m_dst.idx];

				BX_ASSERT(0 == bi.m_srcZ && 0 == bi.m_dstZ && 1 >= bi.m_depth
					, "Blitting 3D regions is not supported"
					);

				GLuint fbo;
				GL_CHECK(glGenFramebuffers(1, &fbo) );

				GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, fbo) );

				GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER
					, GL_COLOR_ATTACHMENT0
					, GL_TEXTURE_2D
					, src.m_id
					, bi.m_srcMip
					) );

				GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
				BX_ASSERT(GL_FRAMEBUFFER_COMPLETE == status, "glCheckFramebufferStatus failed 0x%08x", status);
				BX_UNUSED(status);

				GL_CHECK(glActiveTexture(GL_TEXTURE0) );
				GL_CHECK(glBindTexture(GL_TEXTURE_2D, dst.m_id) );

				GL_CHECK(glCopyTexSubImage2D(GL_TEXTURE_2D
					, bi.m_dstMip
					, bi.m_dstX
					, bi.m_dstY
					, bi.m_srcX
					, bi.m_srcY
					, bi.m_width
					, bi.m_height
					) );

				GL_CHECK(glDeleteFramebuffers(1, &fbo) );
				GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m_currentFbo) );
			}
		}
	}

	void RendererContextGL::submit(Frame* _render, TextVideoMemBlitter& _textVideoMemBlitter)
	{
		if (_render->m_capture)
		{
			renderDocTriggerCapture();
		}

		m_glctx.makeCurrent(NULL);

		BGFX_GL_PROFILER_BEGIN_LITERAL("rendererSubmit", kColorView);

		if (1 < m_numWindows
		&&  m_vaoSupport)
		{
			m_vaoSupport = false;
			GL_CHECK(glBindVertexArray(0) );
			GL_CHECK(glDeleteVertexArrays(1, &m_vao) );
			m_vao = 0;
		}

		const GLuint defaultVao = m_vao;
		if (0 != defaultVao)
		{
			GL_CHECK(glBindVertexArray(defaultVao) );
		}

		GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m_backBufferFbo) );
		GL_CHECK(glFrontFace(GL_CW) );

		OpenGLStateContext& contextState = m_renderingContextState;
		resetContextState(contextState);
		SwapChainGL* currentSwapChain = NULL;

		updateResolution(_render->m_resolution);

		int64_t timeBegin = bx::getHPCounter();
		int64_t captureElapsed = 0;

		uint32_t frameQueryIdx = UINT32_MAX;

		if (m_timerQuerySupport
		&&  !BX_ENABLED(BX_PLATFORM_OSX) )
		{
			frameQueryIdx = m_gpuTimer.begin(BGFX_CONFIG_MAX_VIEWS);
		}

		if (0 < _render->m_iboffset)
		{
			BGFX_PROFILER_SCOPE("bgfx/Update transient index buffer", kColorResource);
			TransientIndexBuffer* ib = _render->m_transientIb;
			m_indexBuffers[ib->handle.idx].update(0, _render->m_iboffset, ib->data, true);
		}

		if (0 < _render->m_vboffset)
		{
			BGFX_PROFILER_SCOPE("bgfx/Update transient vertex buffer", kColorResource);
			TransientVertexBuffer* vb = _render->m_transientVb;
			m_vertexBuffers[vb->handle.idx].update(0, _render->m_vboffset, vb->data, true);
		}

		RenderDraw currentState;
		currentState.clear();
		currentState.m_stateFlags = BGFX_STATE_NONE;
		currentState.m_stencil    = packStencil(BGFX_STENCIL_NONE, BGFX_STENCIL_NONE);

		RenderBind currentBind;
		currentBind.clear();

		static ViewState viewState;
		viewState.reset(_render);

		ProgramHandle currentProgram = BGFX_INVALID_HANDLE;
		ProgramHandle boundProgram   = BGFX_INVALID_HANDLE;
		SortKey key;
		uint16_t view = UINT16_MAX;
		FrameBufferHandle fbh = { BGFX_CONFIG_MAX_FRAME_BUFFERS };

		BlitState bs(_render);

		int32_t resolutionHeight = _render->m_resolution.height;
		uint32_t blendFactor = 0;

		uint8_t primIndex;
		{
			const uint64_t pt = 0;
			primIndex = uint8_t(pt>>BGFX_STATE_PT_SHIFT);
		}
		PrimInfo prim = s_primInfo[primIndex];

		GL_CHECK(OpenGL::PolygonMode(GL_FRONT_AND_BACK
			, _render->m_debug&BGFX_DEBUG_WIREFRAME
			? GL_LINE
			: GL_FILL
			) );

		bool wasCompute = false;
		bool viewHasScissor = false;
		Rect viewScissorRect;
		viewScissorRect.clear();
		uint16_t discardFlags = BGFX_CLEAR_NONE;

		const bool blendIndependentSupported = OpenGL::s_extension[Extension::ARB_draw_buffers_blend].m_supported;
		const bool computeSupported = false
			|| (BX_ENABLED(BGFX_CONFIG_RENDERER_OPENGL) && OpenGL::s_extension[Extension::ARB_compute_shader].m_supported)
			||  (OpenGL::Support_GLES3_1())
			;
		const uint32_t maxComputeBindings = g_caps.limits.maxComputeBindings;

		uint32_t statsNumPrimsSubmitted[BX_COUNTOF(s_primInfo)] = {};
		uint32_t statsNumPrimsRendered[BX_COUNTOF(s_primInfo)] = {};
		uint32_t statsNumInstances[BX_COUNTOF(s_primInfo)] = {};
		uint32_t statsNumIndices = 0;
		uint32_t statsKeyType[2] = {};

		Profiler<TimerQueryGL> profiler(
			  _render
			, m_gpuTimer
			, s_viewName
			, m_timerQuerySupport && !BX_ENABLED(BX_PLATFORM_OSX)
			);

		if (m_occlusionQuerySupport)
		{
			m_occlusionQuery.resolve(_render);
		}

		if (0 == (_render->m_debug&BGFX_DEBUG_IFH) )
		{
			GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m_msaaBackBufferFbo) );

			viewState.m_rect = _render->m_view[0].m_rect;
			int32_t numItems = _render->m_numRenderItems;

			for (int32_t item = 0; item < numItems;)
			{
				const uint64_t encodedKey = _render->m_sortKeys[item];
				const bool isCompute = key.decode(encodedKey);
				statsKeyType[isCompute]++;

				const bool viewChanged = 0
					|| key.m_view != view
					|| item == numItems
					;

				const uint32_t itemIdx       = item;
				const RenderItem& renderItem = _render->m_renderItem[itemIdx];
				const RenderBind& renderBind = _render->m_renderItemBind[itemIdx];
				++item;

				if (viewChanged)
				{
					view = key.m_view;
					currentProgram = BGFX_INVALID_HANDLE;

					if (item > 1)
					{
						profiler.end();
					}

					BGFX_GL_PROFILER_END();

					if (_render->m_view[view].m_fbh.idx != fbh.idx)
					{
						fbh = _render->m_view[view].m_fbh;
						resolutionHeight = _render->m_resolution.height;
						resolutionHeight = setFrameBuffer(fbh, resolutionHeight, discardFlags);

						if (currentSwapChain != m_glctx.m_current)
						{
							resetContextState(contextState);
							currentSwapChain = m_glctx.m_current;
						}
					}

					setViewType(view, "  ");
					BGFX_GL_PROFILER_BEGIN(view, kColorView);

					profiler.begin(view);

					viewState.m_rect = _render->m_view[view].m_rect;

					const Rect& scissorRect = _render->m_view[view].m_scissor;
					viewHasScissor  = !scissorRect.isZero();
					viewScissorRect = viewHasScissor ? scissorRect : viewState.m_rect;

					// hack : 之前viewport随处乱调用，有些用法rt高和rect高不一致。另外gl翻转在引擎里处理
					setViewPort(viewState.m_rect.m_x
						, viewState.m_rect.m_y	// resolutionHeight - viewState.m_rect.m_height - viewState.m_rect.m_y 
						, viewState.m_rect.m_width
						, viewState.m_rect.m_height);

					Clear& clear = _render->m_view[view].m_clear;
					discardFlags = clear.m_flags & BGFX_CLEAR_DISCARD_MASK;

					if (BGFX_CLEAR_NONE != (clear.m_flags & BGFX_CLEAR_MASK) )
					{
						clearQuad(viewState.m_rect, clear, resolutionHeight, _render->m_colorPalette);
					}

					submitBlit(bs, view);
				}

				if (isCompute)
				{
					if (!wasCompute)
					{
						wasCompute = true;

						setViewType(view, "C");
						BGFX_GL_PROFILER_END();
						BGFX_GL_PROFILER_BEGIN(view, kColorCompute);
					}

					if (computeSupported)
					{
						const RenderCompute& compute = renderItem.compute;

						ProgramGL& program = m_program[key.m_program.idx];
						setProgram(program.m_id);

						GLbitfield barrier = 0;
						for (uint32_t ii = 0; ii < maxComputeBindings; ++ii)
						{
							const Binding& bind = renderBind.m_bind[ii];
							if (kInvalidHandle != bind.m_idx)
							{
								switch (bind.m_type)
								{
								case Binding::Texture:
									{
										TextureGL& texture = m_textures[bind.m_idx];
										texture.commit(ii, bind.m_samplerFlags, _render->m_colorPalette);
									}
									break;

								case Binding::Image:
									{
										const TextureGL& texture = m_textures[bind.m_idx];
										GL_CHECK(glBindImageTexture(ii
											, texture.m_id
											, bind.m_mip
											, texture.isCubeMap() || texture.m_target == GL_TEXTURE_2D_ARRAY ? GL_TRUE : GL_FALSE
											, 0
											, s_access[bind.m_access]
											, OpenGL::s_imageFormat[bind.m_format])
											);
										barrier |= GL_SHADER_IMAGE_ACCESS_BARRIER_BIT;
									}
									break;

								case Binding::IndexBuffer:
									{
										const IndexBufferGL& buffer = m_indexBuffers[bind.m_idx];
										GL_CHECK(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ii, buffer.m_id) );
										barrier |= GL_SHADER_STORAGE_BARRIER_BIT;
									}
									break;

								case Binding::VertexBuffer:
									{
										const VertexBufferGL& buffer = m_vertexBuffers[bind.m_idx];
										GL_CHECK(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ii, buffer.m_id) );
										barrier |= GL_SHADER_STORAGE_BARRIER_BIT;
									}
									break;
								}
							}
						}

						if (0 != barrier)
						{
							bool constantsChanged = compute.m_uniformBegin < compute.m_uniformEnd;
							rendererUpdateUniforms(this, _render->m_uniformBuffer[compute.m_uniformIdx], compute.m_uniformBegin, compute.m_uniformEnd);

							if (constantsChanged
							&&  NULL != program.m_constantBuffer)
							{
								commit(*program.m_constantBuffer);
							}

							viewState.setPredefined<1>(this, view, program, _render, compute);

							if (isValid(compute.m_indirectBuffer) )
							{
								barrier |= GL_COMMAND_BARRIER_BIT;
								const VertexBufferGL& vb = m_vertexBuffers[compute.m_indirectBuffer.idx];
								if (currentState.m_indirectBuffer.idx != compute.m_indirectBuffer.idx)
								{
									currentState.m_indirectBuffer = compute.m_indirectBuffer;
									GL_CHECK(glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, vb.m_id) );
								}

								uint32_t numDrawIndirect = UINT16_MAX == compute.m_numIndirect
									? vb.m_size/BGFX_CONFIG_DRAW_INDIRECT_STRIDE
									: compute.m_numIndirect
									;

								uintptr_t args = compute.m_startIndirect * BGFX_CONFIG_DRAW_INDIRECT_STRIDE;
								for (uint32_t ii = 0; ii < numDrawIndirect; ++ii)
								{
									GL_CHECK(glDispatchComputeIndirect( (GLintptr)args) );
									args += BGFX_CONFIG_DRAW_INDIRECT_STRIDE;
								}
							}
							else
							{
								if (isValid(currentState.m_indirectBuffer) )
								{
									currentState.m_indirectBuffer.idx = kInvalidHandle;
									GL_CHECK(glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, 0) );
								}

								GL_CHECK(glDispatchCompute(compute.m_numX, compute.m_numY, compute.m_numZ) );
							}

							GL_CHECK(glMemoryBarrier(barrier) );
						}
					}

					continue;
				}

				bool resetState = viewChanged || wasCompute;

				if (wasCompute)
				{
					wasCompute = false;

					setViewType(view, " ");
					BGFX_GL_PROFILER_END();
					BGFX_GL_PROFILER_BEGIN(view, kColorDraw);
				}

				const RenderDraw& draw = renderItem.draw;

				const bool hasOcclusionQuery = 0 != (draw.m_stateFlags & BGFX_STATE_INTERNAL_OCCLUSION_QUERY);
				{
					const bool occluded = true
						&& isValid(draw.m_occlusionQuery)
						&& !hasOcclusionQuery
						&& !isVisible(_render, draw.m_occlusionQuery, 0 != (draw.m_submitFlags&BGFX_SUBMIT_INTERNAL_OCCLUSION_VISIBLE) )
						;

					if (occluded
					||  _render->m_frameCache.isZeroArea(viewScissorRect, draw.m_scissor) )
					{
						if (resetState)
						{
							currentState.clear();
							currentState.m_scissor = !draw.m_scissor;
							currentBind.clear();
						}

						continue;
					}
				}

				m_currState.m_statesflag.m_renderstate = draw.m_stateFlags;
				m_currState.m_statesflag.m_stencil = draw.m_stencil;
				m_currState.m_rgba = draw.m_rgba;
				m_currState.m_scissor = draw.m_scissor;

				if (resetState)
				{
					currentState.clear();
					currentBind.clear();
				}

				checkRenderState(m_currState);
				setScissorRect(resolutionHeight, _render->m_view[view], _render->m_frameCache.m_rectCache);

				const uint64_t pt = draw.m_stateFlags & BGFX_STATE_PT_MASK;
				primIndex = uint8_t(pt >> BGFX_STATE_PT_SHIFT);
				prim = s_primInfo[primIndex];

				bool programChanged = false;
				bool constantsChanged = draw.m_uniformBegin < draw.m_uniformEnd;
				bool bindAttribs = false;
				rendererUpdateUniforms(this, _render->m_uniformBuffer[draw.m_uniformIdx], draw.m_uniformBegin, draw.m_uniformEnd);

				if (key.m_program.idx != currentProgram.idx)
				{
					currentProgram = key.m_program;
					GLuint id = isValid(currentProgram) ? m_program[currentProgram.idx].m_id : 0;

					// Skip rendering if program index is valid, but program is invalid.
					currentProgram = 0 == id ? ProgramHandle{kInvalidHandle} : currentProgram;

					setProgram(id);
					programChanged =
						constantsChanged =
						bindAttribs = true;
				}

				if (isValid(currentProgram) )
				{
					// Update Renderer state In OpenGL Context,It's written exactly like UE4
					updateViewport(contextState);
					updateRasterizerState(contextState);
					updateDepthStencilState(contextState);
					setBlendStateForActiveRenderTargets(contextState);
					updateScissorRect(contextState);

					ProgramGL& program = m_program[currentProgram.idx];

					if (constantsChanged
					&&  NULL != program.m_constantBuffer)
					{
						commit(*program.m_constantBuffer);
					}

					viewState.setPredefined<1>(this, view, program, _render, draw);

					{
						GLbitfield barrier = 0;
						for (uint32_t stage = 0; stage < BGFX_CONFIG_MAX_TEXTURE_SAMPLERS; ++stage)
						{
							const Binding& bind = renderBind.m_bind[stage];
							Binding& current = currentBind.m_bind[stage];
							if (current.m_idx          != bind.m_idx
							||  current.m_type         != bind.m_type
							||  current.m_samplerFlags != bind.m_samplerFlags
							||  programChanged)
							{
								if (kInvalidHandle != bind.m_idx)
								{
									switch (bind.m_type)
									{
									case Binding::Image:
										{
											const TextureGL& texture = m_textures[bind.m_idx];
											GL_CHECK(glBindImageTexture(stage
																		, texture.m_id
																		, bind.m_mip
																		, texture.isCubeMap() || texture.m_target == GL_TEXTURE_2D_ARRAY ? GL_TRUE : GL_FALSE
																		, 0
																		, s_access[bind.m_access]
																		, OpenGL::s_imageFormat[bind.m_format])
													);
											barrier |= GL_SHADER_IMAGE_ACCESS_BARRIER_BIT;
										}
										break;

									case Binding::Texture:
										{
											TextureGL& texture = m_textures[bind.m_idx];
											texture.commit(stage, bind.m_samplerFlags, _render->m_colorPalette);
										}
										break;

									case Binding::IndexBuffer:
										{
											const IndexBufferGL& buffer = m_indexBuffers[bind.m_idx];
											GL_CHECK(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, stage, buffer.m_id) );
										}
										break;

									case Binding::VertexBuffer:
										{
											const VertexBufferGL& buffer = m_vertexBuffers[bind.m_idx];
											GL_CHECK(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, stage, buffer.m_id) );
										}
										break;
									}
								}
							}

							current = bind;
						}

						if (0 != barrier)
						{
							GL_CHECK(glMemoryBarrier(barrier) );
						}
					}

					{
						for (uint32_t idx = 0, streamMask = draw.m_streamMask
							; 0 != streamMask
							; streamMask >>= 1, idx += 1
							)
						{
							const uint32_t ntz = bx::uint32_cnttz(streamMask);
							streamMask >>= ntz;
							idx         += ntz;

							if (currentState.m_stream[idx].m_handle.idx != draw.m_stream[idx].m_handle.idx)
							{
								currentState.m_stream[idx].m_handle = draw.m_stream[idx].m_handle;
								bindAttribs = true;
							}

							if (currentState.m_stream[idx].m_startVertex != draw.m_stream[idx].m_startVertex)
							{
								currentState.m_stream[idx].m_startVertex = draw.m_stream[idx].m_startVertex;
								bindAttribs = true;
							}
						}

						if (programChanged
						||  currentState.m_streamMask             != draw.m_streamMask
						||  currentState.m_instanceDataBuffer.idx != draw.m_instanceDataBuffer.idx
						||  currentState.m_instanceDataOffset     != draw.m_instanceDataOffset
						||  currentState.m_instanceDataStride     != draw.m_instanceDataStride)
						{
							currentState.m_streamMask         = draw.m_streamMask;
							currentState.m_instanceDataBuffer = draw.m_instanceDataBuffer;
							currentState.m_instanceDataOffset = draw.m_instanceDataOffset;
							currentState.m_instanceDataStride = draw.m_instanceDataStride;

							bindAttribs = true;
						}

						if (currentState.m_indexBuffer.idx != draw.m_indexBuffer.idx)
						{
							currentState.m_indexBuffer = draw.m_indexBuffer;

							if (isValid(draw.m_indexBuffer) )
							{
								IndexBufferGL& ib = m_indexBuffers[draw.m_indexBuffer.idx];
								GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib.m_id) );
							}
							else
							{
								GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0) );
							}
						}

						if (currentState.m_startIndex != draw.m_startIndex)
						{
							currentState.m_startIndex = draw.m_startIndex;
						}

						if (0 != currentState.m_streamMask)
						{
							if (bindAttribs)
							{
								if (isValid(boundProgram) )
								{
									m_program[boundProgram.idx].unbindAttributes();
									m_program[boundProgram.idx].unbindInstanceData();
								}

								boundProgram = currentProgram;

								program.bindAttributesBegin();

								if (UINT8_MAX != draw.m_streamMask)
								{
									for (uint32_t idx = 0, streamMask = draw.m_streamMask
										; 0 != streamMask
										; streamMask >>= 1, idx += 1
										)
									{
										const uint32_t ntz = bx::uint32_cnttz(streamMask);
										streamMask >>= ntz;
										idx         += ntz;

										const VertexBufferGL& vb = m_vertexBuffers[draw.m_stream[idx].m_handle.idx];
										const uint16_t decl = isValid(draw.m_stream[idx].m_layoutHandle)
											? draw.m_stream[idx].m_layoutHandle.idx
											: vb.m_layoutHandle.idx;
										GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vb.m_id) );
										program.bindAttributes(m_vertexLayouts[decl], draw.m_stream[idx].m_startVertex);
									}
								}

								if (isValid(draw.m_instanceDataBuffer) )
								{
									GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffers[draw.m_instanceDataBuffer.idx].m_id) );
									program.bindInstanceData(draw.m_instanceDataStride, draw.m_instanceDataOffset);
								}

								program.bindAttributesEnd();
							}
						}
					}

					if (0 != currentState.m_streamMask)
					{
						uint32_t numVertices = draw.m_numVertices;
						if (UINT32_MAX == numVertices)
						{
							for (uint32_t idx = 0, streamMask = draw.m_streamMask
								; 0 != streamMask
								; streamMask >>= 1, idx += 1
								)
							{
								const uint32_t ntz = bx::uint32_cnttz(streamMask);
								streamMask >>= ntz;
								idx         += ntz;

								const VertexBufferGL& vb = m_vertexBuffers[draw.m_stream[idx].m_handle.idx];
								uint16_t decl = !isValid(vb.m_layoutHandle) ? draw.m_stream[idx].m_layoutHandle.idx : vb.m_layoutHandle.idx;
								const VertexLayout& layout = m_vertexLayouts[decl];

								numVertices = bx::uint32_min(numVertices, vb.m_size/layout.m_stride);
							}
						}

						uint32_t numIndices        = 0;
						uint32_t numPrimsSubmitted = 0;
						uint32_t numInstances      = 0;
						uint32_t numPrimsRendered  = 0;
						uint32_t numDrawIndirect   = 0;

						if (hasOcclusionQuery)
						{
							m_occlusionQuery.begin(_render, draw.m_occlusionQuery);
						}

						if (isValid(draw.m_indirectBuffer) )
						{
							const VertexBufferGL& vb = m_vertexBuffers[draw.m_indirectBuffer.idx];
							if (currentState.m_indirectBuffer.idx != draw.m_indirectBuffer.idx)
							{
								currentState.m_indirectBuffer = draw.m_indirectBuffer;
								GL_CHECK(glBindBuffer(GL_DRAW_INDIRECT_BUFFER, vb.m_id) );
							}

							if (isValid(draw.m_indexBuffer) )
							{
								const IndexBufferGL& ib = m_indexBuffers[draw.m_indexBuffer.idx];
								const bool hasIndex16 = 0 == (ib.m_flags & BGFX_BUFFER_INDEX32);
								const GLenum indexFormat = hasIndex16
									? GL_UNSIGNED_SHORT
									: GL_UNSIGNED_INT
									;

								numDrawIndirect = UINT16_MAX == draw.m_numIndirect
									? vb.m_size/BGFX_CONFIG_DRAW_INDIRECT_STRIDE
									: draw.m_numIndirect
									;

								uintptr_t args = draw.m_startIndirect * BGFX_CONFIG_DRAW_INDIRECT_STRIDE;
								GL_CHECK(OpenGL::MultiDrawElementsIndirect(prim.m_type, indexFormat
									, (void*)args
									, numDrawIndirect
									, BGFX_CONFIG_DRAW_INDIRECT_STRIDE
									) );
							}
							else
							{
								numDrawIndirect = UINT16_MAX == draw.m_numIndirect
									? vb.m_size/BGFX_CONFIG_DRAW_INDIRECT_STRIDE
									: draw.m_numIndirect
									;

								uintptr_t args = draw.m_startIndirect * BGFX_CONFIG_DRAW_INDIRECT_STRIDE;
								GL_CHECK(OpenGL::MultiDrawArraysIndirect(prim.m_type
									, (void*)args
									, numDrawIndirect
									, BGFX_CONFIG_DRAW_INDIRECT_STRIDE
									) );
							}
						}
						else
						{
							if (isValid(currentState.m_indirectBuffer) )
							{
								currentState.m_indirectBuffer.idx = kInvalidHandle;
								GL_CHECK(glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0) );
							}

							if (isValid(draw.m_indexBuffer) )
							{
								const IndexBufferGL& ib  = m_indexBuffers[draw.m_indexBuffer.idx];
								const bool isIndex16     = draw.isIndex16();
								const uint32_t indexSize = isIndex16 ? 2 : 4;
								const GLenum indexFormat = isIndex16
									? GL_UNSIGNED_SHORT
									: GL_UNSIGNED_INT
									;

								if (UINT32_MAX == draw.m_numIndices)
								{
									numIndices        = ib.m_size/indexSize;
									numPrimsSubmitted = numIndices/prim.m_div - prim.m_sub;
									numInstances      = draw.m_numInstances;
									numPrimsRendered  = numPrimsSubmitted*draw.m_numInstances;

									GL_CHECK(OpenGL::DrawElementsInstanced(prim.m_type
										, numIndices
										, indexFormat
										, (void*)0
										, draw.m_numInstances
										) );
								}
								else if (prim.m_min <= draw.m_numIndices)
								{
									numIndices        = draw.m_numIndices;
									numPrimsSubmitted = numIndices/prim.m_div - prim.m_sub;
									numInstances      = draw.m_numInstances;
									numPrimsRendered  = numPrimsSubmitted*draw.m_numInstances;

									GL_CHECK(OpenGL::DrawElementsInstanced(prim.m_type
										, numIndices
										, indexFormat
										, (void*)(uintptr_t)(draw.m_startIndex*indexSize)
										, draw.m_numInstances
										) );
								}
							}
							else
							{
								numPrimsSubmitted = numVertices/prim.m_div - prim.m_sub;
								numInstances      = draw.m_numInstances;
								numPrimsRendered  = numPrimsSubmitted*draw.m_numInstances;

								GL_CHECK(OpenGL::DrawArraysInstanced(prim.m_type
									, 0
									, numVertices
									, draw.m_numInstances
									) );
							}
						}

						if (hasOcclusionQuery)
						{
							m_occlusionQuery.end();
						}

						statsNumPrimsSubmitted[primIndex] += numPrimsSubmitted;
						statsNumPrimsRendered[primIndex]  += numPrimsRendered;
						statsNumInstances[primIndex]      += numInstances;
						statsNumIndices += numIndices;
					}
				}
			}

			if (isValid(boundProgram) )
			{
				m_program[boundProgram.idx].unbindAttributes();
				boundProgram = BGFX_INVALID_HANDLE;
			}

			if (wasCompute)
			{
				setViewType(view, "C");
				BGFX_GL_PROFILER_END();
				BGFX_GL_PROFILER_BEGIN(view, kColorCompute);
			}

			submitBlit(bs, BGFX_CONFIG_MAX_VIEWS);

			blitMsaaFbo();

			if (m_vaoSupport)
			{
				GL_CHECK(glBindVertexArray(m_vao) );
			}

			if (0 < _render->m_numRenderItems)
			{
				if (0 != (m_resolution.reset & BGFX_RESET_FLUSH_AFTER_RENDER) )
				{
					GL_CHECK(glFlush() );
				}

				captureElapsed = -bx::getHPCounter();
				capture();
				captureElapsed += bx::getHPCounter();

				profiler.end();
			}
		}

		BGFX_GL_PROFILER_END();

		m_glctx.makeCurrent(NULL);
		int64_t timeEnd = bx::getHPCounter();
		int64_t frameTime = timeEnd - timeBegin;

		static int64_t min = frameTime;
		static int64_t max = frameTime;
		min = min > frameTime ? frameTime : min;
		max = max < frameTime ? frameTime : max;

		static uint32_t maxGpuLatency = 0;
		static double   maxGpuElapsed = 0.0f;
		double elapsedGpuMs = 0.0;

		if (UINT32_MAX != frameQueryIdx)
		{
			m_gpuTimer.end(frameQueryIdx);

			const TimerQueryGL::Result& result = m_gpuTimer.m_result[BGFX_CONFIG_MAX_VIEWS];
			double toGpuMs = 1000.0 / 1e9;
			elapsedGpuMs   = (result.m_end - result.m_begin) * toGpuMs;
			maxGpuElapsed  = elapsedGpuMs > maxGpuElapsed ? elapsedGpuMs : maxGpuElapsed;

			maxGpuLatency = bx::uint32_imax(maxGpuLatency, result.m_pending-1);
		}

		const int64_t timerFreq = bx::getHPFrequency();

		Stats& perfStats = _render->m_perfStats;
		perfStats.cpuTimeBegin  = timeBegin;
		perfStats.cpuTimeEnd    = timeEnd;
		perfStats.cpuTimerFreq  = timerFreq;
		const TimerQueryGL::Result& result = m_gpuTimer.m_result[BGFX_CONFIG_MAX_VIEWS];
		perfStats.gpuTimeBegin  = result.m_begin;
		perfStats.gpuTimeEnd    = result.m_end;
		perfStats.gpuTimerFreq  = 1000000000;
		perfStats.numDraw       = statsKeyType[0];
		perfStats.numCompute    = statsKeyType[1];
		perfStats.numBlit       = _render->m_numBlitItems;
		perfStats.maxGpuLatency = maxGpuLatency;
		bx::memCopy(perfStats.numPrims, statsNumPrimsRendered, sizeof(perfStats.numPrims) );
		perfStats.gpuMemoryMax  = -INT64_MAX;
		perfStats.gpuMemoryUsed = -INT64_MAX;

		if (_render->m_debug & (BGFX_DEBUG_IFH|BGFX_DEBUG_STATS) )
		{
			BGFX_GL_PROFILER_BEGIN_LITERAL("debugstats", kColorFrame);

			m_needPresent = true;
			TextVideoMem& tvm = m_textVideoMem;

			static int64_t next = timeEnd;

			if (timeEnd >= next)
			{
				next = timeEnd + timerFreq;
				double freq = double(timerFreq);
				double toMs = 1000.0/freq;

				tvm.clear();
				uint16_t pos = 0;
				tvm.printf(0, pos++, BGFX_CONFIG_DEBUG ? 0x8c : 0x8f
					, " %s / " BX_COMPILER_NAME
					  " / " BX_CPU_NAME
					  " / " BX_ARCH_NAME
					  " / " BX_PLATFORM_NAME
					  " / Version 1.%d.%d (commit: " BGFX_REV_SHA1 ")"
					, getRendererName()
					, BGFX_API_VERSION
					, BGFX_REV_NUMBER
					);

				tvm.printf(0, pos++, 0x8f, "       Vendor: %s ", m_vendor);
				tvm.printf(0, pos++, 0x8f, "     Renderer: %s ", m_renderer);
				tvm.printf(0, pos++, 0x8f, "      Version: %s ", m_version);
				tvm.printf(0, pos++, 0x8f, " GLSL version: %s ", m_glslVersion);

				char processMemoryUsed[16];
				bx::prettify(processMemoryUsed, BX_COUNTOF(processMemoryUsed), bx::getProcessMemoryUsed() );
				tvm.printf(0, pos++, 0x8f, "       Memory: %s (process) ", processMemoryUsed);

				pos = 10;
				tvm.printf(10, pos++, 0x8b, "        Frame: %7.3f, % 7.3f \x1f, % 7.3f \x1e [ms] / % 6.2f FPS "
					, double(frameTime)*toMs
					, double(min)*toMs
					, double(max)*toMs
					, freq/frameTime
					);

				const uint32_t msaa = (m_resolution.reset&BGFX_RESET_MSAA_MASK)>>BGFX_RESET_MSAA_SHIFT;
				tvm.printf(10, pos++, 0x8b, "  Reset flags: [%c] vsync, [%c] MSAAx%d, [%c] MaxAnisotropy "
					, !!(m_resolution.reset&BGFX_RESET_VSYNC) ? '\xfe' : ' '
					, 0 != msaa ? '\xfe' : ' '
					, 1<<msaa
					, !!(m_resolution.reset&BGFX_RESET_MAXANISOTROPY) ? '\xfe' : ' '
					);

				double elapsedCpuMs = double(frameTime)*toMs;
				tvm.printf(10, pos++, 0x8b, "    Submitted: %5d (draw %5d, compute %4d) / CPU %7.4f [ms] %c GPU %7.4f [ms] (latency %d) "
					, _render->m_numRenderItems
					, statsKeyType[0]
					, statsKeyType[1]
					, elapsedCpuMs
					, elapsedCpuMs > elapsedGpuMs ? '>' : '<'
					, maxGpuElapsed
					, maxGpuLatency
					);
				maxGpuLatency = 0;
				maxGpuElapsed = 0.0;

				for (uint32_t ii = 0; ii < BX_COUNTOF(s_primInfo); ++ii)
				{
					tvm.printf(10, pos++, 0x8b, "   %10s: %7d (#inst: %5d), submitted: %7d "
						, getName(Topology::Enum(ii) )
						, statsNumPrimsRendered[ii]
						, statsNumInstances[ii]
						, statsNumPrimsSubmitted[ii]
						);
				}

				if (NULL != m_renderdocdll)
				{
					tvm.printf(tvm.m_width-27, 0, 0x4f, " [F11 - RenderDoc capture] ");
				}

				tvm.printf(10, pos++, 0x8b, "      Indices: %7d ", statsNumIndices);
//				tvm.printf(10, pos++, 0x8b, " Uniform size: %7d, Max: %7d ", _render->m_uniformEnd, _render->m_uniformMax);
				tvm.printf(10, pos++, 0x8b, "     DVB size: %7d ", _render->m_vboffset);
				tvm.printf(10, pos++, 0x8b, "     DIB size: %7d ", _render->m_iboffset);

				pos++;
				tvm.printf(10, pos++, 0x8b, " State cache:     ");
				tvm.printf(10, pos++, 0x8b, " Sampler ");
				tvm.printf(10, pos++, 0x8b, " %6d  "
					, m_samplerStateCache.getCount()
					);

#if BGFX_CONFIG_RENDERER_OPENGL
				if (OpenGL::s_extension[Extension::ATI_meminfo].m_supported)
				{
					GLint vboFree[4];
					GL_CHECK(glGetIntegerv(GL_VBO_FREE_MEMORY_ATI, vboFree) );

					GLint texFree[4];
					GL_CHECK(glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI, texFree) );

					GLint rbfFree[4];
					GL_CHECK(glGetIntegerv(GL_RENDERBUFFER_FREE_MEMORY_ATI, rbfFree) );

					pos++;
					tvm.printf(10, pos++, 0x8c, " -------------|    free|  free b|     aux|  aux fb ");

					char tmp0[16];
					char tmp1[16];
					char tmp2[16];
					char tmp3[16];

					bx::prettify(tmp0, BX_COUNTOF(tmp0), vboFree[0]);
					bx::prettify(tmp1, BX_COUNTOF(tmp1), vboFree[1]);
					bx::prettify(tmp2, BX_COUNTOF(tmp2), vboFree[2]);
					bx::prettify(tmp3, BX_COUNTOF(tmp3), vboFree[3]);
					tvm.printf(10, pos++, 0x8b, "           VBO: %10s, %10s, %10s, %10s ", tmp0, tmp1, tmp2, tmp3);

					bx::prettify(tmp0, BX_COUNTOF(tmp0), texFree[0]);
					bx::prettify(tmp1, BX_COUNTOF(tmp1), texFree[1]);
					bx::prettify(tmp2, BX_COUNTOF(tmp2), texFree[2]);
					bx::prettify(tmp3, BX_COUNTOF(tmp3), texFree[3]);
					tvm.printf(10, pos++, 0x8b, "       Texture: %10s, %10s, %10s, %10s ", tmp0, tmp1, tmp2, tmp3);

					bx::prettify(tmp0, BX_COUNTOF(tmp0), rbfFree[0]);
					bx::prettify(tmp1, BX_COUNTOF(tmp1), rbfFree[1]);
					bx::prettify(tmp2, BX_COUNTOF(tmp2), rbfFree[2]);
					bx::prettify(tmp3, BX_COUNTOF(tmp3), rbfFree[3]);
					tvm.printf(10, pos++, 0x8b, " Render Buffer: %10s, %10s, %10s, %10s ", tmp0, tmp1, tmp2, tmp3);
				}
				else if (OpenGL::s_extension[Extension::NVX_gpu_memory_info].m_supported)
				{
					GLint dedicated;
					GL_CHECK(glGetIntegerv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &dedicated) );

					GLint totalAvail;
					GL_CHECK(glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &totalAvail) );
					GLint currAvail;
					GL_CHECK(glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &currAvail) );

					GLint evictedCount;
					GL_CHECK(glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTION_COUNT_NVX, &evictedCount) );

					GLint evictedMemory;
					GL_CHECK(glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX, &evictedMemory) );

					pos++;

					char tmp0[16];
					char tmp1[16];

					bx::prettify(tmp0, BX_COUNTOF(tmp0), dedicated);
					tvm.printf(10, pos++, 0x8b, " Dedicated: %10s ", tmp0);

					bx::prettify(tmp0, BX_COUNTOF(tmp0), currAvail);
					bx::prettify(tmp1, BX_COUNTOF(tmp1), totalAvail);
					tvm.printf(10, pos++, 0x8b, " Available: %10s / %10s ", tmp0, tmp1);

					bx::prettify(tmp0, BX_COUNTOF(tmp0), evictedCount);
					bx::prettify(tmp1, BX_COUNTOF(tmp1), evictedMemory);
					tvm.printf(10, pos++, 0x8b, "  Eviction: %10s / %10s ", tmp0, tmp1);
				}
#endif // BGFX_CONFIG_RENDERER_OPENGL

				pos++;
				double captureMs = double(captureElapsed)*toMs;
				tvm.printf(10, pos++, 0x8b, "     Capture: %7.4f [ms] ", captureMs);

				uint8_t attr[2] = { 0x8c, 0x8a };
				uint8_t attrIndex = _render->m_waitSubmit < _render->m_waitRender;
				tvm.printf(10, pos++, attr[attrIndex&1], " Submit wait: %7.4f [ms] ", double(_render->m_waitSubmit)*toMs);
				tvm.printf(10, pos++, attr[(attrIndex+1)&1], " Render wait: %7.4f [ms] ", double(_render->m_waitRender)*toMs);

				min = frameTime;
				max = frameTime;
			}

			blit(this, _textVideoMemBlitter, tvm);

			BGFX_GL_PROFILER_END();
		}
		else if (_render->m_debug & BGFX_DEBUG_TEXT)
		{
			BGFX_GL_PROFILER_BEGIN_LITERAL("debugtext", kColorFrame);

			blit(this, _textVideoMemBlitter, _render->m_textVideoMem);

			BGFX_GL_PROFILER_END();
		}

		//make sure sync for multi-thread.
		glFinish();
	}
} } // namespace bgfx

#else

namespace bgfx { namespace gl
{
	RendererContextI* rendererCreate(const Init& _init)
	{
		BX_UNUSED(_init);
		return NULL;
	}

	void rendererDestroy()
	{
	}
} /* namespace gl */ } // namespace bgfx

#endif // (BGFX_CONFIG_RENDERER_OPENGLES || BGFX_CONFIG_RENDERER_OPENGL)
