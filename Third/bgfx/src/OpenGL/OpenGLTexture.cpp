#include "../bgfx_p.h"

#if (BGFX_CONFIG_RENDERER_OPENGLES || BGFX_CONFIG_RENDERER_OPENGL)
#	include "OpenGLDrv.h"
#	include <bx/timer.h>
#	include <bx/uint32_t.h>
#	include "../emscripten.h"

namespace bgfx { namespace gl
{
	bool TextureGL::init(GLenum _target, uint32_t _width, uint32_t _height, uint32_t _depth, uint8_t _numMips, uint64_t _flags, bool autoGenMipmap, int32_t refHandle)
	{
		m_target = _target;
		m_numMips = _numMips;
		m_flags = _flags;
		m_width = _width;
		m_height = _height;
		m_depth = _depth;
		m_currentSamplerHash = UINT32_MAX;
		m_autoGenMipmap = autoGenMipmap;
		m_refHandle = refHandle;

		const bool writeOnly = 0 != (m_flags & BGFX_TEXTURE_RT_WRITE_ONLY);
		const bool computeWrite = 0 != (m_flags & BGFX_TEXTURE_COMPUTE_WRITE);
		const bool srgb = 0 != (m_flags & BGFX_TEXTURE_SRGB);
		const bool renderTarget = 0 != (m_flags & BGFX_TEXTURE_RT_MASK);
		const bool textureArray = false
			|| _target == GL_TEXTURE_2D_ARRAY
			|| _target == GL_TEXTURE_CUBE_MAP_ARRAY
			;
		const bool isRefHandle = (m_refHandle > 0);

		if (!writeOnly
			|| (renderTarget && textureArray))
		{
			if (isRefHandle)
			{
				m_id = m_refHandle;
				GL_CHECK(glBindTexture(_target, m_id));
				return false;
			}
			else
			{
				GL_CHECK(glGenTextures(1, &m_id));
				BX_ASSERT(0 != m_id, "Failed to generate texture id.");
				GL_CHECK(glBindTexture(_target, m_id));
				GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
			}
			GL_CHECK(glGenTextures(1, &m_id));
			BX_ASSERT(0 != m_id, "Failed to generate texture id.");
			GL_CHECK(glBindTexture(_target, m_id));
			GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

			const TextureFormatInfo& tfi = OpenGLBase::s_textureFormat[m_textureFormat];

			const GLenum fmt = srgb
				? OpenGLBase::s_textureFormat[m_textureFormat].m_fmtSrgb
				: OpenGLBase::s_textureFormat[m_textureFormat].m_fmt
				;

			m_fmt = fmt;
			m_type = tfi.m_type;

			const bool swizzle = true
				&& TextureFormat::BGRA8 == m_requestedFormat
				&& !OpenGLBase::s_textureFormat[m_requestedFormat].m_supported
				&& !s_renderGL->m_textureSwizzleSupport
				;
			const bool convert = false
				|| m_textureFormat != m_requestedFormat
				|| swizzle
				;

			if (convert)
			{
				m_textureFormat = (uint8_t)TextureFormat::RGBA8;
				const TextureFormatInfo& tfiRgba8 = OpenGLBase::s_textureFormat[TextureFormat::RGBA8];
				m_fmt = tfiRgba8.m_fmt;
				m_type = tfiRgba8.m_type;
			}

			const GLenum internalFmt = srgb
				? OpenGLBase::s_textureFormat[m_textureFormat].m_internalFmtSrgb
				: OpenGLBase::s_textureFormat[m_textureFormat].m_internalFmt
				;

			if (textureArray)
			{
				GL_CHECK(glTexStorage3D(_target
					, _numMips
					, internalFmt
					, m_width
					, m_height
					, _depth
				));
			}
			else if (computeWrite)
			{
				if (_target == GL_TEXTURE_3D)
				{
					GL_CHECK(glTexStorage3D(_target
						, _numMips
						, internalFmt
						, m_width
						, m_height
						, _depth
					));
				}
				else
				{
					GL_CHECK(glTexStorage2D(_target
						, _numMips
						, internalFmt
						, m_width
						, m_height
					));
				}
			}

			setSamplerState(uint32_t(_flags), NULL);

			if (BX_ENABLED(BGFX_CONFIG_RENDERER_OPENGL)
				&& TextureFormat::BGRA8 == m_requestedFormat
				&& !OpenGLBase::s_textureFormat[m_requestedFormat].m_supported
				&& s_renderGL->m_textureSwizzleSupport)
			{
				GLint swizzleMask[] = { GL_BLUE, GL_GREEN, GL_RED, GL_ALPHA };
				GL_CHECK(glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask));
			}
		}

