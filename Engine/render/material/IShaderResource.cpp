#include "IShaderResource.h"
#include "RHI/RHI.h"

NS_JYE_BEGIN

IShaderResource::IShaderResource()
	: m_ProgramHandle(-1)
	, m_bComplied(false)
{

}
	
IShaderResource::~IShaderResource()
{
	Clear();
}
	
void IShaderResource::Clear()
{
	for(auto& it : m_UnifromFunctionList)
	{
		int subUniformSize = (it)->subUniforms.size();
		for (int i = 0; i < subUniformSize; i++)
		{
			SAFE_DELETE((it)->subUniforms[i]);
		}

		SAFE_DELETE(it);
	}

	m_UnifromFunctionList.clear();
	m_UserUnifromList.clear();
	m_ConstBufferList.clear();
}
	
bool IShaderResource::_DeprecatedFilter(bool isd)
{
	return false;
}
	
void IShaderResource::ReleaseResource()
{
	Clear();
	if (m_ProgramHandle != -1)
	{
		RHIDestoryProgram(m_ProgramHandle);
		m_ProgramHandle = -1;
	}
}
	
void IShaderResource::GetUserUniformList(
	Vector<std::pair<RHIDefine::ParameterSlot, RHIDefine::VariablesType>>& uniformList)
{
	uniformList = m_UserUnifromList;
}
	
ShaderUniform* CreateSingleShaderUniform(OutputUniformInfo& uniformInfo)
{
	ShaderUniform* su = new ShaderUniform();
	su->m_stage = 0;
	su->slot = uniformInfo.slot;
	su->handle = uniformInfo.uniHandle;
	su->num = uniformInfo.num;
	su->type = uniformInfo.type;
	su->bindIndex = BuiltinParams::GetBindIndex(su->slot);
	su->regIndex = uniformInfo.regIndex;
	su->regCount = uniformInfo.regCount;
	su->srv_uav = uniformInfo.srv_uav;
	su->name = uniformInfo.name;
	return su;
}
	
ShaderUniform* IShaderResource::CreateShaderUniform(OutputUniformInfo& uniformInfo)
{
	ShaderUniform* su = CreateSingleShaderUniform(uniformInfo);

	Vector<OutputUniformInfo>& subInfos = uniformInfo.subInfos;
	int size = subInfos.size();

	for (int i = 0; i < size; i++)
	{
		ShaderUniform* curSu = CreateSingleShaderUniform(subInfos[i]);
		su->subUniforms.push_back(curSu);
	}

	return su;
}

void IShaderResource::MakeSureComplied()
{
	if (!m_bComplied)
	{
		bool result = CreateProgram(m_ShaderStagesInfo);
		if (!result)
		{
			_ErrorReport();
		}
		m_ShaderStagesInfo.ClearCompressedResult();

		m_bComplied = true;
	}
}

NS_JYE_END
