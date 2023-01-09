#include "../bgfx_p.h"

#if (BGFX_CONFIG_RENDERER_OPENGLES || BGFX_CONFIG_RENDERER_OPENGL)
#	include "OpenGLDrv.h"
#	include <bx/timer.h>
#	include <bx/uint32_t.h>
#	include "../emscripten.h"

namespace bgfx { namespace gl
{
	extern GLenum getGlError();

	static const char* s_attribName[] =
	{
		"a_position",
		"a_normal",
		"a_tangent",
		"a_bitangent",
		"a_color0",
		"a_color1",
		"a_color2",
		"a_color3",
		"a_indices",
		"a_weight",
		"a_texcoord0",
		"a_texcoord1",
		"a_texcoord2",
		"a_texcoord3",
		"a_texcoord4",
		"a_texcoord5",
		"a_texcoord6",
		"a_texcoord7",
	};
	BX_STATIC_ASSERT(Attrib::Count == BX_COUNTOF(s_attribName));

	static const char* s_hlccAttribName[] =
	{
		"in_POSITION",
		"in_NORMAL",
		"in_TANGENT",
		"in_BINORMAL",
		"in_COLOR0",
		"in_COLOR1",
		"in_COLOR2",
		"in_COLOR3",
		"in_BLENDINDICES",
		"in_BLENDWEIGHT",
		"in_TEXCOORD0",
		"in_TEXCOORD1",
		"in_TEXCOORD2",
		"in_TEXCOORD3",
		"in_TEXCOORD4",
		"in_TEXCOORD5",
		"in_TEXCOORD6",
		"in_TEXCOORD7",
	};
	BX_STATIC_ASSERT(Attrib::Count == BX_COUNTOF(s_hlccAttribName));

	static const char* s_instanceDataName[] =
	{
		"in_TEXCOORD4",
		"in_TEXCOORD5",
		"in_TEXCOORD6",
		"in_TEXCOORD7"
	};
	BX_STATIC_ASSERT(BGFX_CONFIG_MAX_INSTANCE_DATA_COUNT == BX_COUNTOF(s_instanceDataName));

	static const GLenum s_attribType[] =
	{
		GL_UNSIGNED_BYTE,            // Uint8
		GL_UNSIGNED_INT_10_10_10_2,  // Uint10
		GL_SHORT,                    // Int16
		GL_HALF_FLOAT,               // Half
		GL_FLOAT,                    // Float
	};
	BX_STATIC_ASSERT(AttribType::Count == BX_COUNTOF(s_attribType));

	void dumpExtensions(const char* _extensions)
	{
		if (NULL != _extensions)
		{
			char name[1024];
			const char* pos = _extensions;
			const char* end = _extensions + bx::strLen(_extensions);
			while (pos < end)
			{
				uint32_t len;
				bx::StringView space = bx::strFind(pos, ' ');
				if (!space.isEmpty())
				{
					len = bx::uint32_min(sizeof(name), (uint32_t)(space.getPtr() - pos));
				}
				else
				{
					len = bx::uint32_min(sizeof(name), (uint32_t)bx::strLen(pos));
				}

				bx::strCopy(name, BX_COUNTOF(name), pos, len);
				name[len] = '\0';

				BX_TRACE("\t%s", name);

				pos += len + 1;
			}
		}
	}

	static uint64_t s_currentlyEnabledVertexAttribArrays = 0;
	static uint64_t s_vertexAttribArraysPendingDisable = 0;
	static uint64_t s_vertexAttribArraysPendingEnable = 0;

	void lazyEnableVertexAttribArray(GLuint index)
	{
		if (BX_ENABLED(BX_PLATFORM_EMSCRIPTEN))
		{
			if (index >= 64)
			{
				// On WebGL platform calling out to WebGL API is detrimental to performance, so optimize
				// out redundant API calls to glEnable/DisableVertexAttribArray.
				GL_CHECK(glEnableVertexAttribArray(index));
				return;
			}

			const uint64_t mask = UINT64_C(1) << index;
			s_vertexAttribArraysPendingEnable |= mask & (~s_currentlyEnabledVertexAttribArrays);
			s_vertexAttribArraysPendingDisable &= ~mask;
		}
		else
		{
			GL_CHECK(glEnableVertexAttribArray(index));
		}
	}

	void lazyDisableVertexAttribArray(GLuint index)
	{
		if (BX_ENABLED(BX_PLATFORM_EMSCRIPTEN))
		{
			if (index >= 64)
			{
				// On WebGL platform calling out to WebGL API is detrimental to performance, so optimize
				// out redundant API calls to glEnable/DisableVertexAttribArray.
				GL_CHECK(glDisableVertexAttribArray(index));
				return;
			}

			const uint64_t mask = UINT64_C(1) << index;
			s_vertexAttribArraysPendingDisable |= mask & s_currentlyEnabledVertexAttribArrays;
			s_vertexAttribArraysPendingEnable &= ~mask;
		}
		else
		{
			GL_CHECK(glDisableVertexAttribArray(index));
		}
	}

