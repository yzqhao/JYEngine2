#pragma once

#include "../Core.h"

NS_JYE_BEGIN

namespace ResourceProperty
{
	enum SourceLoaderType
	{
		SLT_NONE = -1,
		SLT_TEXTURE = 0,//加载一个纹理
		SLT_MESH,//网格
		SLT_MAT,
		SLT_METADATA,	// tex meta
		SLT_COUNT,
	};

	enum SourceType
	{
		CST_SHARED = 0,
		CST_MONOPOLY,
	};

	enum ResourceType//S-shared, M-mono
	{
		SRT_TEXTURE = 0,
		SRT_VERTEXBUFFER,
		SRT_INDICESBUFFER,
		SRT_RENDERTARGET,
		SRT_MATERIALENTITY,

		MRT_RENDEROBJECT,

		RT_COUNT,
	};
}

NS_JYE_END