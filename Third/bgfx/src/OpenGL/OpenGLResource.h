#pragma once

#include "OpenGLEnv.h"

#include "../PlatformSettings.h"
#include "../renderer.h"
#include "../debug_renderdoc.h"
#include "../emscripten.h"

#include <vector>

namespace bgfx { namespace gl {

	// GLTriState is used to replace two-state bool type
	// FALSE_STATE <-------> GL_FALSE
	// TRUE_STATE <-------> GL_TRUE
	// INVALID only used in initialization and resetting state to force opengl state setting API calls
	struct GLTriState
	{
		enum Enum
		{
			FALSE_STATE = 0,
			TRUE_STATE = 1,
			INVALID = 2,
		};
	};

	struct OpenGLRasterizerState
	{
		OpenGLRasterizerState()
			: m_FillMode(GL_NONE)
			, m_CullMode(GL_NONE)
			, m_FrontCullMode(GL_NONE)
			, m_CullingEnable(GLTriState::INVALID)
			, m_MSAAEnable(GLTriState::INVALID)
			, m_LineSmoothEnable(GLTriState::INVALID)
			, m_conservativeRasterEnable(GLTriState::INVALID)
			, m_pointSize(-1.0)
			, m_DepthBias(0.0f)
			, m_SlopeScaleDepthBias(0.0f)
		{
		}
		//bgfx not support rendering api;unreal engine use rendering api
		GLenum m_FillMode;
		GLenum m_FrontCullMode;
		GLenum m_CullMode;

		uint32_t   m_CullingEnable : 2;
		uint32_t   m_MSAAEnable : 2;
		uint32_t   m_LineSmoothEnable : 2;
		uint32_t   m_conservativeRasterEnable : 2;

		float  m_pointSize;

		//BGFX does not support this rendering api
		//solve z-Fighting
		float m_DepthBias;
		//solve z-fighting
		float m_SlopeScaleDepthBias;
	};

	//Mimics the depth stencil state structure in Unreal Engine
	struct OpenGLDepthStencilState
	{
		OpenGLDepthStencilState()
			: m_ZEnable(GLTriState::INVALID)
			, m_ZWriteEnable(GLTriState::INVALID)
			, m_StencilEnable(GLTriState::INVALID)
			, m_TwoSidedStencilMode(GLTriState::INVALID)
			, m_ZFunc(GL_NONE)
			, m_StencilFunc(GL_NONE)
			, m_StencilFail(GL_NONE)
			, m_StencilZFail(GL_NONE)
			, m_StencilPass(GL_NONE)
			, m_StencilRef(0)
			, m_StencilReadMask(0)
			, m_bStencilFunc(GL_NONE)
			, m_bStencilFail(GL_NONE)
			, m_bStencilZFail(GL_NONE)
			, m_bStencilPass(GL_NONE)
			, m_bStencilRef(0)
			, m_bStencilReadMask(0)
			, m_StencilWriteMask(0)
		{
		}

		// Depth test related flags
		uint32_t m_ZEnable : 2;
		uint32_t m_ZWriteEnable : 2;
		// Stencil related flags
		uint32_t m_StencilEnable : 2;
		uint32_t m_TwoSidedStencilMode : 2;

		GLenum m_ZFunc;

		// Front Stencil
		GLenum m_StencilFunc;
		GLenum m_StencilFail;
		GLenum m_StencilZFail;
		GLenum m_StencilPass;
		GLint m_StencilRef;
		GLuint m_StencilReadMask;


		// Back Stencil
		GLenum m_bStencilFunc;
		GLenum m_bStencilFail;
		GLenum m_bStencilZFail;
		GLenum m_bStencilPass;
		GLint m_bStencilRef;
		GLuint m_bStencilReadMask;

		GLuint m_StencilWriteMask;
	};

