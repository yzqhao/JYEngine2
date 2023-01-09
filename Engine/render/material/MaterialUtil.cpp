#include "MaterialUtil.h"
#include "RHI/RHI.h"
#include "RHI/RHIDefine.h"
#include "ShaderStructs.h"
#include "IMaterialSystem.h"
#include "RenderState.h"
#include "Core/Interface/IFileSystem.h"
#include "ParameterArray.h"
#include "Engine/resource/TextureMetadata.h"

NS_JYE_BEGIN

using namespace RHIDefine;

const RHIDefine::VariablesType g_texType[] =
{
	RHIDefine::VariablesType::VT_TEXTURE1D,
	RHIDefine::VariablesType::VT_TEXTURE2D,
	RHIDefine::VariablesType::VT_TEXTURE3D,
	RHIDefine::VariablesType::VT_TEXTURECUBE,
};

const RHIDefine::VariablesType g_arrayType[] =
{
	RHIDefine::VariablesType::VT_FLOAT_ARRAY,
	RHIDefine::VariablesType::VT_VEC2_ARRAY,
	RHIDefine::VariablesType::VT_VEC3_ARRAY,
	RHIDefine::VariablesType::VT_VEC4_ARRAY,
};

struct MatchItem
{
	String fileName;
	String shaderApi;
	bool checkDir;
};

static void GetMatchList(Vector<MatchItem>& matchList)
{
	RHIDefine::GraphicRenderer renderer = RHIGetRenderer();
	RHIDefine::GraphicFeatureLevel featureLevel = RHIGetCurrentFeatureLevel();
	matchList.clear();

	MatchItem curMatch;

#define ADDMatchItem(name, api, checkdir) \
    curMatch.fileName = name; \
	curMatch.shaderApi = api;\
	curMatch.checkDir = checkdir; \
	matchList.push_back(curMatch)

	if (renderer == RHIDefine::RendererOpenGL)
	{
		ADDMatchItem("", "opengl", true);
		ADDMatchItem("/opengl.json", "opengl", false);
		ADDMatchItem("/opengl.lua", "opengl", false);
	}
	else if (renderer == RHIDefine::RendererDirect3D11)
	{
		ADDMatchItem("", "d3d11", true);
		ADDMatchItem("/d3d11.json", "d3d11", false);
		ADDMatchItem("/d3d11.lua", "d3d11", false);
	}
	else if (renderer == RHIDefine::RendererMetal)
	{
		ADDMatchItem("", "metal", true);
		ADDMatchItem("/metal.json", "metal", false);
		ADDMatchItem("/metal.lua", "metal", false);
	}
	else if (renderer == RHIDefine::RendererVulkan)
	{
		ADDMatchItem("", "vulkan", true);
		ADDMatchItem("vulkan.json", "vulkan", false);
		ADDMatchItem("vulkan.lua", "vulkan", false);
	}
	else if (renderer == RHIDefine::RendererGles)
	{
		ADDMatchItem("", "gles3", true);

		if (featureLevel == RHIDefine::ES2)
		{
			ADDMatchItem("/gles2.json", "gles2", false);
			ADDMatchItem("/gles2.lua", "gles2", false);
		}
		else if (featureLevel == RHIDefine::ES3_0)
		{
			ADDMatchItem("/gles3.json", "gles3", false);
			ADDMatchItem("/gles2.json", "gles2", false);
			ADDMatchItem("/gles3.lua", "gles3", false);
			ADDMatchItem("/gles2.lua", "gles2", false);
		}
		else if (featureLevel >= RHIDefine::ES3_1)
		{
			ADDMatchItem("/gles31.json", "gles31", false);
			ADDMatchItem("/gles3.json", "gles3", false);
			ADDMatchItem("/gles2.json", "gles2", false);
			ADDMatchItem("/gles31.lua", "gles31", false);
			ADDMatchItem("/gles3.lua", "gles3", false);
			ADDMatchItem("/gles2.lua", "gles2", false);
		}
	}
}

