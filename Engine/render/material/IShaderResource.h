#pragma once

#include "ShaderStructs.h"
#include "BuiltinParams.h"
#include "ShaderKeyWords.h"

#include <list>

NS_JYE_BEGIN

struct ShaderUniform
{
public:
	RHIDefine::ParameterSlot slot;
	int bindIndex;
	RHIDefine::ShaderType shaderType;
	unsigned char m_stage;
	unsigned char regIndex;
	unsigned short regCount;
	handle handle;
	RHIDefine::VariablesType type;
	unsigned short num;
	String name;
	Vector<ShaderUniform*> subUniforms;
	uint8_t srv_uav;
};

typedef List< ShaderUniform* >			UnifromFunctionList;

class ENGINE_API IShaderResource 
{
public:
	IShaderResource(void);
	virtual ~IShaderResource(void);

	virtual void ReleaseResource();
	virtual bool CreateProgram(ShaderStagesInfo& shaderStages) = 0;
	virtual void _ErrorReport() = 0;

	void GetUserUniformList(
		Vector<std::pair<RHIDefine::ParameterSlot, RHIDefine::VariablesType>>& uniformList);

	handle GetGpuResourceHandle() const;

	bool HandleIsValid(handle handle) const;

	bool HasSlotUsed(RHIDefine::ParameterSlot slot);

	inline UnifromFunctionList& GetUniformList();
	inline UnifromFunctionList& GetBatchUniformList();
	inline const BuiltinFlag& GetBuiltFlagNotBatch() const;

	ShaderUniform* CreateShaderUniform(OutputUniformInfo& uniformInfo);

public:

	FORCEINLINE void SetShaderMacros(ShaderKeyWords macros)
	{
		m_KeyWords = macros;
	}
	FORCEINLINE const ShaderKeyWords& GetShaderMacros()
	{
		return m_KeyWords;
	}

	FORCEINLINE void SetShaderName(const String& name)
	{
#ifdef _DEBUG
		m_ShaderStagesInfo.shaderName = name;
#endif
	}
	FORCEINLINE const String& GetShaderName()
	{
		return m_ShaderStagesInfo.shaderName;
	}
	FORCEINLINE void SetFilePath(const String& path)
	{
#ifdef _DEBUG
		m_ShaderStagesInfo.filePath = path;
#endif
	}

	FORCEINLINE bool IsSupportMacro(ShaderKey shaderKey)
	{
		return m_KeyWords.IsEnabled(shaderKey);
	}

	void MakeSureComplied();

protected:
	void Clear();

private:
	virtual bool _DeprecatedFilter(bool isd);
protected:
	UnifromFunctionList			m_UnifromFunctionList;
	UnifromFunctionList         m_ConstBufferList;
	handle					m_ProgramHandle;
	Vector<std::pair<RHIDefine::ParameterSlot, RHIDefine::VariablesType>>	m_UserUnifromList;
	BuiltinFlag					m_BuiltFlagNotBatch;

protected:
	ShaderKeyWords m_KeyWords;
	ShaderStagesInfo m_ShaderStagesInfo;
	bool m_bComplied;
};

inline handle IShaderResource::GetGpuResourceHandle() const
{
	return m_ProgramHandle;
}
	
inline UnifromFunctionList& IShaderResource::GetUniformList()
{
	return m_UnifromFunctionList;
}
	
inline UnifromFunctionList& IShaderResource::GetBatchUniformList()
{
	return m_ConstBufferList;
}
	
inline bool IShaderResource::HasSlotUsed(RHIDefine::ParameterSlot slot)
{
	bool find = false;
	for(auto& it : m_UnifromFunctionList)
	{
		if ((it)->slot == slot)
		{
			find = true;
			break;
		}
	}
	return find;
}
	
inline bool IShaderResource::HandleIsValid(handle handle) const
{
	return nullhandle != handle;
}
	
inline const BuiltinFlag& IShaderResource::GetBuiltFlagNotBatch() const
{
	return m_BuiltFlagNotBatch;
}

NS_JYE_END