	//Mimics The Blend State Struct In Unreal Engine
	//https://www.khronos.org/registry/OpenGL-Refpages/es3/html/glEnable.xhtml
	//Venus only supports RGB and Alpha to set the mixing factor and mixing 
	//parameter respectively Keep your support with BGFX
	//It also supports GLES3.2 similar to Glenablei and OpenGL4.3 and above APIs
	struct OpenGLBlendState
	{
		//example:glBlendFunc defines the operation of blending for all draw buffers when it is enabled. 
		//glBlendFunci defines the operation of blending for a single draw buffer specified by buf when
		//enabled for that draw buffer.sfactorspecifies which method is used to scale the source 
		//color components. dfactor specifies which method is used to scale the destination color components. 
		struct DrawBuffer
		{
			//normal blend op and factor
			GLenum m_colorBlendOperation;
			GLenum m_colorSourceBlendFactor;
			GLenum m_colorDestBlendFacetor;
			//alpha blend op and factor--bgfx default is only that way
			GLenum m_alphaBlendOperation;
			GLenum m_alphaSourceBlendFactor;
			GLenum m_alphaDestBlendFactor;
			//位域--c++ API
			uint32_t m_supportBlendIndex : 2;
			uint32_t m_separateAlphaBlendEnable : 2;
			uint32_t m_alphaBlendEnable : 2;
			uint32_t m_enableBlendColor : 2;
			uint32_t m_colorWriteMaskR : 2;
			uint32_t m_colorWriteMaskG : 2;
			uint32_t m_colorWriteMaskB : 2;
			uint32_t m_colorWriteMaskA : 2;
			//Blend Color
			GLclampf m_blendColorR;
			GLclampf m_blendColorG;
			GLclampf m_blendColorB;
			GLclampf m_blendColorA;
		};
		OpenGLBlendState()
			: m_sampleAlphaToCoverage(GLTriState::INVALID)
		{
			//In UE4, the maximum simultaneous blending parameter is set to 8
			//In Bgfx,the maximum is 8
			m_drawBuffers.reserve(BGFX_CONFIG_MAX_FRAME_BUFFER_ATTACHMENTS);
			for (int i = 0; i < BGFX_CONFIG_MAX_FRAME_BUFFER_ATTACHMENTS; ++i)
			{
				DrawBuffer m_drawBuffer;
				m_drawBuffer.m_colorBlendOperation = GL_NONE;
				m_drawBuffer.m_colorSourceBlendFactor = GL_NONE;
				m_drawBuffer.m_colorDestBlendFacetor = GL_NONE;

				m_drawBuffer.m_alphaBlendOperation = GL_NONE;
				m_drawBuffer.m_alphaSourceBlendFactor = GL_NONE;
				m_drawBuffer.m_alphaDestBlendFactor = GL_NONE;
				m_drawBuffer.m_supportBlendIndex = GLTriState::INVALID;
				m_drawBuffer.m_separateAlphaBlendEnable = GLTriState::INVALID;
				m_drawBuffer.m_alphaBlendEnable = GLTriState::INVALID;
				m_drawBuffer.m_enableBlendColor = GLTriState::INVALID;
				m_drawBuffer.m_colorWriteMaskR = GLTriState::INVALID;
				m_drawBuffer.m_colorWriteMaskG = GLTriState::INVALID;
				m_drawBuffer.m_colorWriteMaskB = GLTriState::INVALID;
				m_drawBuffer.m_colorWriteMaskA = GLTriState::INVALID;

				m_drawBuffer.m_blendColorR = 0;
				m_drawBuffer.m_blendColorG = 0;
				m_drawBuffer.m_blendColorB = 0;
				m_drawBuffer.m_blendColorA = 0;
				m_drawBuffers.push_back(m_drawBuffer);
			}
		}
		//BGFX support this api,ue4 not support
		uint32_t m_sampleAlphaToCoverage : 2;
		std::vector<DrawBuffer> m_drawBuffers;
	};

	//hash值缓存机制
	//renderState分为:光栅化状态、深度模板状态、混合状态
	//key为hash值，value为三个状态的结构体
	//默认hashmap中存储一个默认状态值
	//假如缓存表中没有该hash值，那么就插入，如果有那就拿出来
	//解决问题：
	//drawcall的hash值为: 6|7|6|7|6
	//当这种问题出现是，将会每次drawcall都会进行解包处理
	struct OpenGLcachedState
	{
		OpenGLcachedState() {}
		OpenGLRasterizerState m_rasterizerState;
		OpenGLDepthStencilState m_depthStencilState;
		OpenGLBlendState m_blendState;
	};

	struct OpenGLStateFlag
	{
		OpenGLStateFlag()
			: m_renderstate(0)
			, m_stencil(0)
		{}

		bool operator ==(const OpenGLStateFlag& other)
		{
			return (m_renderstate == other.m_renderstate && m_stencil == other.m_stencil);
		}

		bool operator !=(const OpenGLStateFlag& other)
		{
			return (m_renderstate != other.m_renderstate || m_stencil != other.m_stencil);
		}

		uint64_t m_renderstate;
		uint64_t m_stencil;
	};

	//unreal engine ability to define:per-context state caching;
	//we ability to define:caching pre drawcall render state
	struct OpenGLStateContext
	{
		OpenGLStateContext()
			: m_scissor(0)
			, m_ScissorEnabled(GLTriState::INVALID)
			, m_rgba(0)
			, m_clearDepth(-1)
			, m_clearStencil(0xffff)
			, m_clearColor{ 0xffff , 0xffff , 0xffff , 0xffff }
			, m_states(NULL)
		{
			m_scissorRect.clear();
			m_viewportRect.clear();
		}