	void applyLazyEnabledVertexAttributes()
	{
		if (BX_ENABLED(BX_PLATFORM_EMSCRIPTEN))
		{
			while (s_vertexAttribArraysPendingDisable)
			{
				uint32_t index = bx::uint32_cnttz(s_vertexAttribArraysPendingDisable);
				uint64_t mask = ~(UINT64_C(1) << index);
				s_vertexAttribArraysPendingDisable &= mask;
				s_currentlyEnabledVertexAttribArrays &= mask;
				GL_CHECK(glDisableVertexAttribArray(index));
			}

			while (s_vertexAttribArraysPendingEnable)
			{
				uint32_t index = bx::uint32_cnttz(s_vertexAttribArraysPendingEnable);
				uint64_t mask = UINT64_C(1) << index;
				s_vertexAttribArraysPendingEnable &= ~mask;
				s_currentlyEnabledVertexAttribArrays |= mask;
				GL_CHECK(glEnableVertexAttribArray(index));
			}
		}
	}
    
	const char* glslTypeName(GLuint _type)
	{
#define GLSL_TYPE(_ty) case _ty: return #_ty

		switch (_type)
		{
			GLSL_TYPE(GL_BOOL);
			GLSL_TYPE(GL_INT);
			GLSL_TYPE(GL_INT_VEC2);
			GLSL_TYPE(GL_INT_VEC3);
			GLSL_TYPE(GL_INT_VEC4);
			GLSL_TYPE(GL_UNSIGNED_INT);
			GLSL_TYPE(GL_UNSIGNED_INT_VEC2);
			GLSL_TYPE(GL_UNSIGNED_INT_VEC3);
			GLSL_TYPE(GL_UNSIGNED_INT_VEC4);
			GLSL_TYPE(GL_FLOAT);
			GLSL_TYPE(GL_FLOAT_VEC2);
			GLSL_TYPE(GL_FLOAT_VEC3);
			GLSL_TYPE(GL_FLOAT_VEC4);
			GLSL_TYPE(GL_FLOAT_MAT2);
			GLSL_TYPE(GL_FLOAT_MAT3);
			GLSL_TYPE(GL_FLOAT_MAT4);

			GLSL_TYPE(GL_SAMPLER_2D);
			GLSL_TYPE(GL_SAMPLER_2D_ARRAY);
			GLSL_TYPE(GL_SAMPLER_2D_MULTISAMPLE);

			GLSL_TYPE(GL_INT_SAMPLER_2D);
			GLSL_TYPE(GL_INT_SAMPLER_2D_ARRAY);
			GLSL_TYPE(GL_INT_SAMPLER_2D_MULTISAMPLE);

			GLSL_TYPE(GL_UNSIGNED_INT_SAMPLER_2D);
			GLSL_TYPE(GL_UNSIGNED_INT_SAMPLER_2D_ARRAY);
			GLSL_TYPE(GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE);

			GLSL_TYPE(GL_SAMPLER_2D_SHADOW);
			GLSL_TYPE(GL_SAMPLER_2D_ARRAY_SHADOW);

			GLSL_TYPE(GL_SAMPLER_3D);
			GLSL_TYPE(GL_INT_SAMPLER_3D);
			GLSL_TYPE(GL_UNSIGNED_INT_SAMPLER_3D);

			GLSL_TYPE(GL_SAMPLER_CUBE);
			GLSL_TYPE(GL_INT_SAMPLER_CUBE);
			GLSL_TYPE(GL_UNSIGNED_INT_SAMPLER_CUBE);

			GLSL_TYPE(GL_IMAGE_1D);
			GLSL_TYPE(GL_INT_IMAGE_1D);
			GLSL_TYPE(GL_UNSIGNED_INT_IMAGE_1D);

			GLSL_TYPE(GL_IMAGE_2D);
			GLSL_TYPE(GL_IMAGE_2D_ARRAY);
			GLSL_TYPE(GL_INT_IMAGE_2D);
			GLSL_TYPE(GL_UNSIGNED_INT_IMAGE_2D);

			GLSL_TYPE(GL_IMAGE_3D);
			GLSL_TYPE(GL_INT_IMAGE_3D);
			GLSL_TYPE(GL_UNSIGNED_INT_IMAGE_3D);

			GLSL_TYPE(GL_IMAGE_CUBE);
			GLSL_TYPE(GL_INT_IMAGE_CUBE);
			GLSL_TYPE(GL_UNSIGNED_INT_IMAGE_CUBE);
		}

#undef GLSL_TYPE

		BX_ASSERT(false, "Unknown GLSL type? %x", _type);
		return "UNKNOWN GLSL TYPE!";
	}

	const char* glEnumName(GLenum _enum)
	{
#define GLENUM(_ty) case _ty: return #_ty

		switch (_enum)
		{
			GLENUM(GL_TEXTURE);
			GLENUM(GL_RENDERBUFFER);

			GLENUM(GL_INVALID_ENUM);
			GLENUM(GL_INVALID_FRAMEBUFFER_OPERATION);
			GLENUM(GL_INVALID_VALUE);
			GLENUM(GL_INVALID_OPERATION);
			GLENUM(GL_OUT_OF_MEMORY);

			GLENUM(GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT);
			GLENUM(GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT);
//			GLENUM(GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER);
//			GLENUM(GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER);
			GLENUM(GL_FRAMEBUFFER_UNSUPPORTED);
		}

#undef GLENUM

		BX_WARN(false, "Unknown enum? %x", _enum);
		return "<GLenum?>";
	}

