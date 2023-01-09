/*
 * Copyright 2011-2022 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx/blob/master/LICENSE
 */

#ifndef BGFX_RENDERER_GL_H_HEADER_GUARD
#define BGFX_RENDERER_GL_H_HEADER_GUARD

#include "OpenGLResource.h"

#define FORCE_SAMESTATE_MRT  1

namespace bgfx
{
	class UniformBuffer;
} // namespace bgfx

namespace bgfx { namespace gl
{
	void dumpExtensions(const char* _extensions);

	void lazyEnableVertexAttribArray(GLuint index);
	void lazyDisableVertexAttribArray(GLuint index);

	struct Workaround
	{
		void reset()
		{
			m_detachShader = true;
		}

		bool m_detachShader;
	};

	struct RendererContextGL : public RendererContextI
	{
		RendererContextGL();
		~RendererContextGL();

		bool init(const Init& _init);
		void shutdown();
		RendererType::Enum getRendererType() const override;
		DeviceFeatureLevel::Enum getDeviceFeatureLevel() const override;
		const char* getRendererName() const override;
		bool isDeviceRemoved() override;
		void flip() override;
		void createIndexBuffer(IndexBufferHandle _handle, const Memory* _mem, uint16_t _flags) override;
		void destroyIndexBuffer(IndexBufferHandle _handle) override;
		void createVertexLayout(VertexLayoutHandle _handle, const VertexLayout& _layout) override;
		void destroyVertexLayout(VertexLayoutHandle /*_handle*/) override;
		void createVertexBuffer(VertexBufferHandle _handle, const Memory* _mem, VertexLayoutHandle _layoutHandle, uint16_t _flags) override;
		void destroyVertexBuffer(VertexBufferHandle _handle) override;
		void createDynamicIndexBuffer(IndexBufferHandle _handle, uint32_t _size, uint16_t _flags) override;
		void updateDynamicIndexBuffer(IndexBufferHandle _handle, uint32_t _offset, uint32_t _size, const Memory* _mem) override;
		void destroyDynamicIndexBuffer(IndexBufferHandle _handle) override;
		void createDynamicVertexBuffer(VertexBufferHandle _handle, uint32_t _size, uint16_t _flags) override;
		void updateDynamicVertexBuffer(VertexBufferHandle _handle, uint32_t _offset, uint32_t _size, const Memory* _mem) override;
		void destroyDynamicVertexBuffer(VertexBufferHandle _handle) override;
		void createShader(ShaderHandle _handle, const Memory* _mem) override;
		void destroyShader(ShaderHandle _handle) override;
		void createProgram(ProgramHandle _handle, ShaderHandle _vsh, ShaderHandle _fsh) override;
		void destroyProgram(ProgramHandle _handle) override;
		void* createTexture(TextureHandle _handle, const Memory* _mem, uint64_t _flags, uint8_t _skip) override;
		void updateTextureBegin(TextureHandle /*_handle*/, uint8_t /*_side*/, uint8_t /*_mip*/) override;
		void updateTexture(TextureHandle _handle, uint8_t _side, uint8_t _mip, const Rect& _rect, uint16_t _z, uint16_t _depth, uint16_t _pitch, const Memory* _mem) override;
		void updateTextureEnd() override;
		void readTexture(TextureHandle _handle, void* _data, uint8_t _mip) override;
		void resizeTexture(TextureHandle _handle, uint16_t _width, uint16_t _height, uint8_t _numMips, uint16_t _numLayers, bool autoGenMipmap) override;
		void overrideInternal(TextureHandle _handle, uintptr_t _ptr) override;
		uintptr_t getInternal(TextureHandle _handle) override;
		void destroyTexture(TextureHandle _handle) override;
		void createFrameBuffer(FrameBufferHandle _handle, uint8_t _num, const Attachment* _attachment) override;
		void createFrameBuffer(FrameBufferHandle _handle, void* _nwh, uint32_t _width, uint32_t _height, TextureFormat::Enum _format, TextureFormat::Enum _depthFormat) override;
		void createFrameBuffer(FrameBufferHandle _handle, int32_t refHandle, uint32_t _width, uint32_t _height, TextureFormat::Enum _format, TextureFormat::Enum _depthFormat) override;
		void destroyFrameBuffer(FrameBufferHandle _handle) override;
		void createUniform(UniformHandle _handle, UniformType::Enum _type, uint16_t _num, const char* _name) override;
		void destroyUniform(UniformHandle _handle) override;
		void requestScreenShot(FrameBufferHandle _handle, const char* _filePath) override;
		void updateViewName(ViewId _id, const char* _name) override;
		void updateUniform(uint16_t _loc, const void* _data, uint32_t _size) override;
		void invalidateOcclusionQuery(OcclusionQueryHandle _handle) override;
		void setMarker(const char* _marker, uint16_t _len) override;
		virtual void setName(Handle _handle, const char* _name, uint16_t _len) override;
		void submitBlit(BlitState& _bs, uint16_t _view);
		void submit(Frame* _render, TextVideoMemBlitter& _textVideoMemBlitter) override;
		void blitSetup(TextVideoMemBlitter& _blitter) override;
		void blitRender(TextVideoMemBlitter& _blitter, uint32_t _numIndices) override;
		void updateResolution(const Resolution& _resolution);
		void setShaderUniform4f(uint8_t /*_flags*/, uint32_t _regIndex, const void* _val, uint32_t _numRegs);
		void setShaderUniform4x4f(uint8_t /*_flags*/, uint32_t _regIndex, const void* _val, uint32_t _numRegs);
		uint32_t setFrameBuffer(FrameBufferHandle _fbh, uint32_t _height, uint16_t _discard = BGFX_CLEAR_NONE, bool _msaa = true);
		uint32_t getNumRt() const;
		void createMsaaFbo(uint32_t _width, uint32_t _height, uint32_t _msaa);
		void destroyMsaaFbo();
		void blitMsaaFbo();
		void setRenderContextSize(uint32_t _width, uint32_t _height, uint32_t _flags = 0);
		void invalidateCache();
		void setSamplerState(uint32_t _stage, uint32_t _numMips, uint32_t _flags, const float _rgba[4]);
		bool isVisible(Frame* _render, OcclusionQueryHandle _handle, bool _visible);
		void updateCapture();
		void capture();
		void captureFinish();
		bool programFetchFromCache(GLuint programId, uint64_t _id);
		void programCache(GLuint programId, uint64_t _id);
		void commit(UniformBuffer& _uniformBuffer);
		void clearQuad(const Rect& _rect, const Clear& _clear, uint32_t _height, const float _palette[][4]);
		void setProgram(GLuint program);
		// Cache uniform uploads to avoid redundant uploading of state that is
		// already set to a shader program
		void setUniform1i(uint32_t loc, int value);
		void setUniform1iv(uint32_t loc, int num, const int* data);
		void setUniform4f(uint32_t loc, float x, float y, float z, float w);
		void setUniform1fv(uint32_t loc, int num, const float* data);
		void setUniform2fv(uint32_t loc, int num, const float* data);
		void setUniform3fv(uint32_t loc, int num, const float* data);
		void setUniform4fv(uint32_t loc, int num, const float* data);
		void setUniformMatrix3fv(uint32_t loc, int num, GLboolean transpose, const float* data);
		void setUniformMatrix4fv(uint32_t loc, int num, GLboolean transpose, const float* data);

