#include "../bgfx_p.h"

#if (BGFX_CONFIG_RENDERER_OPENGLES || BGFX_CONFIG_RENDERER_OPENGL)
#	include "OpenGLDrv.h"
#	include <bx/timer.h>
#	include <bx/uint32_t.h>
#	include "../emscripten.h"

namespace bgfx { namespace gl
{
    void FrameBufferGL::create(uint8_t _num, const Attachment* _attachment)
	{
		GL_CHECK(glGenFramebuffers(1, &m_fbo[0]) );

		m_denseIdx = UINT16_MAX;
		m_numTh = _num;
		bx::memCopy(m_attachment, _attachment, _num*sizeof(Attachment) );

		m_needPresent = false;

		postReset();
	}

	void FrameBufferGL::postReset()
	{
		if (0 != m_fbo[0])
		{
			GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m_fbo[0]) );

			bool needResolve = false;

			GLenum buffers[BGFX_CONFIG_MAX_FRAME_BUFFER_ATTACHMENTS];

			uint32_t colorIdx = 0;
			for (uint32_t ii = 0; ii < m_numTh; ++ii)
			{
				const Attachment& at = m_attachment[ii];

				if (isValid(at.handle) )
				{
					const TextureGL& texture = s_renderGL->m_textures[at.handle.idx];

					if (0 == colorIdx)
					{
						m_width  = bx::uint32_max(texture.m_width  >> at.mip, 1);
						m_height = bx::uint32_max(texture.m_height >> at.mip, 1);
					}

					GLenum attachment = GL_COLOR_ATTACHMENT0 + colorIdx;
					bimg::TextureFormat::Enum format = bimg::TextureFormat::Enum(texture.m_textureFormat);
					if (bimg::isDepth(format) )
					{
						const bimg::ImageBlockInfo& info = bimg::getBlockInfo(format);
						if (0 < info.stencilBits)
						{
							attachment = GL_DEPTH_STENCIL_ATTACHMENT;
						}
						else if (0 == info.depthBits)
						{
							attachment = GL_STENCIL_ATTACHMENT;
						}
						else
						{
							attachment = GL_DEPTH_ATTACHMENT;
						}
					}
					else if (Access::Write == at.access)
					{
						buffers[colorIdx] = attachment;
						++colorIdx;
					}

					if (0 != texture.m_rbo)
					{
						if (!(BX_ENABLED(BGFX_CONFIG_RENDERER_OPENGL >= 30) || OpenGL::Support_GLES3_0())
							&& GL_DEPTH_STENCIL_ATTACHMENT == attachment)
						{
							GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER
								, GL_DEPTH_ATTACHMENT
								, GL_RENDERBUFFER
								, texture.m_rbo
								) );
							GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER
								, GL_STENCIL_ATTACHMENT
								, GL_RENDERBUFFER
								, texture.m_rbo
								) );
						}
						else
						{
							GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER
								, attachment
								, GL_RENDERBUFFER
								, texture.m_rbo
								) );
						}
					}
					else
					{
						if (1 < texture.m_numLayers
						&&  !texture.isCubeMap() )
						{
							if (1 < at.numLayers)
							{
								BX_ASSERT(0 == at.layer, "Can't use start layer > 0 when binding multiple layers to a framebuffer.");

								GL_CHECK(glFramebufferTexture(GL_FRAMEBUFFER
									, attachment
									, texture.m_id
									, at.mip
									) );
							}
							else
							{
								GL_CHECK(glFramebufferTextureLayer(GL_FRAMEBUFFER
									, attachment
									, texture.m_id
									, at.mip
									, at.layer
									) );
							}
						}
						else
						{
							GLenum target = texture.isCubeMap()
								? GL_TEXTURE_CUBE_MAP_POSITIVE_X + at.layer
								: texture.m_target
								;

							GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER
								, attachment
								, target
								, texture.m_id
								, at.mip
								) );
						}
					}

					needResolve |= (0 != texture.m_rbo) && (0 != texture.m_id);
				}
			}

			m_num = uint8_t(colorIdx);

			if (0 == colorIdx && BX_ENABLED(BGFX_CONFIG_RENDERER_OPENGL) )
			{
				// When only depth is attached disable draw buffer to avoid
				// GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER.
				GL_CHECK(glDrawBuffer(GL_NONE) );
			}
			else if (g_caps.limits.maxFBAttachments > 0)
			{
				GL_CHECK(glDrawBuffers(colorIdx, buffers) );
			}

			if (BX_ENABLED(BGFX_CONFIG_RENDERER_OPENGL) || OpenGL::Support_GLES3_0())
			{
				// Disable read buffer to avoid GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER.
				GL_CHECK(glReadBuffer(GL_NONE) );
			}

			OpenGL::frameBufferValidate();

			if (needResolve)
			{
				GL_CHECK(glGenFramebuffers(1, &m_fbo[1]) );
				GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m_fbo[1]) );

				colorIdx = 0;
				for (uint32_t ii = 0; ii < m_numTh; ++ii)
				{
					const Attachment& at = m_attachment[ii];

					if (isValid(at.handle) )
					{
						const TextureGL& texture = s_renderGL->m_textures[at.handle.idx];

						if (0 != texture.m_id)
						{

							GLenum attachment = GL_INVALID_ENUM;
							bimg::TextureFormat::Enum format = bimg::TextureFormat::Enum(texture.m_textureFormat);
							if (bimg::isDepth(format) )
							{
								const bimg::ImageBlockInfo& info = bimg::getBlockInfo(format);
								if (0 < info.stencilBits)
								{
									attachment = GL_DEPTH_STENCIL_ATTACHMENT;
								}
								else if (0 == info.depthBits)
								{
									attachment = GL_STENCIL_ATTACHMENT;
								}
								else
								{
									attachment = GL_DEPTH_ATTACHMENT;
								}
							} else {
								attachment = GL_COLOR_ATTACHMENT0 + colorIdx;
								++colorIdx;
							}

							GLenum target = texture.isCubeMap()
								? GL_TEXTURE_CUBE_MAP_POSITIVE_X + at.layer
								: texture.m_target
								;

							GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER
								, attachment
								, target
								, texture.m_id
								, at.mip
								) );
						}
					}
				}

				OpenGL::frameBufferValidate();
			}

			GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, s_renderGL->m_msaaBackBufferFbo) );
		}
	}

	void FrameBufferGL::create(uint16_t _denseIdx, void* _nwh, uint32_t _width, uint32_t _height, TextureFormat::Enum _format, TextureFormat::Enum _depthFormat)
	{
		BX_UNUSED(_format, _depthFormat);
		m_swapChain = s_renderGL->m_glctx.createSwapChain(_nwh);
		m_width     = _width;
		m_height    = _height;
		m_numTh     = 0;
		m_denseIdx  = _denseIdx;
		m_needPresent = false;
	}

	void FrameBufferGL::create(int32_t refHandle, uint32_t _width, uint32_t _height, TextureFormat::Enum _format, TextureFormat::Enum _depthFormat)
	{
		BX_UNUSED(_format, _depthFormat);

		m_refHandle = refHandle;
		m_width = _width;
		m_height = _height;
		m_numTh = 0;
		m_denseIdx = UINT16_MAX;
		m_needPresent = false;

		if (m_refHandle >= 0)
		{
			m_fbo[0] = refHandle;
		}
	}

	uint16_t FrameBufferGL::destroy()
	{
		if (m_refHandle < 0)
		{
			if (0 != m_num)
			{
				GL_CHECK(glDeleteFramebuffers(0 == m_fbo[1] ? 1 : 2, m_fbo));
				m_num = 0;
			}

			if (NULL != m_swapChain)
			{
				s_renderGL->m_glctx.destroySwapChain(m_swapChain);
				m_swapChain = NULL;
			}
		}

		bx::memSet(m_fbo, 0, sizeof(m_fbo) );
		uint16_t denseIdx = m_denseIdx;
		m_denseIdx = UINT16_MAX;
		m_needPresent = false;
		m_numTh = 0;

		return denseIdx;
	}

	void FrameBufferGL::resolve()
	{
		if (0 != m_fbo[1])
		{
			uint32_t colorIdx = 0;
			for (uint32_t ii = 0; ii < m_numTh; ++ii)
			{
				const Attachment& at = m_attachment[ii];

				if (isValid(at.handle) )
				{
					const TextureGL& texture = s_renderGL->m_textures[at.handle.idx];

					const bool writeOnly = 0 != (texture.m_flags&BGFX_TEXTURE_RT_WRITE_ONLY);
					bimg::TextureFormat::Enum format = bimg::TextureFormat::Enum(texture.m_textureFormat);

					if (!bimg::isDepth(format) )
					{
						GL_CHECK(glDisable(GL_SCISSOR_TEST));
						GL_CHECK(glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo[0]) );
						GL_CHECK(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo[1]) );
						GL_CHECK(glReadBuffer(GL_COLOR_ATTACHMENT0 + colorIdx) );
						GL_CHECK(glDrawBuffer(GL_COLOR_ATTACHMENT0 + colorIdx) );
						colorIdx++;
						GL_CHECK(glBlitFramebuffer(0
							, 0
							, m_width
							, m_height
							, 0
							, 0
							, m_width
							, m_height
							, GL_COLOR_BUFFER_BIT
							, GL_LINEAR
							) );

					} else if (!writeOnly) {
						GL_CHECK(glDisable(GL_SCISSOR_TEST));
						// blit depth attachment as well if it doesn't have
						// BGFX_TEXTURE_RT_WRITE_ONLY render target flag. In most cases it's
						// not necessary to blit the depth buffer.
						GL_CHECK(glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo[0]) );
						GL_CHECK(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo[1]) );
						// OpenGL complains about missing buffer if set
						// attachment. not sure what I'm missing...
						// GL_CHECK(glReadBuffer(GL_DEPTH_ATTACHMENT) );
						// GL_CHECK(glDrawBuffer(GL_DEPTH_ATTACHMENT) );
						GL_CHECK(glBlitFramebuffer(0
							, 0
							, m_width
							, m_height
							, 0
							, 0
							, m_width
							, m_height
							, GL_DEPTH_BUFFER_BIT
							, GL_NEAREST
							) );
					}
				}
			}

			GL_CHECK(glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo[0]) );
			if (BX_ENABLED(BGFX_CONFIG_RENDERER_OPENGL) || OpenGL::Support_GLES3_0())
			{
				GL_CHECK(glReadBuffer(GL_NONE) );
			}
			GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, s_renderGL->m_msaaBackBufferFbo) );
		}

		for (uint32_t ii = 0; ii < m_numTh; ++ii)
		{
			const Attachment& at = m_attachment[ii];

			if (isValid(at.handle) )
			{
				const TextureGL& texture = s_renderGL->m_textures[at.handle.idx];
				texture.resolve(at.resolve);
			}
		}
	}

	void FrameBufferGL::discard(uint16_t _flags)
	{
		GLenum buffers[BGFX_CONFIG_MAX_FRAME_BUFFER_ATTACHMENTS+2];
		uint32_t idx = 0;

		if (BGFX_CLEAR_NONE != (_flags & BGFX_CLEAR_DISCARD_COLOR_MASK) )
		{
			for (uint32_t ii = 0, num = m_num; ii < num; ++ii)
			{
				if (BGFX_CLEAR_NONE != (_flags & (BGFX_CLEAR_DISCARD_COLOR_0<<ii) ) )
				{
					buffers[idx++] = GL_COLOR_ATTACHMENT0 + ii;
				}
			}
		}

		uint32_t dsFlags = _flags & (BGFX_CLEAR_DISCARD_DEPTH|BGFX_CLEAR_DISCARD_STENCIL);
		if (BGFX_CLEAR_NONE != dsFlags)
		{
			if (!Platform::IsMobilePlatform()
			&&  (BGFX_CLEAR_DISCARD_DEPTH|BGFX_CLEAR_DISCARD_STENCIL) == dsFlags)
			{
				buffers[idx++] = GL_DEPTH_STENCIL_ATTACHMENT;
			}
			else if (BGFX_CLEAR_DISCARD_DEPTH == dsFlags)
			{
				buffers[idx++] = GL_DEPTH_ATTACHMENT;
			}
			else if (BGFX_CLEAR_DISCARD_STENCIL == dsFlags)
			{
				buffers[idx++] = GL_STENCIL_ATTACHMENT;
			}
		}

		GL_CHECK(OpenGL::InvalidateFramebuffer(GL_FRAMEBUFFER, idx, buffers) );
	}
    
    

} } // namespace bgfx

#else

namespace bgfx { namespace gl
{
	
} /* namespace gl */ } // namespace bgfx

#endif // (BGFX_CONFIG_RENDERER_OPENGLES || BGFX_CONFIG_RENDERER_OPENGL)