	UniformType::Enum convertGlType(GLenum _type)
	{
		switch (_type)
		{
		case GL_INT:
		case GL_UNSIGNED_INT:
			return UniformType::Sampler;

		case GL_FLOAT:
			return UniformType::Float;
		case GL_FLOAT_VEC2:
			return UniformType::Vec2;
		case GL_FLOAT_VEC3:
			return UniformType::Vec3;
		case GL_FLOAT_VEC4:
			return UniformType::Vec4;

		case GL_FLOAT_MAT2:
			break;

		case GL_FLOAT_MAT3:
			return UniformType::Mat3;

		case GL_FLOAT_MAT4:
			return UniformType::Mat4;

		case GL_SAMPLER_2D:
		case GL_SAMPLER_2D_ARRAY:
		case GL_SAMPLER_2D_MULTISAMPLE:

		case GL_INT_SAMPLER_2D:
		case GL_INT_SAMPLER_2D_ARRAY:
		case GL_INT_SAMPLER_2D_MULTISAMPLE:

		case GL_UNSIGNED_INT_SAMPLER_2D:
		case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
		case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:

		case GL_SAMPLER_2D_SHADOW:
		case GL_SAMPLER_2D_ARRAY_SHADOW:

		case GL_SAMPLER_3D:
		case GL_INT_SAMPLER_3D:
		case GL_UNSIGNED_INT_SAMPLER_3D:

		case GL_SAMPLER_CUBE:
		case GL_INT_SAMPLER_CUBE:
		case GL_UNSIGNED_INT_SAMPLER_CUBE:

		case GL_IMAGE_1D:
		case GL_INT_IMAGE_1D:
		case GL_UNSIGNED_INT_IMAGE_1D:

		case GL_IMAGE_2D:
		case GL_IMAGE_2D_ARRAY:
		case GL_INT_IMAGE_2D:
		case GL_UNSIGNED_INT_IMAGE_2D:

		case GL_IMAGE_3D:
		case GL_INT_IMAGE_3D:
		case GL_UNSIGNED_INT_IMAGE_3D:

		case GL_IMAGE_CUBE:
		case GL_INT_IMAGE_CUBE:
		case GL_UNSIGNED_INT_IMAGE_CUBE:
			return UniformType::Sampler;
		};

		BX_ASSERT(false, "Unrecognized GL type 0x%04x.", _type);
		return UniformType::End;
	}

	void ProgramGL::create(const ShaderGL& _vsh, const ShaderGL& _fsh)
	{
		m_id = glCreateProgram();
		BX_TRACE("Program create: GL%d: GL%d, GL%d", m_id, _vsh.m_id, _fsh.m_id);

		const uint64_t id = (uint64_t(_vsh.m_hash)<<32) | _fsh.m_hash;
		const bool cached = s_renderGL->programFetchFromCache(m_id, id);

		if (!cached)
		{
			GLint linked = 0;
			if (0 != _vsh.m_id)
			{
				GL_CHECK(glAttachShader(m_id, _vsh.m_id) );

				if (0 != _fsh.m_id)
				{
					GL_CHECK(glAttachShader(m_id, _fsh.m_id) );
				}

				GL_CHECK(glLinkProgram(m_id) );
				GL_CHECK(glGetProgramiv(m_id, GL_LINK_STATUS, &linked) );

				if (0 == linked)
				{
					char log[1024];
					GL_CHECK(glGetProgramInfoLog(m_id, sizeof(log), NULL, log) );
					BX_TRACE("%d: %s", linked, log);
				}
			}

			if (0 == linked)
			{
				BX_WARN(0 != _vsh.m_id, "Invalid vertex/compute shader.");
				GL_CHECK(glDeleteProgram(m_id) );
				m_usedCount = 0;
				m_id = 0;
				return;
			}

			s_renderGL->programCache(m_id, id);
		}

		init();

		if (!cached
		&&  s_renderGL->m_workaround.m_detachShader)
		{
			// Must be after init, otherwise init might fail to lookup shader
			// info (NVIDIA Tegra 3 OpenGL ES 2.0 14.01003).
			GL_CHECK(glDetachShader(m_id, _vsh.m_id) );

			if (0 != _fsh.m_id)
			{
				GL_CHECK(glDetachShader(m_id, _fsh.m_id) );
			}
		}
	}

	void ProgramGL::destroy()
	{
		if (NULL != m_constantBuffer)
		{
			UniformBuffer::destroy(m_constantBuffer);
			m_constantBuffer = NULL;
		}

		if (0 != m_id)
		{
			s_renderGL->setProgram(0);
			GL_CHECK(glDeleteProgram(m_id) );
			m_id = 0;
		}
	}