		void* m_renderdocdll;

		uint16_t m_numWindows;
		FrameBufferHandle m_windows[BGFX_CONFIG_MAX_FRAME_BUFFERS];

		IndexBufferGL m_indexBuffers[BGFX_CONFIG_MAX_INDEX_BUFFERS];
		VertexBufferGL m_vertexBuffers[BGFX_CONFIG_MAX_VERTEX_BUFFERS];
		ShaderGL m_shaders[BGFX_CONFIG_MAX_SHADERS];
		ProgramGL m_program[BGFX_CONFIG_MAX_PROGRAMS];
		TextureGL m_textures[BGFX_CONFIG_MAX_TEXTURES];
		VertexLayout m_vertexLayouts[BGFX_CONFIG_MAX_VERTEX_LAYOUTS];
		FrameBufferGL m_frameBuffers[BGFX_CONFIG_MAX_FRAME_BUFFERS];
		UniformRegistry m_uniformReg;
		void* m_uniforms[BGFX_CONFIG_MAX_UNIFORMS];

		TimerQueryGL m_gpuTimer;
		OcclusionQueryGL m_occlusionQuery;

		SamplerStateCache m_samplerStateCache;
		UniformStateCache m_uniformStateCache;

		TextVideoMem m_textVideoMem;
		bool m_rtMsaa;

		FrameBufferHandle m_fbh;
		uint16_t m_fbDiscard;

		Resolution m_resolution;
		void* m_capture;
		uint32_t m_captureSize;
		float m_maxAnisotropy;
		float m_maxAnisotropyDefault;
		int32_t m_maxMsaa;
		GLuint m_vao;
		uint16_t m_maxLabelLen;
		bool m_blitSupported;
		bool m_readBackSupported;
		bool m_vaoSupport;
		bool m_samplerObjectSupport;
		bool m_shadowSamplersSupport;
		bool m_srgbWriteControlSupport;
		bool m_borderColorSupport;
		bool m_programBinarySupport;
		bool m_textureSwizzleSupport;
		bool m_depthTextureSupport;
		bool m_timerQuerySupport;
		bool m_occlusionQuerySupport;
		bool m_atocSupport;
		bool m_conservativeRasterSupport;
		bool m_imageLoadStoreSupport;
		bool m_flip;

		uint64_t m_hash;

		GLenum m_readPixelsFmt;
		GLuint m_backBufferFbo;
		GLuint m_msaaBackBufferFbo;
		union {
			GLuint m_msaaBackBufferRbos[2];
			GLuint m_msaaBackBufferTextures[2];
		};
		GLuint m_msaaBlitProgram;
		GlContext m_glctx;
		bool m_needPresent;

		UniformHandle m_clearQuadColor;
		UniformHandle m_clearQuadDepth;

		const char* m_vendor;
		const char* m_renderer;
		const char* m_version;
		const char* m_glslVersion;

		Workaround m_workaround;

		GLuint m_currentFbo;

	private:
		//RendererContextGL是与Unreal Engine中的FOpenGLDynamicRHI具有类似属性及定位
		OpenGLStateContext m_renderingContextState;
		//记录的是当前帧的渲染状态
		OpenGLStateContext m_currState;
		// invalid context state, used to reset m_renderingContextState
		OpenGLcachedState m_invalidContextState;
		//Memory management is through Vector
		//The hash cache uses the LRU policy
		StateCacheT<OpenGLStateFlag, OpenGLcachedState> m_renderStateHashCache;
		//state setter
		FORCEINLINE void initializeCachedContext();

		FORCEINLINE void clearCachedContext();

		FORCEINLINE void setViewPort(uint16_t x, uint16_t y, uint16_t width, uint16_t height);

		//needs to be called before each draw call--完全仿照Unreal Engine的写法
		//ue4中有setRasterizerState以及createRasterizerState的函数,bgfx中stateflag与之对应
		FORCEINLINE void updateRasterizerState(OpenGLStateContext& contextState);

		FORCEINLINE void updateDepthStencilState(OpenGLStateContext& contextState);

		FORCEINLINE void updateScissorRect(OpenGLStateContext& contextState);

		FORCEINLINE void updateViewport(OpenGLStateContext& contextState);

		//设置混合状态--Unreal Engine FOpenGLDynamicRHI::652
		FORCEINLINE void setBlendStateForActiveRenderTargets(OpenGLStateContext& contextState);

		//UE4中没有该方法，bgfx中该方法是为了分检命令，也可以直接抽象到renderdraw(CMD)中去
		//该方法的作用，类似于UE4中的各种渲染状态的set，但是不同的是，这是一个分发
		//后期建议，将该类状态直接传递下来，在使用BGFX时进行打包，在设置渲染状态时解包
		//对于这种没有GPUHandle的渲染状态，使用标志位直接缓存不打包处理，空间换时间
		FORCEINLINE void checkRenderState(OpenGLStateContext& currState);

		FORCEINLINE void parseRasterizerState(OpenGLStateContext& currState);

		FORCEINLINE void parseDepthStencilState(OpenGLStateContext& currState);

		FORCEINLINE void parseBlendState(OpenGLStateContext& currState);