		OpenGLStateContext(const OpenGLStateContext& other) = delete;
		void operator =(const OpenGLStateContext& other) = delete;

		OpenGLStateFlag m_statesflag;
		uint16_t m_scissor;
		uint32_t m_rgba;	//bgfx blend factor flag
		uint32_t     m_ScissorEnabled : 2;
		Rect     m_scissorRect;
		Rect     m_viewportRect;

		float    m_clearDepth;
		uint16_t m_clearStencil;
		uint16_t m_clearColor[4];

		OpenGLcachedState* m_states;
	};

	template <typename K, typename V>
	struct StateCacheT
	{
		StateCacheT()
			: pop_pointer(0)
		{
			m_index.reserve(m_defaultcachecount);
			m_data.reserve(m_defaultcachecount);

			for (int i = 0; i < m_defaultcachecount; i++)
			{
				m_data.push_back(V());
			}
		}

		V* find(K& _key)
		{
			for (int i = 0; i < m_index.size(); i++)
			{
				if (m_index[i] == _key)
				{
					return (m_data.data() + i);
				}
			}
			return NULL;
		}

		V* add(K& _key)
		{
			int currentSize = m_index.size();
			if (currentSize < m_defaultcachecount)
			{
				m_index.push_back(_key);
				return (m_data.data() + currentSize);
			}
			else
			{
				int _pop_pointer = pop_pointer;
				pop_pointer = (pop_pointer + 1) % m_defaultcachecount;

				m_index[_pop_pointer] = _key;
				return (m_data.data() + _pop_pointer);
			}
		}

		const static int m_defaultcachecount = 100;

	private:

		typedef std::vector<K> IndexArray;
		typedef std::vector<V> DataArray;

		int pop_pointer;
		IndexArray m_index;
		DataArray m_data;
	};

	class UniformStateCache
	{
	public:
		struct f1 { float val[1]; bool operator ==(const f1& rhs) { return val[0] = rhs.val[0]; } };
		struct f2 { float val[2]; bool operator ==(const f2& rhs) { const uint64_t* a = (const uint64_t*)this; const uint64_t* b = (const uint64_t*)&rhs; return a[0] == b[0]; } };
		struct f3 { float val[3]; bool operator ==(const f3& rhs) { const uint64_t* a = (const uint64_t*)this; const uint64_t* b = (const uint64_t*)&rhs; return a[0] == b[0] && val[2] == val[2]; } };
		struct f4 { float val[4]; bool operator ==(const f4& rhs) { const uint64_t* a = (const uint64_t*)this; const uint64_t* b = (const uint64_t*)&rhs; return a[0] == b[0] && a[1] == b[1]; } };
		struct f3x3 { float val[9]; bool operator ==(const f3x3& rhs) { const uint64_t* a = (const uint64_t*)this; const uint64_t* b = (const uint64_t*)&rhs; return a[0] == b[0] && a[1] == b[1] && a[2] == b[2] && a[3] == b[3] && ((const uint32_t*)a)[8] == ((const uint32_t*)b)[8]; } };
		struct f4x4 { float val[16]; bool operator ==(const f4x4& rhs) { const uint64_t* a = (const uint64_t*)this; const uint64_t* b = (const uint64_t*)&rhs; return a[0] == b[0] && a[1] == b[1] && a[2] == b[2] && a[3] == b[3] && a[4] == b[4] && a[5] == b[5] && a[6] == b[6] && a[7] == b[7]; } };

		typedef stl::unordered_map<uint64_t, int>  IMap;
		typedef stl::unordered_map<uint64_t, f1>   F1Map;
		typedef stl::unordered_map<uint64_t, f2>   F2Map;
		typedef stl::unordered_map<uint64_t, f3>   F3Map;
		typedef stl::unordered_map<uint64_t, f4>   F4Map;
		typedef stl::unordered_map<uint64_t, f3x3> F3x3Map;
		typedef stl::unordered_map<uint64_t, f4x4> F4x4Map;

		UniformStateCache()
			: m_currentProgram(0)
		{
		}