	void ProgramGL::init()
	{
		GLint activeAttribs  = 0;
		GLint activeUniforms = 0;
		GLint activeBuffers  = 0;

#if BGFX_CONFIG_RENDERER_OPENGL >= 31
		GL_CHECK(glBindFragDataLocation(m_id, 0, "bgfx_FragColor") );
#endif // BGFX_CONFIG_RENDERER_OPENGL >= 31

		GLint max0, max1;

		bool piqSupported = true
			&& OpenGLBase::s_extension[Extension::ARB_program_interface_query     ].m_supported
			&& OpenGLBase::s_extension[Extension::ARB_shader_storage_buffer_object].m_supported
			;

		if (piqSupported)
		{
			GL_CHECK(glGetProgramInterfaceiv(m_id, GL_PROGRAM_INPUT,   GL_ACTIVE_RESOURCES, &activeAttribs ) );
			GL_CHECK(glGetProgramInterfaceiv(m_id, GL_UNIFORM,         GL_ACTIVE_RESOURCES, &activeUniforms) );
			GL_CHECK(glGetProgramInterfaceiv(m_id, GL_BUFFER_VARIABLE, GL_ACTIVE_RESOURCES, &activeBuffers ) );
			GL_CHECK(glGetProgramInterfaceiv(m_id, GL_PROGRAM_INPUT,   GL_MAX_NAME_LENGTH,  &max0          ) );
			GL_CHECK(glGetProgramInterfaceiv(m_id, GL_UNIFORM,         GL_MAX_NAME_LENGTH,  &max1          ) );
		}
		else
		{
			GL_CHECK(glGetProgramiv(m_id, GL_ACTIVE_ATTRIBUTES, &activeAttribs ) );
			GL_CHECK(glGetProgramiv(m_id, GL_ACTIVE_UNIFORMS,   &activeUniforms) );

			GL_CHECK(glGetProgramiv(m_id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max0) );
			GL_CHECK(glGetProgramiv(m_id, GL_ACTIVE_UNIFORM_MAX_LENGTH,   &max1) );
		}

		uint32_t maxLength = bx::uint32_max(max0, max1);
		char* name = (char*)alloca(maxLength + 1);

		BX_TRACE("Program %d", m_id);
		BX_TRACE("Attributes (%d):", activeAttribs);
		for (int32_t ii = 0; ii < activeAttribs; ++ii)
		{
			GLint size;
			GLenum type = 0;

			if (piqSupported)
			{
				GL_CHECK(glGetProgramResourceName(m_id, GL_PROGRAM_INPUT, ii, maxLength + 1, &size, name) );
				GLenum typeProp[] = { GL_TYPE };
				GL_CHECK(glGetProgramResourceiv(m_id
					, GL_PROGRAM_INPUT
					, ii
					, BX_COUNTOF(typeProp)
					, typeProp
					, 1
					, NULL
					, (GLint *)&type)
					);
			}
			else
			{
				GL_CHECK(glGetActiveAttrib(m_id, ii, maxLength + 1, NULL, &size, &type, name) );
			}

			BX_TRACE("\t%s %s is at location %d"
				, glslTypeName(type)
				, name
				, glGetAttribLocation(m_id, name)
				);
		}

		BX_TRACE("Uniforms (%d):", activeUniforms);
		for (int32_t ii = 0; ii < activeUniforms; ++ii)
		{
			struct VariableInfo
			{
				GLenum type;
				GLint  loc;
				GLint  num;
			};
			VariableInfo vi;
			GLenum props[] = { GL_TYPE, GL_LOCATION, GL_ARRAY_SIZE };

			GLenum gltype;
			GLint num;
			GLint loc;

			if (piqSupported)
			{
				GL_CHECK(glGetProgramResourceiv(m_id
					, GL_UNIFORM
					, ii
					, BX_COUNTOF(props)
					, props
					, BX_COUNTOF(props)
					, NULL
					, (GLint*)&vi
					) );

				GL_CHECK(glGetProgramResourceName(m_id
					, GL_UNIFORM
					, ii
					, maxLength + 1
					, NULL
					, name
					) );

				gltype = vi.type;
				loc    = vi.loc;
				num    = vi.num;
			}
			else
			{
				GL_CHECK(glGetActiveUniform(m_id, ii, maxLength + 1, NULL, &num, &gltype, name) );
				loc = glGetUniformLocation(m_id, name);
			}

			num = bx::uint32_max(num, 1);

			int32_t offset = 0;
			const bx::StringView array = bx::strFind(name, '[');
			if (!array.isEmpty() )
			{
				name[array.getPtr() - name] = '\0';
				BX_TRACE("--- %s", name);
				const bx::StringView end = bx::strFind(array.getPtr()+1, ']');
				bx::fromString(&offset, bx::StringView(array.getPtr()+1, end.getPtr() ) );
			}

			UniformType::Enum type = convertGlType(gltype);
			const UniformRegInfo* info = s_renderGL->m_uniformReg.find(name, type);
			BX_ASSERT(NULL != info, "User defined uniform '%s' is not found, it won't be set.", name);

			if (NULL != info)
			{
				if (NULL == m_constantBuffer)
				{
					m_constantBuffer = UniformBuffer::create(1024);
				}

				m_constantBuffer->writeUniformHandle(type, 0, info->m_handle, uint16_t(num));
				m_constantBuffer->write(loc);
				BX_TRACE("store %s %d", name, info->m_handle);
			}

			BX_TRACE("\tuniform %s %s%s is at location %d, size %d, offset %d"
				, glslTypeName(gltype)
				, name
				, ""
				, loc
				, num
				, offset
				);
			BX_UNUSED(offset);
		}

		if (NULL != m_constantBuffer)
		{
			m_constantBuffer->finish();
		}

		if (piqSupported)
		{
			struct VariableInfo
			{
				GLenum type;
			};
			VariableInfo vi;
			GLenum props[] = { GL_TYPE };

			BX_TRACE("Buffers (%d):", activeBuffers);
			for (int32_t ii = 0; ii < activeBuffers; ++ii)
			{
				GL_CHECK(glGetProgramResourceiv(m_id
					, GL_BUFFER_VARIABLE
					, ii
					, BX_COUNTOF(props)
					, props
					, BX_COUNTOF(props)
					, NULL
					, (GLint*)&vi
					) );

				GL_CHECK(glGetProgramResourceName(m_id
					, GL_BUFFER_VARIABLE
					, ii
					, maxLength + 1
					, NULL
					, name
					) );

				BX_TRACE("\t%s %s at %d"
					, glslTypeName(vi.type)
					, name
					, 0 //vi.loc
					);
			}
		}

		bx::memSet(m_attributes, 0xff, sizeof(m_attributes) );
		uint32_t used = 0;
		for (uint8_t ii = 0; ii < Attrib::Count; ++ii)
		{
			GLint loc = glGetAttribLocation(m_id, s_hlccAttribName[ii]);
			if (-1 != loc)
			{
				BX_TRACE("attr %s: %d", s_hlccAttribName[ii], loc);
				m_attributes[ii] = loc;
				m_used[used++] = ii;
			}
		}
		BX_ASSERT(used < BX_COUNTOF(m_used), "Out of bounds %d > array size %d.", used, Attrib::Count);
		m_usedCount = (uint8_t)used;

		used = 0;
		for (uint32_t ii = 0; ii < BX_COUNTOF(s_instanceDataName); ++ii)
		{
			GLuint loc = glGetAttribLocation(m_id, s_instanceDataName[ii]);
			if (GLuint(-1) != loc)
			{
				BX_TRACE("instance data %s: %d", s_instanceDataName[ii], loc);
				m_instanceData[used++] = loc;
			}
		}
		BX_ASSERT(used < BX_COUNTOF(m_instanceData)
			, "Out of bounds %d > array size %d."
			, used
			, BX_COUNTOF(m_instanceData)
			);
		m_instanceData[used] = -1;
	}