		FORCEINLINE void setScissorRect(uint32_t height, View& view, RectCache& cache);

		FORCEINLINE void resetContextState(OpenGLStateContext& contextState);
	};

	extern RendererContextGL* s_renderGL;

	FORCEINLINE void RendererContextGL::initializeCachedContext()
	{
		if (m_renderingContextState.m_states)
		{
			delete m_renderingContextState.m_states;
		}

		m_renderingContextState.m_states = new OpenGLcachedState;
	}

	FORCEINLINE void RendererContextGL::clearCachedContext()
	{
		if (m_renderingContextState.m_states)
		{
			delete m_renderingContextState.m_states;
		}

		m_currState.m_states = NULL;
		m_renderingContextState.m_states = NULL;
	}

	FORCEINLINE void RendererContextGL::setViewPort(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
	{
		m_currState.m_viewportRect.set(x, y, width, height);
	}

	FORCEINLINE void RendererContextGL::setScissorRect(uint32_t height, View& view, RectCache& cache)
	{
		const Rect& viewScissorRect = view.m_scissor;

		if (m_currState.m_scissor == UINT16_MAX) // use scissor rect of the current view 
		{
			if (viewScissorRect.isZero())
			{
				// if the current view doesn't contain scissor, disable it

				m_currState.m_ScissorEnabled = GL_FALSE;
			}
			else
			{
				m_currState.m_ScissorEnabled = GL_TRUE;
				m_currState.m_scissorRect.set(viewScissorRect.m_x
					, height - viewScissorRect.m_height - viewScissorRect.m_y
					, viewScissorRect.m_width
					, viewScissorRect.m_height);

			}
		}
		else
		{
			// use scissor rect saved in cache
			Rect scissorRect;

			if (viewScissorRect.isZero())
			{
				// if current view doesn't contain scissor, use viewport rect to compute scissor rect
				scissorRect.setIntersect(view.m_rect, cache.m_cache[m_currState.m_scissor]);
			}
			else
			{
				scissorRect.setIntersect(viewScissorRect, cache.m_cache[m_currState.m_scissor]);
			}

			m_currState.m_ScissorEnabled = GL_TRUE;
			m_currState.m_scissorRect.set(scissorRect.m_x
				, height - scissorRect.m_height - scissorRect.m_y
				, scissorRect.m_width
				, scissorRect.m_height
			);
		}
	}

	FORCEINLINE void RendererContextGL::parseRasterizerState(OpenGLStateContext& currState)
	{
		OpenGLRasterizerState& _currStates = currState.m_states->m_rasterizerState;
		uint64_t renderflag = currState.m_statesflag.m_renderstate;

		if (renderflag & BGFX_STATE_FRONT_CCW)
		{
			_currStates.m_FrontCullMode = GL_CCW;
		}
		else
		{
			_currStates.m_FrontCullMode = GL_CW;
		}
		//剪裁的开启与关闭以及剪裁的方式
		if (renderflag & BGFX_STATE_CULL_CCW)
		{
			_currStates.m_CullingEnable = GL_TRUE;
			_currStates.m_CullMode = GL_BACK;
		}
		else if (renderflag & BGFX_STATE_CULL_CW)
		{
			_currStates.m_CullingEnable = GL_TRUE;
			_currStates.m_CullMode = GL_FRONT;
		}
		else
		{
			_currStates.m_CullingEnable = GL_FALSE;
		}

		if (BX_ENABLED(BGFX_CONFIG_RENDERER_OPENGL))
		{
			// multisampling anti-aliasing 
			if (renderflag & BGFX_STATE_MSAA)
			{
				_currStates.m_MSAAEnable = GL_TRUE;
			}
			else
			{
				_currStates.m_MSAAEnable = GL_FALSE;
			}

			// line anti-aliasing
			if (renderflag & BGFX_STATE_LINEAA)
			{
				_currStates.m_LineSmoothEnable = GL_TRUE;
			}
			else
			{
				_currStates.m_LineSmoothEnable = GL_FALSE;
			}

			// point size
			if (renderflag & (BGFX_STATE_PT_POINTS | BGFX_STATE_POINT_SIZE_MASK))
			{
				_currStates.m_pointSize = (float)(bx::uint32_max(1, (renderflag & BGFX_STATE_POINT_SIZE_MASK) >> BGFX_STATE_POINT_SIZE_SHIFT));
			}

			// conservative rasterizaton. Supported by Nvidia extension
			if (m_conservativeRasterSupport)
			{
				if (renderflag & BGFX_STATE_CONSERVATIVE_RASTER)
				{
					_currStates.m_conservativeRasterEnable = GL_TRUE;
				}
				else
				{
					_currStates.m_conservativeRasterEnable = GL_FALSE;
				}
			}
		}
	}

	FORCEINLINE void RendererContextGL::parseDepthStencilState(OpenGLStateContext& currState)
	{
		OpenGLDepthStencilState& _currStates = currState.m_states->m_depthStencilState;
		uint64_t renderflag = currState.m_statesflag.m_renderstate;
		uint64_t stencilflag = currState.m_statesflag.m_stencil;
		// Depth buffer write enable
		if (renderflag & BGFX_STATE_WRITE_Z)
		{
			_currStates.m_ZWriteEnable = GL_TRUE;
		}
		else
		{
			_currStates.m_ZWriteEnable = GL_FALSE;
		}

		// Depth Test and Depth Func
		uint32_t func = (renderflag & BGFX_STATE_DEPTH_TEST_MASK) >> BGFX_STATE_DEPTH_TEST_SHIFT;

		if (0 != func)
		{
			_currStates.m_ZEnable = GL_TRUE;
			_currStates.m_ZFunc = OpenGL::s_cmpFunc[func];
		}
		else
		{
			if (_currStates.m_ZWriteEnable)
			{
				_currStates.m_ZEnable = GL_TRUE;
			}
			else
			{
				_currStates.m_ZEnable = GL_FALSE;
			}
			_currStates.m_ZFunc = GL_ALWAYS;
		}


		// Stencil
		if (stencilflag != 0)
		{
			_currStates.m_StencilEnable = GL_TRUE;
		}
		else
		{
			_currStates.m_StencilEnable = GL_FALSE;
		}

		uint32_t bstencil = unpackStencil(1, stencilflag);
		uint32_t fstencil = unpackStencil(0, stencilflag);
		_currStates.m_TwoSidedStencilMode = ((bstencil != BGFX_STENCIL_NONE) && (bstencil != fstencil));

		// Front stencil
		if (_currStates.m_StencilEnable)
		{
			func = (fstencil & BGFX_STENCIL_TEST_MASK) >> BGFX_STENCIL_TEST_SHIFT;
			_currStates.m_StencilFunc = OpenGL::s_cmpFunc[func];

			_currStates.m_StencilReadMask = (fstencil & BGFX_STENCIL_FUNC_RMASK_MASK) >> BGFX_STENCIL_FUNC_RMASK_SHIFT;
			_currStates.m_StencilRef = (fstencil & BGFX_STENCIL_FUNC_REF_MASK) >> BGFX_STENCIL_FUNC_REF_SHIFT;


			func = (fstencil & BGFX_STENCIL_OP_FAIL_S_MASK) >> BGFX_STENCIL_OP_FAIL_S_SHIFT;
			_currStates.m_StencilFail = OpenGL::s_stencilOp[func];

			func = (fstencil & BGFX_STENCIL_OP_FAIL_Z_MASK) >> BGFX_STENCIL_OP_FAIL_Z_SHIFT;
			_currStates.m_StencilZFail = OpenGL::s_stencilOp[func];

			func = (fstencil & BGFX_STENCIL_OP_PASS_Z_MASK) >> BGFX_STENCIL_OP_PASS_Z_SHIFT;
			_currStates.m_StencilPass = OpenGL::s_stencilOp[func];


			if (_currStates.m_TwoSidedStencilMode)
			{
				func = (bstencil & BGFX_STENCIL_TEST_MASK) >> BGFX_STENCIL_TEST_SHIFT;
				_currStates.m_bStencilFunc = OpenGL::s_cmpFunc[func];

				_currStates.m_bStencilReadMask = (bstencil & BGFX_STENCIL_FUNC_RMASK_MASK) >> BGFX_STENCIL_FUNC_RMASK_SHIFT;
				_currStates.m_bStencilRef = (bstencil & BGFX_STENCIL_FUNC_REF_MASK) >> BGFX_STENCIL_FUNC_REF_SHIFT;


				func = (bstencil & BGFX_STENCIL_OP_FAIL_S_MASK) >> BGFX_STENCIL_OP_FAIL_S_SHIFT;
				_currStates.m_bStencilFail = OpenGL::s_stencilOp[func];

				func = (bstencil & BGFX_STENCIL_OP_FAIL_Z_MASK) >> BGFX_STENCIL_OP_FAIL_Z_SHIFT;
				_currStates.m_bStencilZFail = OpenGL::s_stencilOp[func];

				func = (bstencil & BGFX_STENCIL_OP_PASS_Z_MASK) >> BGFX_STENCIL_OP_PASS_Z_SHIFT;
				_currStates.m_bStencilPass = OpenGL::s_stencilOp[func];
			}
		}
	}

	FORCEINLINE void RendererContextGL::parseBlendState(OpenGLStateContext& currState)
	{
		OpenGLBlendState& _currStates = currState.m_states->m_blendState;
		uint64_t renderflag = currState.m_statesflag.m_renderstate;
		//Blend State is Decoded To The Rendering API
		//If enabled, compute a temporary coverage value where each bit is determined by the 
		//alpha value at the corresponding sample location. The temporary coverage value is 
		//then ANDed with the fragment coverage value.
		if (m_atocSupport)
		{
			if (renderflag & BGFX_STATE_BLEND_ALPHA_TO_COVERAGE)
			{
				_currStates.m_sampleAlphaToCoverage = GL_TRUE;
			}
			else
			{
				_currStates.m_sampleAlphaToCoverage = GL_FALSE;
			}
		}

		//is enable blend
		bool enable = !!(BGFX_STATE_BLEND_MASK & renderflag);
		//GLES3.2 and OpenGL4.3 support api,Glenablei, for example
		//height api support glBlendFunci and glBlendEquationi 
		bool blendIndependentSupported = OpenGL::s_extension[Extension::ARB_draw_buffers_blend].m_supported;
		bool heightApi_independent = !!(renderflag & BGFX_STATE_BLEND_INDEPENDENT) && blendIndependentSupported;
		bool bx_blend = !BX_ENABLED(BGFX_CONFIG_RENDERER_OPENGL);
		uint32_t numRt = getNumRt();
		uint32_t rgba = currState.m_rgba;


		// FIXEME, doesn't support different blend state for MRT attachments currently
		// All attachments use the state of the first drawbuffer
		heightApi_independent = false;
		for (int rti = 0; rti < FORCE_SAMESTATE_MRT; ++rti)
		{
			OpenGLBlendState::DrawBuffer& drawbuffer = _currStates.m_drawBuffers[rti];
			//Record the color mask change
			drawbuffer.m_colorWriteMaskR = !!(renderflag & BGFX_STATE_WRITE_R);
			drawbuffer.m_colorWriteMaskG = !!(renderflag & BGFX_STATE_WRITE_G);
			drawbuffer.m_colorWriteMaskB = !!(renderflag & BGFX_STATE_WRITE_B);
			drawbuffer.m_colorWriteMaskA = !!(renderflag & BGFX_STATE_WRITE_A);

			drawbuffer.m_supportBlendIndex = heightApi_independent;
			// not support height api
			if (!heightApi_independent || bx_blend || 1 >= numRt)
			{
				drawbuffer.m_alphaBlendEnable = enable;
				drawbuffer.m_separateAlphaBlendEnable = true;
				if (enable)
				{
					//blend factor
					uint32_t blend = uint32_t((renderflag & BGFX_STATE_BLEND_MASK) >> BGFX_STATE_BLEND_SHIFT);
					const uint32_t srcRGB = (blend) & 0xf;
					const uint32_t dstRGB = (blend >> 4) & 0xf;
					const uint32_t srcA = (blend >> 8) & 0xf;
					const uint32_t dstA = (blend >> 12) & 0xf;
					//blend equation
					const uint32_t equ = uint32_t((renderflag & BGFX_STATE_BLEND_EQUATION_MASK) >> BGFX_STATE_BLEND_EQUATION_SHIFT);
					const uint32_t equRGB = (equ) & 0x7;
					const uint32_t equA = (equ >> 3) & 0x7;
					//rgb blend factor and operation
					drawbuffer.m_colorSourceBlendFactor = OpenGL::s_blendFactor[srcRGB].m_src;
					drawbuffer.m_colorDestBlendFacetor = OpenGL::s_blendFactor[dstRGB].m_dst;
					drawbuffer.m_colorBlendOperation = OpenGL::s_blendEquation[equRGB];
					//alpla blend factor and operation
					drawbuffer.m_alphaSourceBlendFactor = OpenGL::s_blendFactor[srcA].m_src;
					drawbuffer.m_alphaDestBlendFactor = OpenGL::s_blendFactor[dstA].m_dst;
					drawbuffer.m_alphaBlendOperation = OpenGL::s_blendEquation[equA];
					if ((OpenGL::s_blendFactor[srcRGB].m_factor || OpenGL::s_blendFactor[dstRGB].m_factor) &&
						rgba != 0)
					{
						drawbuffer.m_enableBlendColor = true;
						//const uint32_t rgba = draw.m_rgba;
						drawbuffer.m_blendColorR = ((rgba >> 24)) / 255.0f;
						drawbuffer.m_blendColorG = ((rgba >> 16) & 0xff) / 255.0f;
						drawbuffer.m_blendColorB = ((rgba >> 8) & 0xff) / 255.0f;
						drawbuffer.m_blendColorA = ((rgba) & 0xff) / 255.0f;
					}
					else
					{
						drawbuffer.m_enableBlendColor = false;
					}
				}
			}
			else//support height api
			{
				bool enableBlendIndex = !!(rgba & 0x7ff);
				drawbuffer.m_alphaBlendEnable = enableBlendIndex;
				//The advanced API in the second buffer uses RGBA's hybrid approach
				drawbuffer.m_separateAlphaBlendEnable = false;
				if (enableBlendIndex)//RGBA blend factor and operation change
				{
					uint32_t src = (rgba) & 0xf;
					uint32_t dst = (rgba >> 4) & 0xf;
					uint32_t equation = (rgba >> 8) & 0x7;
					drawbuffer.m_colorSourceBlendFactor = src;
					drawbuffer.m_colorDestBlendFacetor = dst;
					drawbuffer.m_colorBlendOperation = equation;
				}
				rgba = rgba >> 11;
			}
		}
	}

	FORCEINLINE void RendererContextGL::checkRenderState(OpenGLStateContext& currState)
	{

		OpenGLStateContext& contextState = m_renderingContextState;

		if (contextState.m_statesflag != currState.m_statesflag)
		{
			currState.m_states = m_renderStateHashCache.find(currState.m_statesflag);

			if (!currState.m_states)
			{

				currState.m_states = m_renderStateHashCache.add(currState.m_statesflag);
				parseRasterizerState(currState);
				parseDepthStencilState(currState);
				parseBlendState(currState);

			}
			contextState.m_statesflag = currState.m_statesflag;
		}
	}

	FORCEINLINE void RendererContextGL::updateRasterizerState(OpenGLStateContext& contextState)
	{
		//OpenGL是一个状态机，始终保持自己当前的状态，除非用户输入一条命令让它改变状态
		//一次drawcall设置glFrontFace(GL_CCW)，后面所有drawcall都会保持这个状态，除非用户改变为glFrontFace(GL_CW)
		//可以使用glIsEnable(XXXX)来查看这些状态是否开启
		//设置面卷绕方式

		OpenGLRasterizerState& _cachedStates = contextState.m_states->m_rasterizerState;
		OpenGLRasterizerState& _currStates = m_currState.m_states->m_rasterizerState;

		if (_cachedStates.m_FrontCullMode != _currStates.m_FrontCullMode)
		{
			GL_CHECK(glFrontFace(_currStates.m_FrontCullMode));
			//缓存上一次drawcall的卷绕方式
			_cachedStates.m_FrontCullMode = _currStates.m_FrontCullMode;
		}
		//设置剪裁方式
		if (_cachedStates.m_CullingEnable != _currStates.m_CullingEnable)
		{
			if (_currStates.m_CullingEnable)
			{
				GL_CHECK(glEnable(GL_CULL_FACE));

			}
			else
			{
				GL_CHECK(glDisable(GL_CULL_FACE));
			}

			_cachedStates.m_CullingEnable = _currStates.m_CullingEnable;
		}


		if (_currStates.m_CullingEnable && _cachedStates.m_CullMode != _currStates.m_CullMode)
		{
			GL_CHECK(glCullFace(_currStates.m_CullMode));
			_cachedStates.m_CullMode = _currStates.m_CullMode;
		}

		if (BX_ENABLED(BGFX_CONFIG_RENDERER_OPENGL))
		{
			// multisampling anti-aliasing 
			if (_cachedStates.m_MSAAEnable != _currStates.m_MSAAEnable)
			{
				if (_currStates.m_MSAAEnable)
				{
					GL_CHECK(glEnable(GL_MULTISAMPLE));
				}
				else
				{
					GL_CHECK(glDisable(GL_MULTISAMPLE));
				}
				_cachedStates.m_MSAAEnable = _currStates.m_MSAAEnable;
			}

			// line anti-aliasing: deprecated in opengl 3.0,  removed from core opengl3.1 and above
			if (_cachedStates.m_LineSmoothEnable != _currStates.m_LineSmoothEnable)
			{
				if (_currStates.m_LineSmoothEnable)
				{
					GL_CHECK(glEnable(GL_LINE_SMOOTH));
				}
				else
				{
					GL_CHECK(glDisable(GL_LINE_SMOOTH));
				}
				_cachedStates.m_LineSmoothEnable = _currStates.m_LineSmoothEnable;
			}

			// set point size
			if (bx::abs(_cachedStates.m_pointSize - _currStates.m_pointSize) > 0.0001f)
			{
				GL_CHECK(glPointSize(_currStates.m_pointSize));
				_cachedStates.m_pointSize = _currStates.m_pointSize;
			}

			// conservative rasterizaton. Supported by Nvidia extension
			if (m_conservativeRasterSupport)
			{
				if (_cachedStates.m_conservativeRasterEnable != _currStates.m_conservativeRasterEnable)
				{
					if (_currStates.m_conservativeRasterEnable)
					{
						GL_CHECK(glEnable(GL_CONSERVATIVE_RASTERIZATION_NV));
					}
					else
					{
						GL_CHECK(glDisable(GL_CONSERVATIVE_RASTERIZATION_NV));
					}
					_cachedStates.m_conservativeRasterEnable = _currStates.m_conservativeRasterEnable;
				}
			}
		}


	}

	FORCEINLINE void RendererContextGL::updateDepthStencilState(OpenGLStateContext& contextState)
	{
		OpenGLDepthStencilState& _cachedStates = contextState.m_states->m_depthStencilState;
		OpenGLDepthStencilState& _currStates = m_currState.m_states->m_depthStencilState;

		// Depth State
		// Depth Test Update
		if (_cachedStates.m_ZEnable != _currStates.m_ZEnable)
		{
			if (_currStates.m_ZEnable)
			{
				GL_CHECK(glEnable(GL_DEPTH_TEST));
			}
			else
			{
				GL_CHECK(glDisable(GL_DEPTH_TEST));
			}
			_cachedStates.m_ZEnable = _currStates.m_ZEnable;
		}


		if (_currStates.m_ZEnable)
		{
			if (_cachedStates.m_ZWriteEnable != _currStates.m_ZWriteEnable)
			{
				GL_CHECK(glDepthMask(_currStates.m_ZWriteEnable));
				_cachedStates.m_ZWriteEnable = _currStates.m_ZWriteEnable;
			}

			if (_cachedStates.m_ZFunc != _currStates.m_ZFunc)
			{
				GL_CHECK(glDepthFunc(_currStates.m_ZFunc));
				_cachedStates.m_ZFunc = _currStates.m_ZFunc;
			}
		}


		// Stencil State
		// Stencil Test Enable Update
		if (_cachedStates.m_StencilEnable != _currStates.m_StencilEnable)
		{
			if (_currStates.m_StencilEnable)
			{
				GL_CHECK(glEnable(GL_STENCIL_TEST));
			}
			else
			{
				GL_CHECK(glDisable(GL_STENCIL_TEST));
			}
			_cachedStates.m_StencilEnable = _currStates.m_StencilEnable;
		}

		// According to UE4, when switch between one--sided <---> two--sided, its better to invoke all set of functions
		// As some drivers might handle them differently - some of them might keep those states in different variables
		if (_cachedStates.m_TwoSidedStencilMode != _currStates.m_TwoSidedStencilMode)
		{
			// forcefully invalid all cache stencil values so that all values are reset by incoming state
			_cachedStates.m_StencilFunc = 0xFFFFFFFF;
			_cachedStates.m_StencilFail = 0xFFFFFFFF;
			_cachedStates.m_StencilZFail = 0xFFFFFFFF;
			_cachedStates.m_StencilPass = 0xFFFFFFFF;
			_cachedStates.m_StencilReadMask = 0xFFFFFFFF;

			_cachedStates.m_bStencilFunc = 0xFFFFFFFF;
			_cachedStates.m_bStencilFail = 0xFFFFFFFF;
			_cachedStates.m_bStencilZFail = 0xFFFFFFFF;
			_cachedStates.m_bStencilPass = 0xFFFFFFFF;
			_cachedStates.m_bStencilReadMask = 0xFFFFFFFF;

			_cachedStates.m_StencilWriteMask = 0xFFFFFFFF;

			_cachedStates.m_TwoSidedStencilMode = _currStates.m_TwoSidedStencilMode;

		}

		if (_currStates.m_StencilEnable)
		{
			// FIXME: bgfx doesnt support setting stencil write mask, write mask is all enabled currently 
			// control writing to stencil buffer
			/*if (_cachedStates.m_StencilWriteMask != _currStates.m_StencilWriteMask)
			{
				GL_CHECK(glStencilMask(_currStates.m_StencilWriteMask));
				_cachedStates.m_StencilWriteMask = _currStates.m_StencilWriteMask;
			}*/

			// stencil test can be setup separately for front and back faces
			if (_currStates.m_TwoSidedStencilMode)
			{
				// Set Stencil test compare function , reference value and read mask before compare
				if (_cachedStates.m_StencilFunc != _currStates.m_StencilFunc
					|| _cachedStates.m_StencilRef != _currStates.m_StencilRef
					|| _cachedStates.m_StencilReadMask != _currStates.m_StencilReadMask)
				{
					GL_CHECK(glStencilFuncSeparate(GL_FRONT
						, _currStates.m_StencilFunc
						, _currStates.m_StencilRef
						, _currStates.m_StencilReadMask));

					_cachedStates.m_StencilFunc = _currStates.m_StencilFunc;
					_cachedStates.m_StencilRef = _currStates.m_StencilRef;
					_cachedStates.m_StencilReadMask = _currStates.m_StencilReadMask;
				}

				// Set actions to take when stencil & depth test fails/passes
				if (_cachedStates.m_StencilFail != _currStates.m_StencilFail
					|| _cachedStates.m_StencilZFail != _currStates.m_StencilZFail
					|| _cachedStates.m_StencilPass != _currStates.m_StencilPass)
				{
					GL_CHECK(glStencilOpSeparate(GL_FRONT
						, _currStates.m_StencilFail
						, _currStates.m_StencilZFail
						, _currStates.m_StencilPass));

					_cachedStates.m_StencilFail = _currStates.m_StencilFail;
					_cachedStates.m_StencilZFail = _currStates.m_StencilZFail;
					_cachedStates.m_StencilPass = _currStates.m_StencilPass;
				}

				if (_cachedStates.m_bStencilFunc != _currStates.m_bStencilFunc
					|| _cachedStates.m_bStencilRef != _currStates.m_bStencilRef
					|| _cachedStates.m_bStencilReadMask != _currStates.m_bStencilReadMask)
				{
					GL_CHECK(glStencilFuncSeparate(GL_BACK
						, _currStates.m_bStencilFunc
						, _currStates.m_bStencilRef
						, _currStates.m_bStencilReadMask));

					_cachedStates.m_bStencilFunc = _currStates.m_bStencilFunc;
					_cachedStates.m_bStencilRef = _currStates.m_bStencilRef;
					_cachedStates.m_bStencilReadMask = _currStates.m_bStencilReadMask;
				}

				if (_cachedStates.m_bStencilFail != _currStates.m_bStencilFail
					|| _cachedStates.m_bStencilZFail != _currStates.m_bStencilZFail
					|| _cachedStates.m_bStencilPass != _currStates.m_bStencilPass)
				{
					GL_CHECK(glStencilOpSeparate(GL_BACK
						, _currStates.m_bStencilFail
						, _currStates.m_bStencilZFail
						, _currStates.m_bStencilPass));

					_cachedStates.m_bStencilFail = _currStates.m_bStencilFail;
					_cachedStates.m_bStencilZFail = _currStates.m_bStencilZFail;
					_cachedStates.m_bStencilPass = _currStates.m_bStencilPass;
				}
			}
			else
			{
				if (_cachedStates.m_StencilFunc != _currStates.m_StencilFunc
					|| _cachedStates.m_StencilRef != _currStates.m_StencilRef
					|| _cachedStates.m_StencilReadMask != _currStates.m_StencilReadMask)
				{
					GL_CHECK(glStencilFuncSeparate(GL_FRONT_AND_BACK
						, _currStates.m_StencilFunc
						, _currStates.m_StencilRef
						, _currStates.m_StencilReadMask));

					_cachedStates.m_StencilFunc = _currStates.m_StencilFunc;
					_cachedStates.m_StencilRef = _currStates.m_StencilRef;
					_cachedStates.m_StencilReadMask = _currStates.m_StencilReadMask;
				}


				if (_cachedStates.m_StencilFail != _currStates.m_StencilFail
					|| _cachedStates.m_StencilZFail != _currStates.m_StencilZFail
					|| _cachedStates.m_StencilPass != _currStates.m_StencilPass)
				{
					GL_CHECK(glStencilOpSeparate(GL_FRONT_AND_BACK
						, _currStates.m_StencilFail
						, _currStates.m_StencilZFail
						, _currStates.m_StencilPass));

					_cachedStates.m_StencilFail = _currStates.m_StencilFail;
					_cachedStates.m_StencilZFail = _currStates.m_StencilZFail;
					_cachedStates.m_StencilPass = _currStates.m_StencilPass;
				}
			}

		}

	}

	FORCEINLINE void RendererContextGL::updateScissorRect(OpenGLStateContext& contextState)
	{

		if (contextState.m_ScissorEnabled != m_currState.m_ScissorEnabled)
		{
			if (m_currState.m_ScissorEnabled)
			{
				GL_CHECK(glEnable(GL_SCISSOR_TEST));
			}
			else
			{
				GL_CHECK(glDisable(GL_SCISSOR_TEST));
			}
			contextState.m_ScissorEnabled = m_currState.m_ScissorEnabled;
		}

		if (m_currState.m_ScissorEnabled
			&& contextState.m_scissorRect != m_currState.m_scissorRect)
		{
			GL_CHECK(glScissor(m_currState.m_scissorRect.m_x
				, m_currState.m_scissorRect.m_y
				, m_currState.m_scissorRect.m_width
				, m_currState.m_scissorRect.m_height
			));
			contextState.m_scissorRect = m_currState.m_scissorRect;
		}
	}

	FORCEINLINE void RendererContextGL::updateViewport(OpenGLStateContext& contextState)
	{
		if (contextState.m_viewportRect != m_currState.m_viewportRect)
		{
			GL_CHECK(glViewport(m_currState.m_viewportRect.m_x
				, m_currState.m_viewportRect.m_y
				, m_currState.m_viewportRect.m_width
				, m_currState.m_viewportRect.m_height
			));

			contextState.m_viewportRect = m_currState.m_viewportRect;
		}
	}

	FORCEINLINE void RendererContextGL::setBlendStateForActiveRenderTargets(OpenGLStateContext& contextState)
	{
		OpenGLBlendState& _cachedStates = contextState.m_states->m_blendState;
		OpenGLBlendState& _currStates = m_currState.m_states->m_blendState;

		//BGFX support this api,ue4 not support
		if (m_atocSupport)
		{
			if (_cachedStates.m_sampleAlphaToCoverage != _currStates.m_sampleAlphaToCoverage)
			{
				if (_currStates.m_sampleAlphaToCoverage)
				{
					GL_CHECK(glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE));
				}
				else
				{
					GL_CHECK(glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE));
				}
				_cachedStates.m_sampleAlphaToCoverage = _currStates.m_sampleAlphaToCoverage;
			}
		}

