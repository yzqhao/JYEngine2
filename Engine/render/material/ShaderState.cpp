#include "ShaderState.h"
#include "MaterialUtil.h"
#include "ShaderData.h"
#include "System/Utility.hpp"
#include "Core/Interface/IFileSystem.h"
#include "Core/Configure.h"
#include "Engine/GraphicDefine.h"

NS_JYE_BEGIN

ShaderState::ShaderState()
	: m_passMask(0)
	, m_baseLightMask(0)
	, m_deferredMaterial(false)
	, m_gpuInstancing(false)
{

}

ShaderState::~ShaderState()
{
}

ShaderState::ShaderState(const ShaderState& rhs)
{
	//m_pShaderData = new ShaderData(*rhs.m_pShaderData);
	m_pShaderData = rhs.m_pShaderData;
	m_propertymap = rhs.m_propertymap;
	m_passMask = rhs.m_passMask;
	m_baseLightMask = rhs.m_baseLightMask;
	m_deferredMaterial = rhs.m_deferredMaterial;
}

void ShaderState::CreateResource(const String& shaderfile)
{
	String shaderPath;
	String api;
	MatUtil::GetShaderPathAndAPI(shaderfile, shaderPath, api);

	ParserMaterial parseMat;
	JsonMaterialParse jp;
	jp.ParseMaterial(shaderPath, parseMat);

	int size = parseMat.PassDatas.size();
	m_pShaderData = _NEW ShaderData();
	m_pShaderData->oriShaderpPath = shaderfile;
	m_pShaderData->shaderPath = shaderPath;
	m_pShaderData->shaderHash = Utility::HashCode(shaderPath.data());;
	m_pShaderData->shaderName = parseMat.ShaderName;
	m_pShaderData->renderQueue = parseMat.RenderQueue;
	m_pShaderData->version = parseMat.Version;
	m_pShaderData->shaderPassData.reserve(size);

	handle compressFileHandle = nullhandle;
	if (size > 0)
	{
		ParserMaterial::PassData& passData = parseMat.PassDatas[0];
		std::vector<ParserMaterial::ProgramData>& progData = passData.programDatas;
		if (progData.size() > 0 && progData[0].vshaderCode.compress)
		{
			auto dotPos = shaderPath.rfind('.');
			std::string codeFilePath = shaderPath.substr(0, dotPos) + Configure::shaderCodeSuffix + Configure::archiveSuffix;
			std::string oldCodeFilePath = shaderPath + ".code" + Configure::archiveSuffix;
			if (IFileSystem::Instance()->isFileExist(codeFilePath))
			{
				compressFileHandle = m_pShaderData->LoadFile(codeFilePath);
			}
			else if (IFileSystem::Instance()->isFileExist(oldCodeFilePath))
			{
				compressFileHandle = m_pShaderData->LoadFile(oldCodeFilePath);
			}
		}
	}

	auto& shaderPassData = m_pShaderData->shaderPassData;
	shaderPassData.resize(size);
	for (int i = 0; i < size; i++)
	{
		auto& shaderPass = shaderPassData[i];
		auto& passData = parseMat.PassDatas[i];
		{
			MatUtil::ConvertParserData(shaderPass.state, passData.renderState);
		}
		shaderPass.passType = passData.passType;
		shaderPass.passName = passData.passName;

		Vector<ParserMaterial::ProgramData>& programData = passData.programDatas;
		int programCount = programData.size();

		Vector<InputUniformInfo> passUniforms;
		Vector<InputAttributes> passAttributes;
		if (passData.indexingUniforms)
		{
			passUniforms = std::move(MatUtil::UniformsFromParsed(passData.passUniforms));
		}
		if (passData.indexingAttributes)
		{
			passAttributes = std::move(MatUtil::AttributesFromParsed(passData.passAttributes));
		}

		String shaderUrlPrefix = std::move(Utility::StringFormat(
			"shader://%s/%s/%08x/",
			m_pShaderData->shaderPath.c_str(),
			shaderPass.passName.c_str(),
			m_pShaderData->shaderHash
		));

		auto& shaderEnties = shaderPass.entitys;
		{
			shaderEnties.resize(programCount);
		}
		{
			for (int j = 0; j < programCount; j++)
			{
				auto& curProgram = programData[j];
				auto& curEntity = shaderEnties[j];

				{
					ShaderKeyWords keyWords;
					keyWords.EnableStrArray(curProgram.keyWords);
					curEntity.SetShaderMacros(keyWords);
				}
				{
					Vector<InputUniformInfo> vsUniforms(curProgram.vsUniformIndices.size());
					Vector<InputUniformInfo> psUniforms(curProgram.psUniformIndices.size());
					if (passData.indexingUniforms)
					{
						int p = 0;
						for (int index : curProgram.vsUniformIndices)
						{
							vsUniforms[p++] = passUniforms[index];
						}
						p = 0;
						for (int index : curProgram.psUniformIndices)
						{
							psUniforms[p++] = passUniforms[index];
						}
					}
					else
					{
						vsUniforms = std::move(MatUtil::UniformsFromParsed(curProgram.vsUniforms));
						psUniforms = std::move(MatUtil::UniformsFromParsed(curProgram.psUniforms));
					}
					curEntity.SetVSUniforms(std::move(vsUniforms));
					curEntity.SetPSUniforms(std::move(psUniforms));
				}
				{
					Vector<InputAttributes> vsAttributes(curProgram.vsAttributeIndices.size());
					if (passData.indexingAttributes)
					{
						int p = 0;
						for (int index : curProgram.vsAttributeIndices)
						{
							vsAttributes[p++] = passAttributes[index];
						}
					}
					else
					{
						vsAttributes = std::move(MatUtil::AttributesFromParsed(curProgram.vsAttributes));
					}
					curEntity.SetVSAttributes(std::move(vsAttributes));
					curEntity.SetVSBufferSize(curProgram.vsBufferSize);
					curEntity.SetPSBufferSize(curProgram.psBufferSize);
				}
				{
					ShaderCode vsCode;
					ShaderCode psCode;
					MatUtil::ConvertShaderCode(vsCode, curProgram.vshaderCode);
					MatUtil::ConvertShaderCode(psCode, curProgram.pshaderCode);
					vsCode.compressFileHandle = compressFileHandle;
					psCode.compressFileHandle = compressFileHandle;
					curEntity.SetVShaderCode(vsCode);
					curEntity.SetPShaderCode(psCode);
				}
				{
					curEntity.SetShaderVersion(m_pShaderData->version);

					curEntity.SetShaderName(shaderPass.passName);
					curEntity.SetFilePath(shaderUrlPrefix);
				}
			}
		}
	}
		
	Vector<DefProperty>& defValue = m_pShaderData->defPropertys;
	Vector<ParserMaterial::ParamInfo>& infos = parseMat.Params;
	defValue.reserve(infos.size());

	for (int i = 0; i < infos.size(); i++)
	{
		defValue.emplace_back();
		DefProperty& curProperty = defValue.back();
		ParserMaterial::ParamInfo& curInfo = infos[i];

		MatUtil::SetDefPropertyValue(curProperty, curInfo);
	}

	SetContent();
}