static TextureEntity* GetSysTexture(const String& texName, RHIDefine::VariablesType varType)
{
	bool bNormalTex = false;

	byte* colorData = NULL;
	static byte white[4] = { 255, 255, 255, 255 };
	static byte black[4] = { 0, 0, 0, 255 };
	static byte blackgrey[4] = { 127, 127, 127, 255 };
	static byte bump[4] = { 127, 127, 255, 255 };

	String texPath;
	if (texName == "white")
	{
		colorData = white;
		texPath = "comm:/texture/white.jpg";
	}
	else if (texName == "black")
	{
		colorData = black;
		texPath = "comm:/texture/black.png";
	}
	else if (texName == "blackgrey")
	{
		colorData = blackgrey;
		texPath = "comm:/texture/blackgrey.png";
	}
	else if (texName == "bump")
	{
		colorData = bump;
		texPath = "comm:/texture/defnormal.png";
		bNormalTex = true;
	}

	TextureEntity* pTexEntity = nullptr;

	if (varType == RHIDefine::VT_TEXTURE2D)
	{
		pTexEntity = _NEW TextureEntity();
		pTexEntity->PushMetadata(
			TextureFileMetadata(
				RHIDefine::TEXTURE_2D,
				RHIDefine::TU_STATIC,
				RHIDefine::PixelFormat::PF_AUTO,
				1,
				true,
				0,
				RHIDefine::TextureWarp::TW_REPEAT,
				RHIDefine::TextureWarp::TW_REPEAT,
				RHIDefine::TextureFilter::TF_LINEAR,
				RHIDefine::TextureFilter::TF_LINEAR,
				texPath, true, false));
		pTexEntity->SetKeepSource(true);
		pTexEntity->CreateResource();
	}
	else if (varType == RHIDefine::VT_TEXTURECUBE)
	{
	}
	else
	{
		assert(false && "not support material texture type");
	}

	return pTexEntity;
}

void MatUtil::SetDefPropertyValue(DefProperty& defProperty, ParserMaterial::ParamInfo& propertyInfo)
{
	ParserMaterial::ParamsType varType = propertyInfo.type;

	defProperty.SetName(propertyInfo.name);
	defProperty.SetDesc(propertyInfo.desc);
	defProperty.SetAttribute(propertyInfo.attribute);

	if (varType == ParserMaterial::VT_FLOAT)
	{
		defProperty.SetFloat(propertyInfo.defValue[0]);
	}
	else if (varType == ParserMaterial::VT_FLOATRANGE)
	{
		defProperty.SetFloatRange(propertyInfo.defValue[0],
			propertyInfo.defValue[1], propertyInfo.defValue[2]);
	}
	else if (varType == ParserMaterial::VT_VEC2)
	{
		defProperty.SetFloat2(propertyInfo.defValue[0], propertyInfo.defValue[1]);
	}
	else if (varType == ParserMaterial::VT_VEC3)
	{
		defProperty.SetFloat3(propertyInfo.defValue[0], propertyInfo.defValue[1],
			propertyInfo.defValue[2]);
	}
	else if (varType == ParserMaterial::VT_VEC4)
	{
		defProperty.SetFloat4(propertyInfo.defValue[0], propertyInfo.defValue[1],
			propertyInfo.defValue[2], propertyInfo.defValue[3]);
	}
	else if (varType == ParserMaterial::VT_COLOR)
	{
		defProperty.SetColor(propertyInfo.defValue[0], propertyInfo.defValue[1],
			propertyInfo.defValue[2], propertyInfo.defValue[3]);
	}
	else if (varType == ParserMaterial::VT_TEXTURE1D)
	{
		TextureEntity* pTexEntity = GetSysTexture(propertyInfo.defTexPath, RHIDefine::VT_TEXTURE1D);
		defProperty.SetTexture1D(pTexEntity);
	}
	else if (varType == ParserMaterial::VT_TEXTURE2D)
	{
		TextureEntity* pTexEntity = GetSysTexture(propertyInfo.defTexPath, RHIDefine::VT_TEXTURE2D);
		defProperty.SetTexture2D(pTexEntity);
	}
	else if (varType == ParserMaterial::VT_TEXTURE3D)
	{
		TextureEntity* pTexEntity = GetSysTexture(propertyInfo.defTexPath, RHIDefine::VT_TEXTURE3D);
		defProperty.SetTexture3D(pTexEntity);
	}
	else if (varType == ParserMaterial::VT_TEXTURECUBE)
	{
		TextureEntity* pTexEntity = GetSysTexture(propertyInfo.defTexPath, RHIDefine::VT_TEXTURECUBE);
		defProperty.SetTextureCube(pTexEntity);
	}
}