		if (renderTarget && !isRefHandle)
		{
			uint32_t msaaQuality = ((m_flags & BGFX_TEXTURE_RT_MSAA_MASK) >> BGFX_TEXTURE_RT_MSAA_SHIFT);
			msaaQuality = bx::uint32_satsub(msaaQuality, 1);
			msaaQuality = bx::uint32_min(s_renderGL->m_maxMsaa, msaaQuality == 0 ? 0 : 1 << msaaQuality);
			const bool msaaSample = 0 != (m_flags & BGFX_TEXTURE_MSAA_SAMPLE);

			if (!msaaSample
				&& (0 != msaaQuality || writeOnly)
				&& !textureArray)
			{
				GL_CHECK(glGenRenderbuffers(1, &m_rbo));
				BX_ASSERT(0 != m_rbo, "Failed to generate renderbuffer id.");
				GL_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, m_rbo));

				if (0 == msaaQuality)
				{
					GL_CHECK(glRenderbufferStorage(GL_RENDERBUFFER
						, OpenGLBase::s_rboFormat[m_textureFormat]
						, _width
						, _height
					));
				}
				else if (BX_ENABLED(BGFX_CONFIG_RENDERER_OPENGL) || OpenGL::Support_GLES3_0())
				{
					GL_CHECK(glRenderbufferStorageMultisample(GL_RENDERBUFFER
						, msaaQuality
						, OpenGLBase::s_rboFormat[m_textureFormat]
						, _width
						, _height
					));
				}

