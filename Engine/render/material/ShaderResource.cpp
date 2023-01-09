#include "ShaderResource.h"
#include "Core/Interface/ILogSystem.h"
#include "RHI/RHI.h"
#include "MaterialUtil.h"

NS_JYE_BEGIN

ShaderResource::ShaderResource()
{

}

ShaderResource::~ShaderResource()
{

}

bool ShaderResource::CreateProgram(ShaderStagesInfo& shaderStages)
{
	Clear();

	Vector<OutputUniformInfo> outVsUniform;
	Vector<OutputUniformInfo> outPsUniform;
	Vector<InputAttributes> inAttributes;

	const String& vscode = shaderStages.GetVsCode();
	const String& fscode = shaderStages.GetPsCode();
	uint shaderVersion = shaderStages.shaderVersion;

	handle vsHandle = RHICreateShader(vscode, RHIDefine::ShaderType::VS, shaderStages.vsUniforms,
		shaderStages.vsAttributes, shaderStages.vsBufferSize, outVsUniform);
	RHISetShaderName(vsHandle, shaderStages.filePath);

	handle psHandle = RHICreateShader(fscode, RHIDefine::ShaderType::PS, shaderStages.psUniforms,
		inAttributes, shaderStages.psBufferSize, outPsUniform);
	RHISetShaderName(psHandle, shaderStages.filePath);

	m_ProgramHandle = RHICreateProgram(vsHandle, psHandle);
	// handle vs uniforms
	unsigned char stage = 0;
	bool regHasValue = true;
	for (int i = 0; i < outVsUniform.size(); i++)
	{
		OutputUniformInfo& curInfo = outVsUniform.at(i);
		ShaderUniform* su = CreateShaderUniform(curInfo);

		if (MatUtil::IsNeedStage(curInfo.type))
		{
			unsigned char seq = stage++;

			if (!regHasValue)
			{
				su->regIndex = seq;
			}
			su->shaderType = RHIDefine::ShaderType::VS;
			su->m_stage = seq;
		}

		m_UnifromFunctionList.push_back(su);
	}

	// handle ps uniforms
	for (int i = 0; i < outPsUniform.size(); i++)
	{
		OutputUniformInfo& curInfo = outPsUniform.at(i);
		ShaderUniform* su = CreateShaderUniform(curInfo);

		if (MatUtil::IsNeedStage(curInfo.type))
		{
			unsigned char seq = stage++;

			if (!regHasValue)
			{
				su->regIndex = seq;
			}

			su->shaderType = RHIDefine::ShaderType::PS;
			su->m_stage = seq;
		}

		m_UnifromFunctionList.push_back(su);
	}

	Vector<uint16> builtinVarsNotBatch;

	for(auto& it : m_UnifromFunctionList)
	{
		RHIDefine::VariablesType varType = (it)->type;
		int bindIndex = (it)->bindIndex;

		if (varType == RHIDefine::VT_CONSTBUFFER)
		{
			m_ConstBufferList.push_back(it);
		}
		else
		{
			if (bindIndex >= 0)
			{
				builtinVarsNotBatch.push_back(bindIndex);
			}
		}
	}

	m_BuiltFlagNotBatch = BuiltinParams::QueryFlag(builtinVarsNotBatch);

	return true;
}

void ShaderResource::_ErrorReport()
{
	const String& vsCode = m_ShaderStagesInfo.GetVsCode();
	const String& psCode = m_ShaderStagesInfo.GetPsCode();

	if (!vsCode.empty() && !psCode.empty())
	{
		JYLOGTAG(LogEngineRHI, "ShaderSource complied failed vscode:%s, pscode:%s\n",
			vsCode.c_str(), psCode.c_str());
	}
}

NS_JYE_END