bool MatUtil::GetShaderPathAndAPI(const String& metaPath, String& shaderPath, String shaderAPI)
{
	bool findResult = false;

	Vector<MatchItem> matchItems;
	GetMatchList(matchItems);

	const String& curName = metaPath;
	int count = matchItems.size();
	for (int i = 0; i < count; i++)
	{
		MatchItem& curItem = matchItems[i];
		String& fileName = curItem.fileName;
		String shaderComPath = curName + fileName;

		bool isFile = false;

		if (!curItem.checkDir)
		{
			isFile = IFileSystem::Instance()->isFileExist(shaderComPath.c_str());
		}

		if (isFile)
		{
			shaderPath = shaderComPath;
			shaderAPI = curItem.shaderApi;

			findResult = true;
			break;
		}
	}

	return findResult;
}
	
bool MatUtil::IsTextureType(RHIDefine::VariablesType varType)
{
	bool isTexType = false;

	for (int i = 0; i < sizeof(g_texType) / sizeof(RHIDefine::VariablesType); i++)
	{
		if (varType == g_texType[i])
		{
			isTexType = true;
			break;
		}
	}

	return isTexType;
}

bool MatUtil::IsArrayType(RHIDefine::VariablesType varType)
{
	bool isArrayType = false;

	for (int i = 0; i < sizeof(g_arrayType) / sizeof(RHIDefine::VariablesType); i++)
	{
		if (varType == g_arrayType[i])
		{
			isArrayType = true;
			break;
		}
	}

	return isArrayType;
}

void MatUtil::ConvertShaderCode(ShaderCode& outCode, const ParserMaterial::ShaderCode& shaderCode)
{
	outCode.blockIndex = shaderCode.blockIndex;
	outCode.blockOffset = shaderCode.blockOffset;
	outCode.blockSize = shaderCode.blockSize;
	outCode.compress = shaderCode.compress;
	outCode.shaderCode = shaderCode.shaderCode;
}

bool MatUtil::IsNeedStage(RHIDefine::VariablesType varType)
{
	bool isTexType = MatUtil::IsTextureType(varType);
	bool isBufferType = (varType == RHIDefine::VT_BUFFER || varType == RHIDefine::VT_CONSTBUFFER);
	return isTexType || isBufferType;
}

static RHIDefine::VariablesType UniformTypeFromParsed(const String& type, unsigned short num)
{
	if (strcmp(type.c_str(), "float") == 0)
	{
		return num > 1 ? RHIDefine::VT_FLOAT_ARRAY : RHIDefine::VT_FLOAT;
	}
	else if (strcmp(type.c_str(), "float2") == 0)
	{
		return num > 1 ? RHIDefine::VT_VEC2_ARRAY : RHIDefine::VT_VEC2;
	}
	else if (strcmp(type.c_str(), "float3") == 0)
	{
		return num > 1 ? RHIDefine::VT_VEC3_ARRAY : RHIDefine::VT_VEC3;
	}
	else if (strcmp(type.c_str(), "float4") == 0)
	{
		return num > 1 ? RHIDefine::VT_VEC4_ARRAY : RHIDefine::VT_VEC4;
	}
	else if (strcmp(type.c_str(), "float3x3") == 0)
	{
		return num > 1 ? RHIDefine::VT_MAT3_ARRAY : RHIDefine::VT_MAT3;
	}
	else if (strcmp(type.c_str(), "float4x4") == 0)
	{
		return num > 1 ? RHIDefine::VT_MAT4_ARRAY : RHIDefine::VT_MAT4;
	}
	else if (strcmp(type.c_str(), "sampler2D") == 0)
	{
		return RHIDefine::VT_TEXTURE2D;
	}
	else if (strcmp(type.c_str(), "samplerCube") == 0)
	{
		return RHIDefine::VT_TEXTURECUBE;
	}
	else if (strcmp(type.c_str(), "buffer") == 0)
	{
		return RHIDefine::VT_BUFFER;
	}
	else if (strcmp(type.c_str(), "samplerBuffer") == 0)
	{
		return RHIDefine::VT_SAMPLERBUFFER;
	}
	else if (strcmp(type.c_str(), "constBuffer") == 0)
	{
		return RHIDefine::VT_CONSTBUFFER;
	}
	return RHIDefine::VT_ERRORCODE;
}