void ShaderState::SetContent()
{
	m_propertymap.clear();
	m_passMask = 0;

	Vector<DefProperty>& defPropertys = m_pShaderData->defPropertys;
	for (int i = 0; i < defPropertys.size(); i++)
	{
		RHIDefine::ParameterSlot slot = defPropertys[i].GetSlot();
		m_propertymap.insert(std::pair<RHIDefine::ParameterSlot, size_t>(slot, i));
	}

	Vector<ShaderPass>& pass = m_pShaderData->shaderPassData;
	size_t count = pass.size();
	for (size_t index = 0; index < count; index++)
	{
		RHIDefine::PassType type = pass[index].passType;
		m_passMask |= (1ULL << type);
	}

	m_deferredMaterial = IsSupportPass(RHIDefine::PT_GBUFFER);

	ShaderPass* basePass = GetShaderPass(RHIDefine::PassType::PT_FORWARDBASE);
	if (basePass != nullptr)
	{
		auto& entitys = basePass->entitys;
		for (int i = 0; i < entitys.size(); i++)
		{
			bool supportDir = entitys[i].IsSupportMacro(KeyWordUtil::DirLight);
			bool supportPoint = entitys[i].IsSupportMacro(KeyWordUtil::PointLight);
			bool supportSpot = entitys[i].IsSupportMacro(KeyWordUtil::SpotLight);
			bool supportGpuInstancing = entitys[i].IsSupportMacro(KeyWordUtil::GPUINSTANCING);

			if (supportDir)
			{
				m_baseLightMask = (m_baseLightMask | (1u << GraphicDefine::LT_DIRECTIONAL));
			}

			if (supportPoint)
			{
				m_baseLightMask = (m_baseLightMask | (1u << GraphicDefine::LT_POINT));
			}

			if (supportSpot)
			{
				m_baseLightMask = (m_baseLightMask | (1u << GraphicDefine::LT_SPOT));
			}

			if (supportGpuInstancing)
			{
				m_gpuInstancing = true;
			}
		}
	}
}

