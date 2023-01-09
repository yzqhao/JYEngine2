#pragma once

#include "RHI/RHI.h"
#include "Engine/private/Define.h"
#include "DecompressShaderAnalyzer.h"

NS_JYE_BEGIN

class ShaderCode
{
private:
	String compressCode;
public:
	String shaderCode;
	bool compress;
	handle compressFileHandle;
	int blockIndex;
	int blockOffset;
	int blockSize;

	ShaderCode()
		:compressFileHandle(0)
	{

	}

	const String& GetCode()
	{
		if (compress)
		{
			if (compressCode.empty())
			{
				DecompressShaderAnalyzer::Instance()->DecompressShaderFromFile(compressFileHandle, blockIndex, blockOffset, blockSize, compressCode);
			}
			return compressCode;
		}
		else
		{
			return shaderCode;
		}
	}

	void ClearCompressed()
	{
		String().swap(compressCode);
	}
};

struct ShaderStagesInfo
{
	String	shaderName;
	String	filePath;
	ShaderCode vscode;
	ShaderCode pscode;
	ShaderCode cscode;

	Vector<InputUniformInfo> vsUniforms;
	Vector<InputAttributes> vsAttributes;
	unsigned short vsBufferSize = 0;

	Vector<InputUniformInfo>  psUniforms;
	unsigned short psBufferSize = 0;

	Vector<InputUniformInfo>  csUniforms;
	unsigned short csBufferSize = 0;

	int shaderVersion = 0;

	const String& GetVsCode()
	{
		return vscode.GetCode();
	}

	const String& GetPsCode()
	{
		return pscode.GetCode();
	}

	const String& GetCsCode()
	{
		return cscode.GetCode();
	}

	void ClearCompressedResult()
	{
		vscode.ClearCompressed();
		pscode.ClearCompressed();
	}

	void Clear()
	{
		String().swap(shaderName);
		String().swap(filePath);
		String().swap(vscode.shaderCode);
		String().swap(pscode.shaderCode);
		String().swap(cscode.shaderCode);
		Vector<InputUniformInfo>().swap(vsUniforms);
		Vector<InputUniformInfo>().swap(psUniforms);
		Vector<InputUniformInfo>().swap(csUniforms);
		Vector<InputAttributes>().swap(vsAttributes);
	}
};

NS_JYE_END