Vector<InputUniformInfo> MatUtil::UniformsFromParsed(const Vector<ParserMaterial::UniformData>& uniformData)
{
	Vector<InputUniformInfo> uniforms(uniformData.size());
	for (int i = 0; i < uniformData.size(); i++)
	{
		auto& curUniform = uniforms[i];
		auto& curParseData = uniformData.at(i);
		curUniform.name = curParseData.name;
		curUniform.type = UniformTypeFromParsed(curParseData.type, curParseData.num);
		curUniform.srv_uav = static_cast<uint8_t>(curParseData.sit);
		curUniform.slot = IMaterialSystem::Instance()->NewParameterSlot(ParameterUsage::SU_UNIFORM, curUniform.name);
		curUniform.regCount = curParseData.regCount;
		curUniform.regIndex = curParseData.regIndex;
		curUniform.num = curParseData.num;
	}

	for (int i = 0; i < uniformData.size(); i++)
	{
		auto& curUniform = uniforms[i];
		auto& curParseData = uniformData.at(i);
		int subIndexSize = curParseData.subIndexs.size();
		if (subIndexSize)
		{
			curUniform.subUniform.reserve(subIndexSize);
			for (int j = 0; j < subIndexSize; j++)
			{
				unsigned short curSubIndex = curParseData.subIndexs[j];
				curUniform.subUniform.push_back(uniforms[curSubIndex]);
			}
		}
	}

	return uniforms;
}

Vector<InputAttributes> MatUtil::AttributesFromParsed(const Vector<ParserMaterial::AttributeData>& attributesData)
{
	Vector<InputAttributes> attributes(attributesData.size());
	for (int i = 0; i < attributesData.size(); i++)
	{
		auto& curAttributes = attributes.at(i);
		auto& curParseData = attributesData.at(i);
		curAttributes.name = curParseData.attName;
		curAttributes.id = curParseData.attId;
	}

	return attributes;
}

static void PushCommonEffectList(RenderState& rs, BaseStateType* pType)
{
	const std::string& name = pType->GetName();
	if (!name.empty())
	{
		IMaterialSystem* pInstance = IMaterialSystem::Instance();
		bool hasSlot = pInstance->ParameterSlotExist(name);
		RHIDefine::ParameterSlot slot;
		if (!hasSlot)
		{
			slot = pInstance->NewParameterSlot(RHIDefine::ParameterUsage::SU_UNIFORM, name);
		}
		else
		{
			slot = pInstance->GetParameterSlot(name);
		}

		rs.m_comEffectByVar.push_back(std::make_pair(pType, slot));
	}
}