ShaderPass* ShaderState::GetShaderPassByIndex(int passIndex) const
{
	ShaderPass* pShaderPass = nullptr;
	Vector<ShaderPass>& pass = m_pShaderData->shaderPassData;

	if (passIndex < pass.size())
	{
		pShaderPass = &pass[passIndex];
	}
	return pShaderPass;
}

bool ShaderState::GetRenderPass(int passIndex, ShaderKeyWords& keyWords, PassInfo& passInfo)
{
	ShaderResource* pShader = nullptr;
	ShaderPass* pShaderPass = GetShaderPassByIndex(passIndex);
	assert(pShaderPass != nullptr);

	Vector<ShaderResource>& shaders = pShaderPass->entitys;

	int score = std::numeric_limits<int>::min();
	for (int i = 0; i < shaders.size(); i++)
	{
		const ShaderKeyWords& curKeyWords = shaders[i].GetShaderMacros();
		int coreScore = ShaderKeyWords::ComputeMatchScore(curKeyWords, keyWords);

		if (coreScore > score)
		{
			score = coreScore;
			pShader = &shaders[i];
		}
	}

	if (pShader != nullptr)
	{
		pShader->MakeSureComplied();
	}

	passInfo.pRenderState = &pShaderPass->state;
	passInfo.pShaderEntity = pShader;

	assert(pShader != nullptr);
	return true;
}

const DefProperty& ShaderState::GetProperty(RHIDefine::ParameterSlot slot)
{
	std::map<RHIDefine::ParameterSlot, size_t>::iterator iter = m_propertymap.find(slot);
	const Vector<DefProperty>& defPropertys = GetDefPropertys();

	if (iter != m_propertymap.end())
	{
		int index = iter->second;

		if (index >= 0 && index < defPropertys.size())
		{
			return defPropertys[index];
		}
	}

	static DefProperty defv;
	return defv;
}

bool ShaderState::HasDefProperty(RHIDefine::ParameterSlot slot) const
{
	std::map<RHIDefine::ParameterSlot, size_t>::const_iterator iter = m_propertymap.find(slot);
	const Vector<DefProperty>& defPropertys = GetDefPropertys();

	if (iter != m_propertymap.end())
	{
		int index = iter->second;

		if (index >= 0 && index < defPropertys.size())
		{
			return true;
		}
	}

	return false;
}

ShaderPass* ShaderState::GetShaderPass(int passType) const
{
	std::vector<ShaderPass>& pass = m_pShaderData->shaderPassData;
	size_t count = pass.size();
	ShaderPass* res = nullptr;
	for (size_t index = 0; index < count; index++)
	{
		if (pass[index].passType == passType)
		{
			res = &(pass[index]);
			break;
		}
	}

	return res;
}

NS_JYE_END