	void ProgramGL::bindAttributesBegin()
	{
		bx::memCopy(m_unboundUsedAttrib, m_used, sizeof(m_unboundUsedAttrib));
	}

	void ProgramGL::bindAttributes(const VertexLayout& _layout, uint32_t _baseVertex)
	{
		for (uint32_t ii = 0, iiEnd = m_usedCount; ii < iiEnd; ++ii)
		{
			Attrib::Enum attr = Attrib::Enum(m_used[ii]);
			GLint loc = m_attributes[attr];

			uint8_t num;
			AttribType::Enum type;
			bool normalized;
			bool asInt;
			_layout.decode(attr, num, type, normalized, asInt);

			if (-1 != loc)
			{
				if (UINT16_MAX != _layout.m_attributes[attr])
				{
					lazyEnableVertexAttribArray(loc);
					GL_CHECK(OpenGL::VertexAttribDivisor(loc, 0) );

					uint32_t baseVertex = _baseVertex*_layout.m_stride + _layout.m_offset[attr];
					if ( (BX_ENABLED(BGFX_CONFIG_RENDERER_OPENGL >= 30) || OpenGL::Support_GLES3_0())
					&&  !isFloat(type)
					&&  !normalized)
					{
						GL_CHECK(glVertexAttribIPointer(loc
							, num
							, s_attribType[type]
							, _layout.m_stride
							, (void*)(uintptr_t)baseVertex)
							);
					}
					else
					{
						GL_CHECK(glVertexAttribPointer(loc
							, num
							, s_attribType[type]
							, normalized
							, _layout.m_stride
							, (void*)(uintptr_t)baseVertex)
							);
					}

					m_unboundUsedAttrib[ii] = Attrib::Count;
				}
			}
		}
	}

	void ProgramGL::bindInstanceData(uint32_t _stride, uint32_t _baseVertex) const
	{
		uint32_t baseVertex = _baseVertex;
		for (uint32_t ii = 0; -1 != m_instanceData[ii]; ++ii)
		{
			GLint loc = m_instanceData[ii];
			lazyEnableVertexAttribArray(loc);
			GL_CHECK(glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, _stride, (void*)(uintptr_t)baseVertex));
			GL_CHECK(glVertexAttribDivisor(loc, 1));
			baseVertex += 16;
		}
	}

	void ProgramGL::bindAttributesEnd()
	{
		for (uint32_t ii = 0, iiEnd = m_usedCount; ii < iiEnd; ++ii)
		{
			if (Attrib::Count != m_unboundUsedAttrib[ii])
			{
				Attrib::Enum attr = Attrib::Enum(m_unboundUsedAttrib[ii]);
				GLint loc = m_attributes[attr];
				lazyDisableVertexAttribArray(loc);
			}
		}

		applyLazyEnabledVertexAttributes();
	}

	void ProgramGL::unbindAttributes()
	{
		for(uint32_t ii = 0, iiEnd = m_usedCount; ii < iiEnd; ++ii)
		{
			if (Attrib::Count == m_unboundUsedAttrib[ii])
			{
				Attrib::Enum attr = Attrib::Enum(m_used[ii]);
				GLint loc = m_attributes[attr];
				lazyDisableVertexAttribArray(loc);
			}
		}
	}

	void ProgramGL::unbindInstanceData() const
	{
		for(uint32_t ii = 0; -1 != m_instanceData[ii]; ++ii)
		{
			GLint loc = m_instanceData[ii];
			lazyDisableVertexAttribArray(loc);
		}
	}

	void strins(char* _str, const char* _insert)
	{
		size_t len = bx::strLen(_insert);
		bx::memMove(&_str[len], _str, bx::strLen(_str) + 1);
		bx::memCopy(_str, _insert, len);
	}

