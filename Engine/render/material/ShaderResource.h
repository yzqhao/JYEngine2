#pragma once

#include "IShaderResource.h"

NS_JYE_BEGIN

class ENGINE_API ShaderResource : public IShaderResource
{
public:
	ShaderResource(void);
	virtual ~ShaderResource(void);

	inline void SetVShaderCode(const ShaderCode& code)
	{
		m_ShaderStagesInfo.vscode = code;
	}
	inline const String& GetVShaderCode()
	{
		return m_ShaderStagesInfo.GetVsCode();
	}
	inline void SetPShaderCode(const ShaderCode& code)
	{
		m_ShaderStagesInfo.pscode = code;
	}
	inline const String& GetPShaderCode()
	{
		return m_ShaderStagesInfo.GetPsCode();
	}

	inline void SetVSUniforms(const Vector<InputUniformInfo>& vsUniforms)
	{
		m_ShaderStagesInfo.vsUniforms = vsUniforms;
	}
	inline void SetPSUniforms(const Vector<InputUniformInfo>& psUniforms)
	{
		m_ShaderStagesInfo.psUniforms = psUniforms;
	}
	inline void SetVSAttributes(const Vector<InputAttributes>& vsAtts)
	{
		m_ShaderStagesInfo.vsAttributes = vsAtts;
	}
	inline void SetVSUniforms(Vector<InputUniformInfo>&& vsUniforms)
	{
		m_ShaderStagesInfo.vsUniforms = std::move(vsUniforms);
	}
	inline void SetPSUniforms(Vector<InputUniformInfo>&& psUniforms)
	{
		m_ShaderStagesInfo.psUniforms = std::move(psUniforms);
	}
	inline void SetVSAttributes(Vector<InputAttributes>&& vsAtts)
	{
		m_ShaderStagesInfo.vsAttributes = std::move(vsAtts);
	}
	inline void SetVSBufferSize(unsigned short vsBufferSize)
	{
		m_ShaderStagesInfo.vsBufferSize = vsBufferSize;
	}
	inline void SetPSBufferSize(unsigned short psBufferSize)
	{
		m_ShaderStagesInfo.psBufferSize = psBufferSize;
	}
	inline void SetShaderVersion(uint shaderVersion)
	{
		m_ShaderStagesInfo.shaderVersion = shaderVersion;
	}

public:
	virtual bool CreateProgram(ShaderStagesInfo& shaderStages) final override;

	virtual void _ErrorReport() final override;
};

NS_JYE_END