				GL_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, 0));

				if (writeOnly)
				{
					// This is render buffer, there is no sampling, no need
					// to create texture.
					return false;
				}
			}
		}

		return true;
	}

	void TextureGL::create(const Memory* _mem, uint64_t _flags, uint8_t _skip)
	{
		bimg::ImageContainer imageContainer;

		if (bimg::imageParse(imageContainer, _mem->data, _mem->size))
		{
			const uint8_t startLod = bx::min<uint8_t>(_skip, imageContainer.m_numMips - 1);

			bimg::TextureInfo ti;
			bimg::imageGetSize(
				&ti
				, uint16_t(imageContainer.m_width >> startLod)
				, uint16_t(imageContainer.m_height >> startLod)
				, uint16_t(imageContainer.m_depth >> startLod)
				, imageContainer.m_cubeMap
				, 1 < imageContainer.m_numMips
				, imageContainer.m_numLayers
				, imageContainer.m_format
				, imageContainer.m_autoGenMipmap
				, imageContainer.m_refHandle
			);
			ti.numMips = bx::min<uint8_t>(imageContainer.m_numMips - startLod, ti.numMips);

			m_requestedFormat = uint8_t(imageContainer.m_format);
			m_textureFormat = uint8_t(getViableTextureFormat(imageContainer));

			const bool computeWrite = 0 != (_flags & BGFX_TEXTURE_COMPUTE_WRITE);
			const bool srgb = 0 != (_flags & BGFX_TEXTURE_SRGB);
			const bool msaaSample = 0 != (_flags & BGFX_TEXTURE_MSAA_SAMPLE);
			uint32_t msaaQuality = ((_flags & BGFX_TEXTURE_RT_MSAA_MASK) >> BGFX_TEXTURE_RT_MSAA_SHIFT);
			msaaQuality = bx::uint32_satsub(msaaQuality, 1);
			msaaQuality = bx::uint32_min(s_renderGL->m_maxMsaa, msaaQuality == 0 ? 0 : 1 << msaaQuality);

			GLenum target = msaaSample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
			if (imageContainer.m_cubeMap)
			{
				target = GL_TEXTURE_CUBE_MAP;
			}
			else if (imageContainer.m_depth > 1)
			{
				target = GL_TEXTURE_3D;
			}

			const bool textureArray = 1 < ti.numLayers;
			if (textureArray)
			{
				switch (target)
				{
				case GL_TEXTURE_CUBE_MAP:       target = GL_TEXTURE_CUBE_MAP_ARRAY;       break;
				case GL_TEXTURE_2D_MULTISAMPLE: target = GL_TEXTURE_2D_MULTISAMPLE_ARRAY; break;
				default:                        target = GL_TEXTURE_2D_ARRAY;             break;
				}
			}

			m_numLayers = ti.numLayers;

			if (!init(target
				, ti.width
				, ti.height
				, textureArray ? ti.numLayers : ti.depth
				, ti.numMips
				, _flags
				, ti.autoGenMaipmap
				, ti.refHandle
			))
			{
				return;
			}

			target = isCubeMap()
				? GL_TEXTURE_CUBE_MAP_POSITIVE_X
				: m_target
				;

			const GLenum internalFmt = srgb
				? OpenGLBase::s_textureFormat[m_textureFormat].m_internalFmtSrgb
				: OpenGLBase::s_textureFormat[m_textureFormat].m_internalFmt
				;
			const GLenum fmt = srgb
				? OpenGLBase::s_textureFormat[m_textureFormat].m_fmtSrgb
				: OpenGLBase::s_textureFormat[m_textureFormat].m_fmt
				;

			const bool swizzle = true
				&& TextureFormat::BGRA8 == m_requestedFormat
				&& !OpenGLBase::s_textureFormat[m_requestedFormat].m_supported
				&& !s_renderGL->m_textureSwizzleSupport
				;
			const bool compressed = bimg::isCompressed(bimg::TextureFormat::Enum(m_requestedFormat));
			const bool convert = false
				|| m_textureFormat != m_requestedFormat
				|| swizzle
				;

			BX_TRACE("Texture%-4s %3d: %s %s(requested: %s), layers %d, %dx%dx%d%s."
				, imageContainer.m_cubeMap ? "Cube" : (1 < imageContainer.m_depth ? "3D" : "2D")
				, this - s_renderGL->m_textures
				, getName((TextureFormat::Enum)m_textureFormat)
				, srgb ? "+sRGB " : ""
				, getName((TextureFormat::Enum)m_requestedFormat)
				, ti.numLayers
				, ti.width
				, ti.height
				, imageContainer.m_cubeMap ? 6 : (1 < imageContainer.m_depth ? imageContainer.m_depth : 0)
				, 0 != (m_flags & BGFX_TEXTURE_RT_MASK) ? " (render target)" : ""
			);

			BX_WARN(!convert, "Texture %s%s%s from %s to %s."
				, swizzle ? "swizzle" : ""
				, swizzle && convert ? " and " : ""
				, convert ? "convert" : ""
				, getName((TextureFormat::Enum)m_requestedFormat)
				, getName((TextureFormat::Enum)m_textureFormat)
			);

			uint8_t* temp = NULL;
			if (convert)
			{
				temp = (uint8_t*)BX_ALLOC(g_allocator, ti.width * ti.height * 4);
			}

			const uint16_t numSides = ti.numLayers * (imageContainer.m_cubeMap ? 6 : 1);

			for (uint16_t side = 0; side < numSides; ++side)
			{
				uint32_t width = ti.width;
				uint32_t height = ti.height;
				uint32_t depth = ti.depth;
				GLenum imageTarget = imageContainer.m_cubeMap && !textureArray
					? target + side
					: target
					;

				for (uint8_t lod = 0, num = ti.numMips; lod < num; ++lod)
				{
					width = bx::uint32_max(1, width);
					height = bx::uint32_max(1, height);
					depth = 1 < imageContainer.m_depth
						? bx::uint32_max(1, depth)
						: side
						;

					bimg::ImageMip mip;
					if (bimg::imageGetRawData(imageContainer, side, lod + startLod, _mem->data, _mem->size, mip))
					{
						if (compressed
							&& !convert)
						{
							GL_CHECK(OpenGLBase::compressedTexImage(imageTarget
								, lod
								, internalFmt
								, width
								, height
								, depth
								, 0
								, mip.m_size
								, mip.m_data
							));
						}
						else
						{
							const uint8_t* data = mip.m_data;

							if (convert)
							{
								imageDecodeToRgba8(
									g_allocator
									, temp
									, mip.m_data
									, mip.m_width
									, mip.m_height
									, mip.m_width * 4
									, mip.m_format
								);
								data = temp;
							}

							GL_CHECK(OpenGLBase::texImage(imageTarget
								, msaaQuality
								, lod
								, internalFmt
								, width
								, height
								, depth
								, 0
								, fmt
								, m_type
								, data
							));
						}
					}
					else if (!computeWrite)
					{
						if (compressed
							&& !convert)
						{
							uint32_t size = bx::max<uint32_t>(1, (width + 3) >> 2)
								* bx::max<uint32_t>(1, (height + 3) >> 2)
								* 4 * 4 * bimg::getBitsPerPixel(bimg::TextureFormat::Enum(m_textureFormat)) / 8
								;

							GL_CHECK(OpenGLBase::compressedTexImage(imageTarget
								, lod
								, internalFmt
								, width
								, height
								, depth
								, 0
								, size
								, NULL
							));
						}
						else
						{
							GL_CHECK(OpenGLBase::texImage(imageTarget
								, msaaQuality
								, lod
								, internalFmt
								, width
								, height
								, depth
								, 0
								, fmt
								, m_type
								, NULL
							));
						}
					}

					width >>= 1;
					height >>= 1;
					depth >>= 1;
				}
			}

			if (NULL != temp)
			{
				BX_FREE(g_allocator, temp);
			}
		}

		if (m_autoGenMipmap)
		{
			glGenerateMipmap(m_target);
		}

		GL_CHECK(glBindTexture(m_target, 0));
	}

	void TextureGL::destroy()
	{
		bool bRefHandle = (m_refHandle > 0);
		if (bRefHandle)
		{
			m_refHandle = -1;
			m_rbo = 0;
			m_id = 0;
			return;
		}
		if (0 == (m_flags & BGFX_SAMPLER_INTERNAL_SHARED)
			&& 0 != m_id)
		{
			GL_CHECK(glBindTexture(m_target, 0));
			GL_CHECK(glDeleteTextures(1, &m_id));
			m_id = 0;
		}

		if (0 != m_rbo)
		{
			GL_CHECK(glDeleteRenderbuffers(1, &m_rbo));
			m_rbo = 0;
		}
	}

	void TextureGL::overrideInternal(uintptr_t _ptr)
	{
		destroy();
		m_flags |= BGFX_SAMPLER_INTERNAL_SHARED;
		m_id = (GLuint)_ptr;
	}

	void TextureGL::update(uint8_t _side, uint8_t _mip, const Rect& _rect, uint16_t _z, uint16_t _depth, uint16_t _pitch, const Memory* _mem)
	{
		const uint32_t bpp = bimg::getBitsPerPixel(bimg::TextureFormat::Enum(m_textureFormat));
		const uint32_t rectpitch = _rect.m_width * bpp / 8;
		uint32_t srcpitch = UINT16_MAX == _pitch ? rectpitch : _pitch;

		GL_CHECK(glBindTexture(m_target, m_id));
		GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

		GLenum target = isCubeMap()
			? GL_TEXTURE_CUBE_MAP_POSITIVE_X
			: m_target
			;

		const bool swizzle = true
			&& TextureFormat::BGRA8 == m_requestedFormat
			&& !OpenGLBase::s_textureFormat[m_requestedFormat].m_supported
			&& !s_renderGL->m_textureSwizzleSupport
			;
		const bool unpackRowLength = !!BGFX_CONFIG_RENDERER_OPENGL || OpenGLBase::s_extension[Extension::EXT_unpack_subimage].m_supported;
		const bool compressed = bimg::isCompressed(bimg::TextureFormat::Enum(m_requestedFormat));
		const bool convert = false
			|| (compressed && m_textureFormat != m_requestedFormat)
			|| swizzle
			;

		Rect rect;
		rect.setIntersect(_rect
			, {
				0, 0,
				uint16_t(bx::max(1u, m_width >> _mip)),
				uint16_t(bx::max(1u, m_height >> _mip)),
			});

		uint32_t width = rect.m_width;
		uint32_t height = rect.m_height;

		uint8_t* temp = NULL;
		if (convert
			|| !unpackRowLength)
		{
			temp = (uint8_t*)BX_ALLOC(g_allocator, rectpitch * height);
		}
		else if (unpackRowLength)
		{
			GL_CHECK(glPixelStorei(GL_UNPACK_ROW_LENGTH, srcpitch * 8 / bpp));
		}

		if (compressed
			&& !convert)
		{
			const uint8_t* data = _mem->data;

			if (!unpackRowLength)
			{
				bimg::imageCopy(temp, width, height, 1, bpp, srcpitch, data);
				data = temp;
			}
			const GLenum internalFmt = (0 != (m_flags & BGFX_TEXTURE_SRGB))
				? OpenGLBase::s_textureFormat[m_textureFormat].m_internalFmtSrgb
				: OpenGLBase::s_textureFormat[m_textureFormat].m_internalFmt
				;
			GL_CHECK(OpenGLBase::compressedTexSubImage(target + _side
				, _mip
				, rect.m_x
				, rect.m_y
				, _z
				, rect.m_width
				, rect.m_height
				, _depth
				, internalFmt
				, _mem->size
				, data
			));
		}
		else
		{
			const uint8_t* data = _mem->data;

			if (convert)
			{
				bimg::imageDecodeToRgba8(g_allocator, temp, data, width, height, srcpitch, bimg::TextureFormat::Enum(m_requestedFormat));
				data = temp;
				srcpitch = rectpitch;
			}

			if (BX_IGNORE_C4127(true
				&& !unpackRowLength
				&& !convert))
			{
				bimg::imageCopy(temp, width, height, 1, bpp, srcpitch, data);
				data = temp;
			}

			GL_CHECK(OpenGLBase::texSubImage(target + _side
				, _mip
				, rect.m_x
				, rect.m_y
				, _z
				, rect.m_width
				, rect.m_height
				, _depth
				, m_fmt
				, m_type
				, data
			));
		}

		if (!convert
			&& unpackRowLength)
		{
			GL_CHECK(glPixelStorei(GL_UNPACK_ROW_LENGTH, 0));
		}

		if (m_autoGenMipmap)
		{
			glGenerateMipmap(m_target);
		}

		if (NULL != temp)
		{
			BX_FREE(g_allocator, temp);
		}
	}

	void TextureGL::setSamplerState(uint32_t _flags, const float _rgba[4])
	{
		if (!OpenGL::Support_GLES3_0()
			&& !OpenGLBase::s_textureFilter[m_textureFormat])
		{
			// Force point sampling when texture format doesn't support linear sampling.
			_flags &= ~(0
				| BGFX_SAMPLER_MIN_MASK
				| BGFX_SAMPLER_MAG_MASK
				| BGFX_SAMPLER_MIP_MASK
				);
			_flags |= 0
				| BGFX_SAMPLER_MIN_POINT
				| BGFX_SAMPLER_MAG_POINT
				| BGFX_SAMPLER_MIP_POINT
				;
		}

		const uint32_t flags = (0 != (BGFX_SAMPLER_INTERNAL_DEFAULT & _flags) ? m_flags : _flags) & BGFX_SAMPLER_BITS_MASK;

		bool hasBorderColor = false;
		bx::HashMurmur2A murmur;
		murmur.begin();
		murmur.add(flags);
		if (NULL != _rgba)
		{
			if (BGFX_SAMPLER_U_BORDER == (flags & BGFX_SAMPLER_U_BORDER)
				|| BGFX_SAMPLER_V_BORDER == (flags & BGFX_SAMPLER_V_BORDER)
				|| BGFX_SAMPLER_W_BORDER == (flags & BGFX_SAMPLER_W_BORDER))
			{
				murmur.add(_rgba, 16);
				hasBorderColor = true;
			}
		}
		uint32_t hash = murmur.end();

		if (hash != m_currentSamplerHash)
		{
			const GLenum  target = m_target == GL_TEXTURE_2D_MULTISAMPLE ? GL_TEXTURE_2D : m_target;
			const GLenum  targetMsaa = m_target;
			const uint8_t numMips = m_numMips;

			GL_CHECK(glTexParameteri(target, GL_TEXTURE_WRAP_S, OpenGLBase::s_textureAddress[(flags & BGFX_SAMPLER_U_MASK) >> BGFX_SAMPLER_U_SHIFT]));
			GL_CHECK(glTexParameteri(target, GL_TEXTURE_WRAP_T, OpenGLBase::s_textureAddress[(flags & BGFX_SAMPLER_V_MASK) >> BGFX_SAMPLER_V_SHIFT]));

			if (BX_ENABLED(BGFX_CONFIG_RENDERER_OPENGL) || OpenGL::Support_GLES3_0()
				|| OpenGLBase::s_extension[Extension::APPLE_texture_max_level].m_supported)
			{
				GL_CHECK(glTexParameteri(targetMsaa, GL_TEXTURE_MAX_LEVEL, numMips - 1));
			}

			if (target == GL_TEXTURE_3D)
			{
				GL_CHECK(glTexParameteri(target, GL_TEXTURE_WRAP_R, OpenGLBase::s_textureAddress[(flags & BGFX_SAMPLER_W_MASK) >> BGFX_SAMPLER_W_SHIFT]));
			}

			GLenum magFilter;
			GLenum minFilter;
			OpenGLBase::getFilters(flags, 1 < numMips, magFilter, minFilter);
			GL_CHECK(glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter));
			GL_CHECK(glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter));

			if (BX_ENABLED(BGFX_CONFIG_RENDERER_OPENGL))
			{
				GL_CHECK(glTexParameterf(target, GL_TEXTURE_LOD_BIAS, float(BGFX_CONFIG_MIP_LOD_BIAS)));
			}

			if (s_renderGL->m_borderColorSupport
				&& hasBorderColor)
			{
				GL_CHECK(glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, _rgba));
			}

			if (0 != (flags & (BGFX_SAMPLER_MIN_ANISOTROPIC | BGFX_SAMPLER_MAG_ANISOTROPIC))
				&& 0.0f < s_renderGL->m_maxAnisotropy)
			{
				GL_CHECK(glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, s_renderGL->m_maxAnisotropy));
			}

			if (OpenGL::Support_GLES3_0()
				|| s_renderGL->m_shadowSamplersSupport)
			{
				const uint32_t cmpFunc = (flags & BGFX_SAMPLER_COMPARE_MASK) >> BGFX_SAMPLER_COMPARE_SHIFT;
				if (0 == cmpFunc)
				{
					GL_CHECK(glTexParameteri(target, GL_TEXTURE_COMPARE_MODE, GL_NONE));
				}
				else
				{
					GL_CHECK(glTexParameteri(target, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE));
					GL_CHECK(glTexParameteri(target, GL_TEXTURE_COMPARE_FUNC, OpenGLBase::s_cmpFunc[cmpFunc]));
				}
			}

			m_currentSamplerHash = hash;
		}
	}

	void TextureGL::commit(uint32_t _stage, uint32_t _flags, const float _palette[][4])
	{
		const uint32_t flags = 0 == (BGFX_SAMPLER_INTERNAL_DEFAULT & _flags)
			? _flags
			: uint32_t(m_flags)
			;
		const uint32_t index = (flags & BGFX_SAMPLER_BORDER_COLOR_MASK) >> BGFX_SAMPLER_BORDER_COLOR_SHIFT;

		GL_CHECK(glActiveTexture(GL_TEXTURE0 + _stage));
		GL_CHECK(glBindTexture(m_target, m_id));

		if (m_refHandle > 0)
		{
			if (OpenGL::Support_GLES3_0())
			{
				GL_CHECK(glBindSampler(_stage, 0));
			}
			return;
		}

		if (Platform::IsMobilePlatform()
			&& !OpenGL::Support_GLES3_0())
		{
			// GLES2 doesn't have support for sampler object.
			setSamplerState(flags, _palette[index]);
		}
		else if (BX_ENABLED(BGFX_CONFIG_RENDERER_OPENGL)
			&& BX_ENABLED(BGFX_CONFIG_RENDERER_OPENGL < 31))
		{
			// In case that GL 2.1 sampler object is supported via extension.
			if (s_renderGL->m_samplerObjectSupport)
			{
				s_renderGL->setSamplerState(_stage, m_numMips, flags, _palette[index]);
			}
			else
			{
				setSamplerState(flags, _palette[index]);
			}
		}
		else
		{
			// Everything else has sampler object.
			s_renderGL->setSamplerState(_stage, m_numMips, flags, _palette[index]);
		}
	}

	void TextureGL::resolve(uint8_t _resolve) const
	{
		const bool renderTarget = 0 != (m_flags & BGFX_TEXTURE_RT_MASK);
		if (renderTarget
			&& 1 < m_numMips
			&& 0 != (_resolve & BGFX_RESOLVE_AUTO_GEN_MIPS))
		{
			GL_CHECK(glBindTexture(m_target, m_id));
			GL_CHECK(glGenerateMipmap(m_target));
			GL_CHECK(glBindTexture(m_target, 0));
		}
	}
    
    

} } // namespace bgfx

#else

namespace bgfx { namespace gl
{
	
} /* namespace gl */ } // namespace bgfx

#endif // (BGFX_CONFIG_RENDERER_OPENGLES || BGFX_CONFIG_RENDERER_OPENGL)
