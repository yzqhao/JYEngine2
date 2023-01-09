#pragma once

#include "GLBasicTypes.h"

#ifndef FORCEINLINE
#if BGFX_CONFIG_DEBUG
#define FORCEINLINE	inline									/* Easier to debug */
#elif BX_PLATFORM_WINDOWS
#define FORCEINLINE __forceinline
#else
#define FORCEINLINE inline __attribute__ ((always_inline))	/* Force code to be inline */
#endif
#endif

// Base static class
class OpenGLBase
{
public:
	enum OESTextureType
	{
		None,
		OES100,
		OES300,
		OESESSL300
	};

	static FORCEINLINE bool Support_GLES3_0()										{ return false; }
	static FORCEINLINE bool Support_GLES3_1()										{ return false; }
	static FORCEINLINE bool Support_GLES3_2()										{ return false; }
	static FORCEINLINE bool IsLowEndMobile()										{ return false; }
	static FORCEINLINE DeviceFeatureLevel::Enum GetCurrentFeatureLevel()			{ return DeviceFeatureLevel::NONE; }
	static FORCEINLINE void SetCurrentFeatureLevel(DeviceFeatureLevel::Enum level)	{	}


	static FORCEINLINE bool SupportFrameBufferFetch()								{ return OpenGLBase::s_FrameBufferFetch; };
	static FORCEINLINE bool SupportsPixelBuffer()									{ return false; }
	static FORCEINLINE bool NeedFrameBufferFetchDef()								{ return OpenGLBase::s_FrameBufferFetchNeedDef; }
	static FORCEINLINE bool SupportsShaderMRTFramebufferFetch()						{ return OpenGLBase::bSupportsShaderMRTFramebufferFetch; }
	static FORCEINLINE bool SupportFrameBufferFetchDepthStencil()					{ return OpenGLBase::s_FrameBufferFetchDepthStencil; };
	static FORCEINLINE bool SupportsShaderTextureLod()								{ return OpenGLBase::s_ShaderTextureLod; }
	static FORCEINLINE bool SupportsColorBufferHalfFloat()							{ return OpenGLBase::s_bSupportsColorBufferHalfFloat; }
	static FORCEINLINE bool SupportsColorBufferFloat()								{ return OpenGLBase::s_bSupportsColorBufferFloat; }
	static FORCEINLINE OESTextureType GetOESType()									{ return  s_OesType; }

	static void compressedTexSubImage(
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
	);

	static void texSubImage(
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
	);

	static void compressedTexImage(
		GLenum _target
		, GLint _level
		, GLenum _internalformat
		, GLsizei _width
		, GLsizei _height
		, GLsizei _depth
		, GLint _border
		, GLsizei _imageSize
		, const GLvoid* _data
	);

	static void texImage(
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
	);

	static void getFilters(uint32_t _flags, bool _hasMips, GLenum& _magFilter, GLenum& _minFilter);


	static void frameBufferValidate();
	static void VertexAttribDivisor(GLuint _index, GLuint _divisor);
	static void DrawArraysInstanced(GLenum _mode, GLint _first, GLsizei _count, GLsizei _primcount);
	static void DrawElementsInstanced(GLenum _mode, GLsizei _count, GLenum _type, const GLvoid* _indices, GLsizei _primcount);
	static void InsertEventMarker(GLsizei _length, const char* _marker);
	static void PushDebugGroup(GLenum _source, GLuint _id, GLsizei _length, const char* _message);
	static void PopDebugGroup();
	static void ObjectLabel(GLenum _identifier, GLuint _name, GLsizei _length, const char* _label);
	static void InvalidateFramebuffer(GLenum _target, GLsizei _numAttachments, const GLenum* _attachments);
	static void MultiDrawArraysIndirect(GLenum _mode, const void* _indirect, GLsizei _drawcount, GLsizei _stride);
	static void MultiDrawElementsIndirect(GLenum _mode, GLenum _type, const void* _indirect, GLsizei _drawcount, GLsizei _stride);
	static void PolygonMode(GLenum _face, GLenum _mode);
	
	static Extension s_extension[];
	static TextureFormatInfo s_textureFormat[];
	static GLenum s_rboFormat[];
	static const GLenum s_cmpFunc[];
	static bool s_textureFilter[TextureFormat::Count + 1];
	static GLenum s_textureAddress[];
	static const Blend s_blendFactor[];
	static const GLenum s_blendEquation[];
	static const GLenum s_stencilOp[];
	static const GLenum s_stencilFace[];
	static GLenum s_imageFormat[];
	static const GLenum s_textureFilterMag[];
	static const GLenum s_textureFilterMin[][3];

	static bool s_FrameBufferFetchNeedDef;
	static bool bSupportsShaderMRTFramebufferFetch;
	static bool s_FrameBufferFetchDepthStencil;
	static bool s_ShaderTextureLod;
	static bool s_FrameBufferFetch;
	static bool s_bSupportsColorBufferHalfFloat;
	static bool s_bSupportsColorBufferFloat;
	static OESTextureType s_OesType;
};