void MatUtil::ConvertParserData(RenderState& rs, const ParserMaterial::ParserRenderState& renderState)
{
	rs.m_isDepthTest = renderState.m_isDepthTest;
	PushCommonEffectList(rs, &rs.m_isDepthTest);
	rs.m_isDepthMask = renderState.m_isDepthMask;
	PushCommonEffectList(rs, &rs.m_isDepthMask);

	rs.m_DepthFunction = renderState.m_DepthFunction;
	PushCommonEffectList(rs, &rs.m_DepthFunction);
	rs.m_isCullFace = renderState.m_isCullFace;
	PushCommonEffectList(rs, &rs.m_isCullFace);
	rs.m_isAlphaEnable = renderState.m_isAlphaEnable;
	PushCommonEffectList(rs, &rs.m_isAlphaEnable);
	rs.m_Src = renderState.m_Src;
	PushCommonEffectList(rs, &rs.m_Src);
	rs.m_Des = renderState.m_Des;
	PushCommonEffectList(rs, &rs.m_Des);
	rs.m_SrcA = renderState.m_SrcA;
	PushCommonEffectList(rs, &rs.m_SrcA);
	rs.m_DesA = renderState.m_DesA;
	PushCommonEffectList(rs, &rs.m_DesA);

	rs.m_isStencil = renderState.m_isStencil;
	PushCommonEffectList(rs, &rs.m_isStencil);
	rs.m_StencilMask = renderState.m_StencilMask;
	PushCommonEffectList(rs, &rs.m_StencilMask);

	//rs.m_StencilFuncFront = renderState.m_StencilFuncFront;
	//rs.m_StencilFuncRefFront = renderState.m_StencilFuncRefFront;
	//rs.m_StencilFuncMaskFront = renderState.m_StencilFuncMaskFront;
	//rs.m_StencilFailFront = renderState.m_StencilFailFront;
	//rs.m_StencilZFailFront = renderState.m_StencilZFailFront;
	//rs.m_StencilZPassFront = renderState.m_StencilZPassFront;
	
	//rs.m_StencilFuncBack = renderState.m_StencilFuncBack;
	//rs.m_StencilFuncRefBack = renderState.m_StencilFuncRefBack;
	//rs.m_StencilFuncMaskBack = renderState.m_StencilFuncMaskBack;
	//rs.m_StencilFailBack = renderState.m_StencilFailBack;
	//rs.m_StencilZFailBack = renderState.m_StencilZFailBack;
	//rs.m_StencilZPassBack = renderState.m_StencilZPassBack;

	rs.m_PolygonMode = renderState.m_PolygonMode;
	PushCommonEffectList(rs, &rs.m_PolygonMode);

	rs.m_PointSize = renderState.m_PointSize;
	PushCommonEffectList(rs, &rs.m_PointSize);

	rs.m_LineWidth = renderState.m_LineWidth;
	PushCommonEffectList(rs, &rs.m_LineWidth);

	rs.m_ColorMask = renderState.m_ColorMask;
	PushCommonEffectList(rs, &rs.m_ColorMask);

	rs.m_ScissorX = renderState.m_ScissorX;
	PushCommonEffectList(rs, &rs.m_ScissorX);
	rs.m_ScissorY = renderState.m_ScissorY;
	PushCommonEffectList(rs, &rs.m_ScissorY);
	rs.m_ScissorZ = renderState.m_ScissorZ;
	PushCommonEffectList(rs, &rs.m_ScissorZ);
	rs.m_ScissorW = renderState.m_ScissorW;
	PushCommonEffectList(rs, &rs.m_ScissorW);

	rs.m_RenderQueue = renderState.m_RenderQueue;

	rs.ApplyStateSetting();
}

void MatUtil::ApplyStateVar(RenderState* rs, ParameterArray* paramArray)
{
	bool change = false;

	size_t count = rs->m_comEffectByVar.size();
	for (int index = 0; index < count; index++)
	{
		BaseStateType* baseType = rs->m_comEffectByVar[index].first;
		RHIDefine::ParameterSlot slot = rs->m_comEffectByVar[index].second;
		MaterialParameter* vObj = paramArray->GetParameter(slot);

		if (vObj != nullptr && vObj->IsSameType(MaterialParameterFloat::ms_Type))
		{
			const MaterialParameterFloat* input = static_cast<const MaterialParameterFloat*>(vObj);
			change |= baseType->SetValue(input->m_float);
		}
		else
		{
			const std::string& name = baseType->GetName();
			JYERROR("not found state var", name.c_str());
		}
	}

	if (change)
	{
		rs->ApplyStateSetting();
	}
}


NS_JYE_END