		// Inserts the new value into the uniform cache, and returns true
		// if the old value was different than the new one.
		template<typename T>
		bool updateUniformCache(uint32_t loc, const T& value)
		{
			if (BX_ENABLED(BGFX_GL_CONFIG_UNIFORM_CACHE))
			{
				// Uniform state cache for various types.
				stl::unordered_map<uint64_t, T>& uniformCacheMap = getUniformCache<T>();

				uint64_t key = (uint64_t(m_currentProgram) << 32) | loc;

				auto iter = uniformCacheMap.find(key);

				// Not found in the cache? Add it.
				if (iter == uniformCacheMap.end())
				{
					uniformCacheMap[key] = value;
					return true;
				}

				// Value in the cache was the same as new state? Skip reuploading this state.
				if (iter->second == value)
				{
					return false;
				}

				iter->second = value;
			}

			return true;
		}

		void saveCurrentProgram(GLuint program)
		{
			if (BX_ENABLED(BGFX_GL_CONFIG_UNIFORM_CACHE))
			{
				m_currentProgram = program;
			}
		}

	private:
		GLuint m_currentProgram;

		IMap    m_uniformiCacheMap;
		F1Map   m_uniformf1CacheMap;
		F2Map   m_uniformf2CacheMap;
		F3Map   m_uniformf3CacheMap;
		F4Map   m_uniformf4CacheMap;
		F3x3Map m_uniformf3x3CacheMap;
		F4x4Map m_uniformf4x4CacheMap;

		template<typename T>
		stl::unordered_map<uint64_t, T>& getUniformCache();
	};

	template<>
	inline UniformStateCache::IMap& UniformStateCache::getUniformCache() { return m_uniformiCacheMap; }

	template<>
	inline UniformStateCache::F1Map& UniformStateCache::getUniformCache() { return m_uniformf1CacheMap; }

	template<>
	inline UniformStateCache::F2Map& UniformStateCache::getUniformCache() { return m_uniformf2CacheMap; }

	template<>
	inline UniformStateCache::F3Map& UniformStateCache::getUniformCache() { return m_uniformf3CacheMap; }

	template<>
	inline UniformStateCache::F4Map& UniformStateCache::getUniformCache() { return m_uniformf4CacheMap; }

	template<>
	inline UniformStateCache::F3x3Map& UniformStateCache::getUniformCache() { return m_uniformf3x3CacheMap; }

	template<>
	inline UniformStateCache::F4x4Map& UniformStateCache::getUniformCache() { return m_uniformf4x4CacheMap; }

	class SamplerStateCache
	{
	public:
		GLuint add(uint32_t _hash);
		GLuint find(uint32_t _hash);
		void invalidate(uint32_t _hash);
		void invalidate();

		uint32_t getCount() const
		{
			return uint32_t(m_hashMap.size());
		}

	private:
		typedef stl::unordered_map<uint32_t, GLuint> HashMap;
		HashMap m_hashMap;
	};

	struct IndexBufferGL
	{
		void create(uint32_t _size, void* _data, uint16_t _flags);
		void update(uint32_t _offset, uint32_t _size, void* _data, bool _discard = false);
		void destroy();

		GLuint m_id;
		uint32_t m_size;
		uint16_t m_flags;
	};

	struct VertexBufferGL
	{
		void create(uint32_t _size, void* _data, VertexLayoutHandle _layoutHandle, uint16_t _flags);
		void update(uint32_t _offset, uint32_t _size, void* _data, bool _discard = false);
		void destroy();

		GLuint m_id;
		GLenum m_target;
		uint32_t m_size;
		VertexLayoutHandle m_layoutHandle;
	};

	struct TextureGL
	{
		TextureGL()
			: m_id(0)
			, m_rbo(0)
			, m_target(GL_TEXTURE_2D)
			, m_fmt(GL_ZERO)
			, m_type(GL_ZERO)
			, m_flags(0)
			, m_currentSamplerHash(UINT32_MAX)
			, m_numMips(0)
			, m_autoGenMipmap(false)
			, m_refHandle(-1)
		{
		}

		bool init(GLenum _target, uint32_t _width, uint32_t _height, uint32_t _depth, uint8_t _numMips, uint64_t _flags, bool autoGenMipmap, int32_t refHandle);
		void create(const Memory* _mem, uint64_t _flags, uint8_t _skip);
		void destroy();
		void overrideInternal(uintptr_t _ptr);
		void update(uint8_t _side, uint8_t _mip, const Rect& _rect, uint16_t _z, uint16_t _depth, uint16_t _pitch, const Memory* _mem);
		void setSamplerState(uint32_t _flags, const float _rgba[4]);
		void commit(uint32_t _stage, uint32_t _flags, const float _palette[][4]);
		void resolve(uint8_t _resolve) const;

		bool isCubeMap() const
		{
			return 0
				|| GL_TEXTURE_CUBE_MAP == m_target
				|| GL_TEXTURE_CUBE_MAP_ARRAY == m_target
				;
		}

