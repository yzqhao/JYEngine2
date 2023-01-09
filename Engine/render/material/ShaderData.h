
#pragma once

#include "JsonMaterialParse.h"
#include "ShaderPass.h"
#include "DefProperty.h"
#include "DecompressShaderAnalyzer.h"
#include "Core/Reference.h"

NS_JYE_BEGIN

class IShaderData : public Reference
{
public:
	unsigned int shaderHash;
	String shaderPath;
	String oriShaderpPath;
	String shaderName;
	uint version = 0;
	handle compressFileHandle = nullhandle;

	handle LoadFile(const String& filename)
	{
		compressFileHandle = DecompressShaderAnalyzer::Instance()->LoadFile(filename);
		return compressFileHandle;
	}

	~IShaderData()
	{
		if (compressFileHandle != nullhandle)
		{
			DecompressShaderAnalyzer::Instance()->UnloadFile(compressFileHandle);
		}
	}
};

class ShaderData : public IShaderData
{
public:
	Vector<ShaderPass> shaderPassData;
	Vector<DefProperty> defPropertys;
	int renderQueue;	// RHIDefine::RenderQueue
};

NS_JYE_END