		//uint32_t numRt = getNumRt();
		//uint32_t drawBuffers_size = m_currState.m_blendState.m_drawBuffers.size();
		for (int rti = 0; rti < FORCE_SAMESTATE_MRT; ++rti)
		{
			const OpenGLBlendState::DrawBuffer& currRT = _currStates.m_drawBuffers[rti];
			OpenGLBlendState::DrawBuffer& cachRT = _cachedStates.m_drawBuffers[rti];
			//enable blend and disable blend
			if (cachRT.m_alphaBlendEnable != currRT.m_alphaBlendEnable)
			{
				//height api support mutil buffer blend
				if (currRT.m_supportBlendIndex)
				{
					if (currRT.m_alphaBlendEnable)
					{
						GL_CHECK(glEnablei(GL_BLEND, rti));
					}
					else
					{
						GL_CHECK(glDisablei(GL_BLEND, rti));
					}
				}
				else//normal blend api enable
				{
					if (currRT.m_alphaBlendEnable)
					{
						GL_CHECK(glEnable(GL_BLEND));
					}
					else
					{
						GL_CHECK(glDisable(GL_BLEND));
					}
				}
				cachRT.m_alphaBlendEnable = currRT.m_alphaBlendEnable;
			}
			if (currRT.m_alphaBlendEnable)
			{
				if (currRT.m_separateAlphaBlendEnable)
				{
					if (currRT.m_supportBlendIndex)
					{
						//set blend factor as separate
						if (cachRT.m_colorSourceBlendFactor != currRT.m_colorSourceBlendFactor
							|| cachRT.m_colorDestBlendFacetor != currRT.m_colorDestBlendFacetor
							|| cachRT.m_alphaSourceBlendFactor != currRT.m_alphaSourceBlendFactor
							|| cachRT.m_alphaDestBlendFactor != currRT.m_alphaDestBlendFactor)
						{
							GL_CHECK(glBlendFuncSeparatei(rti
								, currRT.m_colorSourceBlendFactor
								, currRT.m_colorDestBlendFacetor
								, currRT.m_alphaSourceBlendFactor
								, currRT.m_alphaDestBlendFactor));
						}
						if (cachRT.m_colorBlendOperation != currRT.m_colorBlendOperation
							|| cachRT.m_alphaBlendOperation != currRT.m_alphaBlendOperation)
						{
							GL_CHECK(glBlendEquationSeparatei(rti
								, currRT.m_colorBlendOperation
								, currRT.m_alphaBlendOperation));
						}
					}
					else
					{
						if (cachRT.m_colorSourceBlendFactor != currRT.m_colorSourceBlendFactor
							|| cachRT.m_colorDestBlendFacetor != currRT.m_colorDestBlendFacetor
							|| cachRT.m_alphaSourceBlendFactor != currRT.m_alphaSourceBlendFactor
							|| cachRT.m_alphaDestBlendFactor != currRT.m_alphaDestBlendFactor)
						{
							GL_CHECK(glBlendFuncSeparate(
								currRT.m_colorSourceBlendFactor
								, currRT.m_colorDestBlendFacetor
								, currRT.m_alphaSourceBlendFactor
								, currRT.m_alphaDestBlendFactor));
						}

						if (cachRT.m_colorBlendOperation != currRT.m_colorBlendOperation
							|| cachRT.m_alphaBlendOperation != currRT.m_alphaBlendOperation)
						{
							GL_CHECK(glBlendEquationSeparate(
								currRT.m_colorBlendOperation
								, currRT.m_alphaBlendOperation));
						}
					}
				}
				else
				{
					if (currRT.m_supportBlendIndex)
					{
						if (cachRT.m_colorSourceBlendFactor != currRT.m_colorSourceBlendFactor
							|| cachRT.m_colorDestBlendFacetor != currRT.m_colorDestBlendFacetor)
						{
							GL_CHECK(glBlendFunci(rti, currRT.m_colorSourceBlendFactor, currRT.m_colorDestBlendFacetor));
						}

						if (cachRT.m_colorBlendOperation != currRT.m_colorBlendOperation)
						{
							GL_CHECK(glBlendEquationi(rti, currRT.m_colorBlendOperation));
						}
					}
				}
				cachRT.m_separateAlphaBlendEnable = currRT.m_separateAlphaBlendEnable;
				cachRT.m_colorBlendOperation = currRT.m_colorBlendOperation;
				cachRT.m_colorSourceBlendFactor = currRT.m_colorSourceBlendFactor;
				cachRT.m_colorDestBlendFacetor = currRT.m_colorDestBlendFacetor;
				if (currRT.m_separateAlphaBlendEnable)
				{
					cachRT.m_alphaSourceBlendFactor = currRT.m_alphaSourceBlendFactor;
					cachRT.m_alphaDestBlendFactor = currRT.m_alphaDestBlendFactor;
					cachRT.m_alphaBlendOperation = currRT.m_alphaBlendOperation;
				}
				else
				{
					cachRT.m_alphaSourceBlendFactor = currRT.m_colorSourceBlendFactor;
					cachRT.m_alphaDestBlendFactor = currRT.m_colorDestBlendFacetor;
				}
				if (currRT.m_enableBlendColor)
				{
					//blend color
					if (cachRT.m_blendColorR != currRT.m_blendColorR
						|| cachRT.m_blendColorG != currRT.m_blendColorG
						|| cachRT.m_blendColorB != currRT.m_blendColorB
						|| cachRT.m_blendColorA != currRT.m_blendColorA)
					{
						GL_CHECK(glBlendColor(currRT.m_blendColorR
							, currRT.m_blendColorG
							, currRT.m_blendColorB
							, currRT.m_blendColorA));
						cachRT.m_enableBlendColor = currRT.m_enableBlendColor;
						cachRT.m_blendColorR = currRT.m_blendColorR;
						cachRT.m_blendColorG = currRT.m_blendColorG;
						cachRT.m_blendColorB = currRT.m_blendColorB;
						cachRT.m_blendColorA = currRT.m_blendColorA;
					}
				}
			}
			cachRT.m_supportBlendIndex = currRT.m_supportBlendIndex;
			if (cachRT.m_colorWriteMaskR != currRT.m_colorWriteMaskR
				|| cachRT.m_colorWriteMaskG != currRT.m_colorWriteMaskG
				|| cachRT.m_colorWriteMaskB != currRT.m_colorWriteMaskB
				|| cachRT.m_colorWriteMaskA != currRT.m_colorWriteMaskA)
			{
				GL_CHECK(glColorMask(
					currRT.m_colorWriteMaskR
					, currRT.m_colorWriteMaskG
					, currRT.m_colorWriteMaskB
					, currRT.m_colorWriteMaskA));

				cachRT.m_colorWriteMaskR = currRT.m_colorWriteMaskR;
				cachRT.m_colorWriteMaskG = currRT.m_colorWriteMaskG;
				cachRT.m_colorWriteMaskB = currRT.m_colorWriteMaskB;
				cachRT.m_colorWriteMaskA = currRT.m_colorWriteMaskA;
			}
		}
	}//end setBlendStateForActiveRenderTargets

	FORCEINLINE void RendererContextGL::resetContextState(OpenGLStateContext& contextState)
	{

		contextState.m_statesflag.m_renderstate = 0;
		contextState.m_statesflag.m_stencil = 0;
		contextState.m_ScissorEnabled = GLTriState::INVALID;
		contextState.m_scissorRect.clear();
		contextState.m_viewportRect.clear();

		contextState.m_clearDepth = -1;
		contextState.m_clearStencil = -1;
		bx::memSet(contextState.m_clearColor, -1, 8);

		// points m_states in cached context to m_invalidContextState
		*contextState.m_states = m_invalidContextState;
	}

} /* namespace gl */ } // namespace bgfx


#endif // BGFX_RENDERER_GL_H_HEADER_GUARD