	void string_replace(std::string& strBig, const std::string& strsrc, const std::string& strdst)
	{
		std::string::size_type pos = 0;
		std::string::size_type srclen = strsrc.size();
		std::string::size_type dstlen = strdst.size();

		if ((pos = strBig.find(strsrc, pos)) != std::string::npos)
		{
			strBig.replace(pos, srclen, strdst);
		}
	}

	void string_replace_all(std::string& strBig, const std::string& strsrc, const std::string& strdst)
	{
		std::string::size_type pos = 0;
		std::string::size_type srclen = strsrc.size();
		std::string::size_type dstlen = strdst.size();

		while ((pos = strBig.find(strsrc, pos)) != std::string::npos)
		{
			strBig.replace(pos, srclen, strdst);
			pos += dstlen;
		}
	}

	std::string getAndRemvoeExtensions(std::string& src)
	{
		size_t endExtenPos = src.find("// end extensions");
		int endExtenLen = strlen("// end extensions");
		std::string output;
		if (endExtenPos != std::string::npos)
		{
			output = src.substr(0, endExtenPos + endExtenLen + 1);
			src.erase(0, endExtenPos + endExtenLen);
		}

		return output;
	}

	void ShaderGL::create(const Memory* _mem)
	{
		bx::MemoryReader reader(_mem->data, _mem->size);
		m_hash = bx::hash<bx::HashMurmur2A>(_mem->data, _mem->size);

		bx::ErrorAssert err;

		uint32_t magic;
		bx::read(&reader, magic, &err);

		if (isShaderType(magic, 'C'))
		{
			m_type = GL_COMPUTE_SHADER;
		}
		else if (isShaderType(magic, 'F'))
		{
			m_type = GL_FRAGMENT_SHADER;
		}
		else if (isShaderType(magic, 'V'))
		{
			m_type = GL_VERTEX_SHADER;
		}

		// Test if it's self defined shader, which is optimized by ourselves.
		// Bgfx will ignore this case.
		bool isCusShader = isCustomShader(magic);

		uint32_t hashIn;
		bx::read(&reader, hashIn, &err);

		uint32_t hashOut;

		if (isShaderVerLess(magic, 6))
		{
			hashOut = hashIn;
		}
		else
		{
			bx::read(&reader, hashOut, &err);
		}

		uint16_t count;
		bx::read(&reader, count, &err);

		BX_TRACE("%s Shader consts %d"
			, getShaderTypeName(magic)
			, count
		);

		for (uint32_t ii = 0; ii < count; ++ii)
		{
			uint8_t nameSize = 0;
			bx::read(&reader, nameSize, &err);

			char name[256];
			bx::read(&reader, &name, nameSize, &err);
			name[nameSize] = '\0';

			uint8_t type;
			bx::read(&reader, type, &err);

			uint8_t num;
			bx::read(&reader, num, &err);

			uint16_t regIndex;
			bx::read(&reader, regIndex, &err);

			uint16_t regCount;
			bx::read(&reader, regCount, &err);

			// 记录绑定的索引，提前绑定，可以提升效率
			bgfx::UniformType::Enum uniformType = (bgfx::UniformType::Enum)(0x0F & type);
			if (type == bgfx::UniformType::Sampler)
			{
				std::string uniformName = name;
				m_TextureUniforms[uniformName] = regIndex;
			}
			else if (type == bgfx::UniformType::ConstBuffer)
			{
				std::string uniformName = name;
				m_UniformBlocks[uniformName] = regIndex;
			}
		}

		uint32_t shaderSize;
		bx::read(&reader, shaderSize, &err);

		m_id = glCreateShader(m_type);
		BX_WARN(0 != m_id, "Failed to create shader.");

		char* shaderCode = const_cast<char*>((const char*)reader.getDataPtr());

		std::string shaderCodeStr;
		char* pGapidShader = NULL;
		if (false && m_type == GL_FRAGMENT_SHADER)
		{
			char dst[] = "gl_FragColor  ";
			pGapidShader = (char*)malloc(shaderSize + 1);
			memcpy(pGapidShader, shaderCode, shaderSize);
			pGapidShader[shaderSize] = '\0';

			for (int i = 0; i < 8; i++)
			{
				char result[16];
				bx::snprintf(result, BX_COUNTOF(result), "gl_FragData[%d]", i);
				char* pos = const_cast<char*>(strstr(pGapidShader, result));
				if (pos)
				{
					int count = 0;
					while (count < 14)
					{
						*pos = dst[count];
						pos++;
						count++;
					}
				}
			}

			shaderCodeStr.assign(pGapidShader, shaderSize);
		}
		else
		{
			shaderCodeStr.assign(shaderCode, shaderSize);
		}

		if (m_id != 0)
		{
			bx::StringView code;
			char* temp = NULL;

			if (m_type != GL_COMPUTE_SHADER)
			{
				bool isAndroid = BX_ENABLED(BX_PLATFORM_ANDROID);
				bool isIOS = BX_ENABLED(BX_PLATFORM_IOS);
				bool isDesktop = (BX_ENABLED(BX_PLATFORM_WINDOWS) || BX_ENABLED(BX_PLATFORM_OSX));
				bool isCompleteShader = isCustomShader(magic);

				if ((!isCompleteShader) && (isAndroid || isIOS || isDesktop))
				{
					int32_t tempLen = shaderCodeStr.length() + (4 << 10);
					temp = (char*)malloc(tempLen);
					bx::StaticMemoryBlockWriter writer(temp, tempLen);

					bool bES31 = (shaderCodeStr.find("#version 310 es") != std::string::npos);
					bool bEmitExternal = (shaderCodeStr.find("samplerExternalOES") != std::string::npos);
					bool bEs30 = (OpenGL::GetCurrentFeatureLevel() == DeviceFeatureLevel::ES3_0);
					bool bEs20 = (OpenGL::GetCurrentFeatureLevel() == DeviceFeatureLevel::ES2);
					bool needExtInstanceDefine = false;

					if (isAndroid || isIOS)
					{
						if (bES31)
						{
							bx::write(&writer, "#version 310 es\n", &err);
							string_replace(shaderCodeStr, "#version 310 es", "");
						}
						else if (bEs30)
						{
							bx::write(&writer, "#version 300 es\n", &err);
							string_replace(shaderCodeStr, "#version 100", "");
						}
						else if (bEs20)
						{
							bx::write(&writer, "#version 100\n", &err);
							string_replace(shaderCodeStr, "#version 100", "");
							needExtInstanceDefine = true;
						}
					}

					if (needExtInstanceDefine)
					{
						bx::write(&writer, "#ifdef GL_EXT_draw_instanced\n", &err);
						bx::write(&writer, "#define VENUS_EXT_draw_instanced 1\n", &err);
						bx::write(&writer, "#endif\n", &err);
					}

					bool bFramebufferFetch = OpenGL::NeedFrameBufferFetchDef();
					if (bFramebufferFetch && m_type == GL_FRAGMENT_SHADER)
					{
						bx::write(&writer, "#define VENUS_EXT_shader_framebuffer_fetch 1\n", &err);
					}

					if (m_type == GL_FRAGMENT_SHADER)
					{
						bool bMRTFramebufferFetch = OpenGL::SupportsShaderMRTFramebufferFetch();
						bool bMRTFramebufferFetchDS = OpenGL::SupportFrameBufferFetchDepthStencil();
						if (bMRTFramebufferFetch)
						{
							bx::write(&writer, "#define VENUS_MRT_FRAMEBUFFER_FETCH 1\n", &err);
							if (bMRTFramebufferFetchDS)
							{
								bx::write(&writer, "#define USE_SCENE_DEPTH_ON 1\n", &err);
							}
							else
							{
								bx::write(&writer, "#define USE_SCENE_DEPTH_OFF 1\n", &err);
							}
						}
					}

					if (bEmitExternal)
					{
						size_t endPos = shaderCodeStr.find("// Uses samplerExternalOES");

						if (endPos != std::string::npos)
						{
							std::string extensions = getAndRemvoeExtensions(shaderCodeStr);
							bx::write(&writer, extensions.c_str(), &err);

							OpenGL::OESTextureType oesType = OpenGL::GetOESType();

							if (oesType != OpenGL::None)
							{
								switch (oesType)
								{
								case OpenGL::OES100:
									bx::write(&writer, "#extension GL_OES_EGL_image_external : require\n", &err);
									break;
								case OpenGL::OES300:
									bx::write(&writer, "#extension GL_OES_EGL_image_external : require\n", &err);
									break;
								case OpenGL::OESESSL300:
									bx::write(&writer, "#extension GL_OES_EGL_image_external_essl3 : require\n", &err);
									break;
								default:
									break;
								}
							}
							else
							{
								bx::write(&writer, "#define samplerExternalOES sampler2D\n", &err);
							}
						}
					}

					if (bES31 || isDesktop)
					{
						std::string extensions = getAndRemvoeExtensions(shaderCodeStr);
						bx::write(&writer, extensions.c_str(), &err);

						if (bES31)
						{
							bx::write(&writer, "#define INTERFACE_LOCATION(Pos) layout(location=Pos) \n", &err);
							bx::write(&writer, "#define INTERFACE_BLOCK(Pos, Interp, Modifiers, Semantic, PreType, PostType) layout(location=Pos) Modifiers Semantic { PreType PostType; }\n", &err);
						}
						else
						{
							bx::write(&writer, "#extension GL_ARB_separate_shader_objects : enable\n", &err);
							bx::write(&writer, "#define INTERFACE_LOCATION(Pos) layout(location=Pos) \n", &err);
							bx::write(&writer, "#define INTERFACE_BLOCK(Pos, Interp, Modifiers, Semantic, PreType, PostType) layout(location=Pos) Interp Modifiers struct { PreType PostType; }\n", &err);
						}
					}
					else if (bEs30)
					{
						bx::write(&writer, "#define INTERFACE_LOCATION(Pos) layout(location=Pos) \n", &err);
					}


					{
						if (bEs30)
						{
							if (m_type == GL_VERTEX_SHADER)
							{
								bx::write(&writer,
									"#define texture2D texture \n"
									"#define texture2DProj textureProj \n"
									"#define texture2DLod textureLod \n"
									"#define texture2DLodEXT textureLod \n"
									"#define texture2DProjLod textureProjLod \n"
									"#define textureCube texture \n"
									"#define textureCubeLod textureLod \n"
									"#define textureCubeLodEXT textureLod \n"
									"#define texture3D texture \n"
									"#define texture3DProj textureProj \n"
									"#define texture3DLod textureLod \n", &err);

								string_replace_all(shaderCodeStr, "attribute", "in");
								string_replace_all(shaderCodeStr, "varying", "out");
							}
							else if (m_type == GL_FRAGMENT_SHADER)
							{
								std::string extensions = getAndRemvoeExtensions(shaderCodeStr);
								bx::write(&writer, extensions.c_str(), &err);
								if (isAndroid)
								{
									bx::write(&writer, "#extension GL_EXT_shader_texture_lod : enable\n", &err);
								}

								bx::write(&writer,
									"#define texture2D texture \n"
									"#define texture2DProj textureProj \n"
									"#define texture2DLod textureLod \n"
									"#define texture2DLodEXT textureLod \n"
									"#define texture2DProjLod textureProjLod \n"
									"#define textureCube texture \n"
									"#define textureCubeLod textureLod \n"
									"#define textureCubeLodEXT textureLod \n"
									"#define texture3D texture \n"
									"#define texture3DProj textureProj \n"
									"#define texture3DLod textureLod \n"
									"#define texture3DProjLod textureProjLod \n"
									"#define shadow2DEXT(a, b) texture2D(a, b) \n"
									"\n"
									"#define gl_FragColor out_FragColor \n"
									"#ifdef EXT_shader_framebuffer_fetch_enabled \n"
									"INTERFACE_LOCATION(0) inout mediump vec4 out_FragColor; \n"
									"#else \n"
									"INTERFACE_LOCATION(0) out mediump vec4 out_FragColor; \n"
									"#endif \n", &err);

								string_replace_all(shaderCodeStr, "attribute", "in");
								string_replace_all(shaderCodeStr, "varying", "in");
							}
						}
						else if (isAndroid)
						{
							if (m_type == GL_FRAGMENT_SHADER)
							{
								bx::write(&writer, "#define DONTEMITSAMPLERDEFAULTPRECISION \n", &err);
							}

							// need gl externs
							bool supportTexLod = OpenGL::SupportsShaderTextureLod();
							bool requireCubeLodDefine = true;
							bool requireHack = false;

							if (!supportTexLod)
							{
								bx::write(&writer,
									"#define DONTEMITEXTENSIONSHADERTEXTURELODENABLE \n"
									"#define texture2DLodEXT(a, b, c) texture2D(a, b) \n"
									"#define textureCubeLodEXT(a, b, c) textureCube(a, b) \n", &err
								);
							}
							else if (requireCubeLodDefine)
							{
								bx::write(&writer, "#define textureCubeLodEXT textureCubeLod \n", &err);
							}

							if (requireHack)
							{
								bx::write(&writer, "#define TEXCOORDPRECISIONWORKAROUND \n", &err);
							}
						}
					}

					bx::write(&writer, "#define HLSLCC_DX11ClipSpace 0 \n", &err);
					bx::write(&writer, shaderCodeStr.c_str(), &err);
					bx::write(&writer, '\0', &err);

					code = temp;
				}
				else
				{
					code = shaderCodeStr.c_str();
				}
			}
			else
			{
				code = shaderCodeStr.c_str();
				int32_t codeLen = (int32_t)bx::strLen(code);
				int32_t tempLen = codeLen + (4 << 10);
				temp = (char*)malloc(tempLen);
				bx::StaticMemoryBlockWriter writer(temp, tempLen);

				bx::write(&writer, code.getPtr(), &err);
				bx::write(&writer, '\0', &err);
			}

			// compile shader
			GL_CHECK(glShaderSource(m_id, 1, (const GLchar**)&code, NULL));
			GL_CHECK(glCompileShader(m_id));

			GLint compiled = 0;
			GL_CHECK(glGetShaderiv(m_id, GL_COMPILE_STATUS, &compiled));

			if (0 == compiled)
			{
				/*	// shader编译失败输出shader文本
				LineReader lineReader(code);
				bx::Error err;
				for (int32_t line = 1; err.isOk(); ++line)
				{
					char str[4096];
					int32_t len = bx::read(&lineReader, str, BX_COUNTOF(str) - 1, &err);

					if (err.isOk())
					{
						str[len] = '\0';
						bx::StringView eol = bx::strFindEol(str);
						if (eol.getPtr() != str)
						{
							*const_cast<char*>(eol.getPtr()) = '\0';
						}
						BX_TRACE("%3d %s", line, str);
						LOGE("%3d %s", line, str);
					}
				}
				*/
				GLsizei len;
				char log[1024];
				GL_CHECK(glGetShaderInfoLog(m_id, sizeof(log), &len, log));
				BX_TRACE("Failed to compile shader. %d: %s", compiled, log);

				GL_CHECK(glDeleteShader(m_id));
				m_id = 0;
				BGFX_FATAL(false, bgfx::Fatal::InvalidShader, "Failed to compile shader.");
			}

			if (temp)
			{
				free(temp);
			}
		}

		if (pGapidShader != NULL)
		{
			free(pGapidShader);
		}
	}

	void ShaderGL::destroy()
	{
		if (0 != m_id)
		{
			GL_CHECK(glDeleteShader(m_id));
			m_id = 0;
		}
	}
    

} } // namespace bgfx

#else

namespace bgfx { namespace gl
{
	
} /* namespace gl */ } // namespace bgfx

#endif // (BGFX_CONFIG_RENDERER_OPENGLES || BGFX_CONFIG_RENDERER_OPENGL)