		GLuint m_id;
		GLuint m_rbo;
		GLenum m_target;
		GLenum m_fmt;
		GLenum m_type;
		uint64_t m_flags;
		uint32_t m_currentSamplerHash;
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_depth;
		uint32_t m_numLayers;
		uint8_t m_numMips;
		uint8_t m_requestedFormat;
		uint8_t m_textureFormat;
		bool m_autoGenMipmap;
		int32_t m_refHandle;
	};

	struct ShaderGL
	{
		ShaderGL()
			: m_id(0)
			, m_type(0)
			, m_hash(0)
		{
		}

		void create(const Memory* _mem);
		void destroy();

		GLuint m_id;
		GLenum m_type;
		uint32_t m_hash;

		std::map<std::string, uint16_t> m_UniformBlocks;
		std::map<std::string, uint16_t> m_TextureUniforms;
	};

	struct FrameBufferGL
	{
		FrameBufferGL()
			: m_swapChain(NULL)
			, m_denseIdx(UINT16_MAX)
			, m_num(0)
			, m_needPresent(false)
			, m_refHandle(-1)
		{
			bx::memSet(m_fbo, 0, sizeof(m_fbo));
		}

		void create(uint8_t _num, const Attachment* _attachment);
		void create(uint16_t _denseIdx, void* _nwh, uint32_t _width, uint32_t _height, TextureFormat::Enum _format, TextureFormat::Enum _depthFormat);
		void create(int32_t refHandle, uint32_t _width, uint32_t _height, TextureFormat::Enum _format, TextureFormat::Enum _depthFormat);
		void postReset();
		uint16_t destroy();
		void resolve();
		void discard(uint16_t _flags);

		SwapChainGL* m_swapChain;
		GLuint m_fbo[2];
		uint32_t m_width;
		uint32_t m_height;
		uint16_t m_denseIdx;
		uint8_t  m_num;
		uint8_t  m_numTh;
		bool     m_needPresent;
		int32_t  m_refHandle;
		Attachment m_attachment[BGFX_CONFIG_MAX_FRAME_BUFFER_ATTACHMENTS];
	};

	struct ProgramGL
	{
		ProgramGL()
			: m_id(0)
			, m_constantBuffer(NULL)
		{
			m_instanceData[0] = -1;
		}

		void create(const ShaderGL& _vsh, const ShaderGL& _fsh);
		void destroy();
		void init();

		void bindAttributesBegin();
		void bindAttributes(const VertexLayout& _layout, uint32_t _baseVertex = 0);
		void bindInstanceData(uint32_t _stride, uint32_t _baseVertex = 0) const;
		void bindAttributesEnd();
		void unbindInstanceData() const;
		void unbindAttributes();

		GLuint m_id;

		uint8_t m_unboundUsedAttrib[Attrib::Count]; // For tracking unbound used attributes between begin()/end().
		uint8_t m_usedCount;
		uint8_t m_used[Attrib::Count]; // Dense.
		GLint m_attributes[Attrib::Count]; // Sparse.
		GLint m_instanceData[BGFX_CONFIG_MAX_INSTANCE_DATA_COUNT + 1];

		UniformBuffer* m_constantBuffer;
	};

	struct TimerQueryGL
	{
		TimerQueryGL();

		void create();
		void destroy();
		uint32_t begin(uint32_t _resultIdx);
		void end(uint32_t _idx);
		bool update();

		struct Result
		{
			void reset()
			{
				m_begin = 0;
				m_end = 0;
				m_pending = 0;
			}

			uint64_t m_begin;
			uint64_t m_end;
			uint32_t m_pending;
		};

		struct Query
		{
			GLuint   m_begin;
			GLuint   m_end;
			uint32_t m_resultIdx;
			bool     m_ready;
		};

		Result m_result[BGFX_CONFIG_MAX_VIEWS + 1];

		Query m_query[BGFX_CONFIG_MAX_VIEWS * 4];
		bx::RingBufferControl m_control;
	};

	struct OcclusionQueryGL
	{
		OcclusionQueryGL()
			: m_control(BX_COUNTOF(m_query))
		{
		}

		void create();
		void destroy();
		void begin(Frame* _render, OcclusionQueryHandle _handle);
		void end();
		void resolve(Frame* _render, bool _wait = false);
		void invalidate(OcclusionQueryHandle _handle);

		struct Query
		{
			GLuint m_id;
			OcclusionQueryHandle m_handle;
		};

		Query m_query[BGFX_CONFIG_MAX_OCCLUSION_QUERIES];
		bx::RingBufferControl m_control;
	};

}
}
