
#include "JsonMaterialParse.h"
#include "Core/Interface/ILogSystem.h"
#include "Core/Interface/IFile.h"
#include "Core/Interface/IFileSystem.h"
#include "json/JsonParserUtil.h"

#include <cassert>
#include <cassert>
#include <unordered_map>

using namespace JsonParserUtil;

NS_JYE_BEGIN

JsonMaterialParse::JsonMaterialParse()
{
}

JsonMaterialParse::~JsonMaterialParse()
{
}


static bool ParseProperties(
	ParseContext& context,
	const sajson::value& root,
	ParserMaterial& matData)
{
#define MAKE_PARAM_TYPE(n) { #n, ParserMaterial::VT_##n }
	static std::unordered_map<
		String,
		ParserMaterial::ParamsType
	> paramTypeMap =
	{
		MAKE_PARAM_TYPE(FLOAT),
		MAKE_PARAM_TYPE(FLOATRANGE),
		MAKE_PARAM_TYPE(VEC2),
		MAKE_PARAM_TYPE(VEC3),
		MAKE_PARAM_TYPE(VEC4),
		MAKE_PARAM_TYPE(COLOR),
		MAKE_PARAM_TYPE(TEXTURE1D),
		MAKE_PARAM_TYPE(TEXTURE2D),
		MAKE_PARAM_TYPE(TEXTURE3D),
		MAKE_PARAM_TYPE(TEXTURECUBE),
		MAKE_PARAM_TYPE(KEYWORDENUM),
	};
#undef MAKE_PARAM_TYPE
	matData.ShaderName = root.get_value_of_key(SALIT("ShaderName")).as_string();
	matData.Version = root.get_value_of_key(SALIT("Version")).get_integer_value();
	auto propObj = root.get_value_of_key(SALIT("Properties"));
	auto attrObj = root.get_value_of_key(SALIT("Attributes"));
	for (int i = 0; i < propObj.get_length(); ++i)
	{
		auto propName = propObj.get_object_key(i).as_string();
		auto propValueArr = propObj.get_object_value(i);
		auto propDesc = propValueArr.get_array_element(0).as_string();
		auto propType = propValueArr.get_array_element(1).as_string();
		auto propDefValueArr = propValueArr.get_array_element(2);

		ParserMaterial::ParamInfo curInfo;

		curInfo.name = propName;
		curInfo.type = paramTypeMap[propType];
		curInfo.desc = propDesc;

		if (attrObj.get_type() == sajson::TYPE_OBJECT)
		{
			int attrIndex = attrObj.find_object_key(SASTR(propName));
			if (attrIndex < attrObj.get_length())
			{
				curInfo.attribute = attrObj.get_object_value(attrIndex).as_string();
			}
		}
		memset(curInfo.defValue, 0, sizeof(curInfo.defValue));
		curInfo.defTexPath = "";

		switch (curInfo.type)
		{
		case ParserMaterial::VT_VEC4:
		case ParserMaterial::VT_COLOR:
			curInfo.defValue[3] = (float)get_double_number(propDefValueArr.get_array_element(3));
		case ParserMaterial::VT_VEC3:
		case ParserMaterial::VT_FLOATRANGE:
			curInfo.defValue[2] = (float)get_double_number(propDefValueArr.get_array_element(2));
		case ParserMaterial::VT_VEC2:
			curInfo.defValue[1] = (float)get_double_number(propDefValueArr.get_array_element(1));
		case ParserMaterial::VT_FLOAT:
			curInfo.defValue[0] = (float)get_double_number(propDefValueArr.get_array_element(0));
			break;
		case ParserMaterial::VT_TEXTURE1D:
		case ParserMaterial::VT_TEXTURE2D:
		case ParserMaterial::VT_TEXTURE3D:
		case ParserMaterial::VT_TEXTURECUBE:
			curInfo.defTexPath = propDefValueArr.get_array_element(0).as_string();
			break;
		default:
			JYLOGTAG(LogEngineRHI, "invalid property type %s\n", propType.c_str());
			return false;
		}

		matData.Params.emplace_back(std::move(curInfo));
	}
	return true;
}

static bool ParseRenderQueue(
	ParseContext& context,
	const sajson::value& root,
	ParserMaterial& matData)
{
	matData.RenderQueue = RHIDefine::MRQ_OPAQUE;
	const char* renderQueue = root.get_value_of_key(SALIT("RenderQueue")).as_cstring();
	const char* signChar = strchr(renderQueue, '+');
	if (*renderQueue != '%')
	{
		int queueOffset = 0;

		if (signChar != NULL)
		{
			queueOffset = atoi(signChar + 1);
		}
		else
		{
			signChar = strchr(renderQueue, '-');
			if (signChar != NULL)
			{
				queueOffset = -(atoi(signChar + 1));
			}
		}

		static char* pOpaue[] = {
			"Opaque",
			"Transparent",
			"Background",
			"TransparentCutout",
			"PostEffect",
			"Overlay"
		};

		static RHIDefine::RenderQueue pRenderQueues[]
		{
			RHIDefine::MRQ_OPAQUE,
			RHIDefine::MRQ_TRANSPARENT,
			RHIDefine::MRQ_BACKGROUND,
			RHIDefine::MRQ_TRANSPARENT_CUTOUT,
			RHIDefine::MRQ_OVERLAY
		};

		for (int i = 0; i < sizeof(pOpaue) / sizeof(char*); i++)
		{
			int maxLen = strlen(pOpaue[i]);
			if (strncmp(pOpaue[i], renderQueue, maxLen) == 0)
			{
				matData.RenderQueue = pRenderQueues[i] + queueOffset;
				break;
			}
		}
	}
	else
	{
		matData.RenderQueue = atoi(renderQueue) + 1;
		matData.RenderQueue = RHIDefine::MRQ_OPAQUE;
	}
	return true;
}
	
static bool ParseLightMode(
	ParseContext& context,
	const sajson::value& passJson,
	ParserMaterial::PassData& passData)
{
	static const std::unordered_map<String, RHIDefine::PassType> lightModeMap =
	{
		{ "ALWAYS", RHIDefine::PT_ALWAYS },
		{ "FORWARDBASE", RHIDefine::PT_FORWARDBASE },
		{ "FORWARDADD", RHIDefine::PT_FORWARDADD },
		{ "DEPTHPASS", RHIDefine::PT_DEPTHPASS },
		{ "GBUFFER", RHIDefine::PT_GBUFFER },
		{ "UNIVERSAL_POST_EFFECT", RHIDefine::PT_UNIVERSAL_POST_EFFECT },
	};
	passData.passType = RHIDefine::PT_FORWARDBASE;

	int lightModeIndex = passJson.find_object_key(SALIT("LIGHT_MODE"));
	if (lightModeIndex < passJson.get_length())
	{
		auto lightModeArr = passJson.get_object_value(lightModeIndex);
		if (lightModeArr.get_type() == sajson::TYPE_ARRAY && lightModeArr.get_length() > 0)
		{
			String mode = lightModeArr.get_array_element(0).as_string();
			if (lightModeMap.find(mode) != lightModeMap.end())
			{
				passData.passType = lightModeMap.find(mode)->second;
			}
		}
	}
	return true;
}
	
static const char* toVarName(const char* name)
{
	return name != nullptr && *name == '%' ? name + 1 : nullptr;
}

static sajson::value entry(const sajson::value& json, const sajson::string& key)
{
	int i = json.find_object_key(key);
	if (i < json.get_length())
	{
		return json.get_object_value(i);
	}

	return sajson::value();
}

static sajson::value entry(const sajson::value& json, int index)
{
	if (index >= 0 && index < json.get_length())
	{
		return json.get_array_element(index);
	}
	return sajson::value();
}

template<
	typename T,
	typename I,
	typename M,
	typename V,
	typename std::enable_if<
	std::is_same<T, ParserStateType<V>>::value&&
	std::is_same<M, std::unordered_map<String, V>>::value &&
	(std::is_same<I, sajson::string>::value || std::is_same<I, int>::value),
	int
	>::type = 0
>
	static bool SetValueOrVar(T& target, const sajson::value& json, const I& index, const M& map, V defValue)
{
		target.value = defValue;
	auto e = entry(json, index);
	if (e.get_type() == sajson::TYPE_NULL)
	{
		return false;
	}
	auto name = e.as_cstring();
	auto varName = toVarName(name);
	if (name == nullptr)
	{
		return false;
	}
	if (varName == nullptr)
	{
		auto iter = map.find(name);
		if (iter == map.end())
		{
			return false;
		}
			target.value = iter->second;
	}
	else
	{
		target.valueName = varName;
	}
	return true;
}

template<
	typename T,
	typename I,
	typename V,
	typename std::enable_if<
	std::is_same<T, ParserStateType<V>>::value &&
	(std::is_same<I, sajson::string>::value || std::is_same<I, int>::value),
	int
	>::type = 0
>
	static bool SetValueOrVar(T& target, const sajson::value& json, const I& index, V defValue)
{
		target.value = defValue;
	auto e = entry(json, index);
	if (e.get_type() == sajson::TYPE_NULL)
	{
		return false;
	}
	if (e.get_type() == sajson::TYPE_INTEGER)
	{
			target.value = e.get_integer_value();
	}
	else if (e.get_type() == sajson::TYPE_STRING)
	{
		auto name = e.as_cstring();
		auto varName = toVarName(name);
		if (varName != nullptr)
		{
			target.valueName = varName;
		}
	}
	return true;
}

static bool ParseAlphaState(
	ParseContext& context,
	const sajson::value& passJson,
	ParserMaterial::PassData& passData)
{
#define MAKE_BLEND(n) { #n, RHIDefine::BL_##n }
	static std::unordered_map<String, RHIDefine::Blend> blendMap =
	{
		MAKE_BLEND(ZERO),
		MAKE_BLEND(ONE),
		MAKE_BLEND(SRC_COLOR),
		MAKE_BLEND(ONE_MINUS_SRC_COLOR),
		MAKE_BLEND(SRC_ALPHA),
		MAKE_BLEND(ONE_MINUS_SRC_ALPHA),
		MAKE_BLEND(DST_ALPHA),
		MAKE_BLEND(ONE_MINUS_DST_ALPHA),
		MAKE_BLEND(DST_COLOR),
		MAKE_BLEND(ONE_MINUS_DST_COLOR),
	};
#undef MAKE_BLEND
	static std::unordered_map<String, RHIDefine::Boolean> boolMap =
	{
		{ "ALPHA_OFF", RHIDefine::MB_FALSE },
		{ "ALPHA_BLEND", RHIDefine::MB_TRUE },
	};
	auto& renderState = passData.renderState;
	renderState.m_isAlphaEnable = RHIDefine::MB_FALSE;
	int alphaModeIndex = passJson.find_object_key(SALIT("ALPHA_MODE"));
	if (alphaModeIndex == passJson.get_length())
	{
		//TODO:
		return true;
	}
	auto alphaModeArr = passJson.get_object_value(alphaModeIndex);
	if (alphaModeArr.get_type() != sajson::TYPE_ARRAY)
	{
		//TODO:
		return true;
	}
	SetValueOrVar(renderState.m_isAlphaEnable, alphaModeArr, 0, boolMap, RHIDefine::MB_FALSE);
	SetValueOrVar(renderState.m_Src, alphaModeArr, 1, blendMap, RHIDefine::BL_NULL);
	SetValueOrVar(renderState.m_Des, alphaModeArr, 2, blendMap, RHIDefine::BL_NULL);
	SetValueOrVar(renderState.m_SrcA, alphaModeArr, 3, blendMap, RHIDefine::BL_NULL);
	SetValueOrVar(renderState.m_DesA, alphaModeArr, 4, blendMap, RHIDefine::BL_NULL);
	return true;
}
static bool ParseDrawModeState(
	ParseContext& context,
	const sajson::value& passJson,
	ParserMaterial::PassData& passData)
{
	static const std::unordered_map<String, RHIDefine::CullFaceMode> cullFaceMap =
	{
		{ "CULL_FACE_OFF", RHIDefine::CFM_OFF },
		{ "CULL_FACE_FRONT", RHIDefine::CFM_FRONT },
		{ "CULL_FACE_BACK", RHIDefine::CFM_BACK },
	};
	static const std::unordered_map<String, RHIDefine::Boolean> boolMap =
	{
		{ "DEPTH_MASK_OFF", RHIDefine::MB_FALSE },
		{ "DEPTH_MASK_ON", RHIDefine::MB_TRUE },
		{ "DEPTH_TEST_OFF", RHIDefine::MB_FALSE },
		{ "DEPTH_TEST_ON", RHIDefine::MB_TRUE },
	};
	static const std::unordered_map<String, RHIDefine::Function> funcMap =
	{
		{ "DEPTH_FUNCTION_NEVER", RHIDefine::FN_NEVER },
		{ "DEPTH_FUNCTION_LESS", RHIDefine::FN_LESS },
		{ "DEPTH_FUNCTION_EQUAL", RHIDefine::FN_EQUAL },
		{ "DEPTH_FUNCTION_LEQUAL", RHIDefine::FN_LEQUAL },
		{ "DEPTH_FUNCTION_GREATER", RHIDefine::FN_GREATER },
		{ "DEPTH_FUNCTION_NOTEQUAL", RHIDefine::FN_NOTEQUAL },
		{ "DEPTH_FUNCTION_GEQUAL", RHIDefine::FN_GEQUAL },
		{ "DEPTH_FUNCTION_ALWAYS", RHIDefine::FN_ALWAYS },
	};

	auto& renderState = passData.renderState;
	renderState.m_isCullFace = RHIDefine::CFM_OFF;
	renderState.m_isDepthMask = RHIDefine::MB_FALSE;
	renderState.m_isDepthTest = RHIDefine::MB_FALSE;
	int drawModeIndex = passJson.find_object_key(SALIT("DRAW_MODE"));
	if (drawModeIndex == passJson.get_length())
	{
		//TODO:
		return true;
	}
	auto drawModeArr = passJson.get_object_value(drawModeIndex);
	if (drawModeArr.get_type() != sajson::TYPE_ARRAY)
	{
		//TODO:
		return true;
	}
	SetValueOrVar(renderState.m_isCullFace, drawModeArr, 0, cullFaceMap, RHIDefine::CFM_OFF);
	SetValueOrVar(renderState.m_isDepthMask, drawModeArr, 1, boolMap, RHIDefine::MB_FALSE);
	SetValueOrVar(renderState.m_isDepthTest, drawModeArr, 2, boolMap, RHIDefine::MB_FALSE);
	SetValueOrVar(renderState.m_DepthFunction, drawModeArr, 3, funcMap, RHIDefine::FN_LESS);
	return true;
}
static bool ParseColMaskState(
	ParseContext& context,
	const sajson::value& passJson,
	ParserMaterial::PassData& passData)
{
#define MAKE_COLOR(n) { #n, RHIDefine::CM_##n }
	static const std::unordered_map<std::string, RHIDefine::ColorMask> colorMaskMap =
	{
		MAKE_COLOR(COLOR_R),
		MAKE_COLOR(COLOR_G),
		MAKE_COLOR(COLOR_B),
		MAKE_COLOR(COLOR_A),
		MAKE_COLOR(COLOR_RGBA),
		MAKE_COLOR(COLOR_RGB),
	};
#undef MAKE_COLOR
	auto& renderState = passData.renderState;
	int colorMaskIndex = passJson.find_object_key(SALIT("COLOR_MASK"));
	if (colorMaskIndex == passJson.get_length())
	{
		return true;
	}
	auto colorMaskMArr = passJson.get_object_value(colorMaskIndex);
	if (colorMaskMArr.get_type() != sajson::TYPE_ARRAY)
	{
		return true;
	}
	SetValueOrVar(renderState.m_ColorMask, colorMaskMArr, 0, colorMaskMap, RHIDefine::CM_COLOR_NONE);
	return true;
}

static bool ParseStencilState(
	ParseContext& context,
	const sajson::value& passJson,
	ParserMaterial::PassData& passData)
{
	static const std::unordered_map<String, RHIDefine::Boolean> boolMap =
	{
		{ "STENCIL_OFF", RHIDefine::MB_FALSE },
		{ "STENCIL_ON", RHIDefine::MB_TRUE },
	};
	static const std::unordered_map<String, RHIDefine::Function> funcMap =
	{
		{ "STENCIL_FUNCTION_NEVER", RHIDefine::FN_NEVER },
		{ "STENCIL_FUNCTION_LESS", RHIDefine::FN_LESS },
		{ "STENCIL_FUNCTION_EQUAL", RHIDefine::FN_EQUAL },
		{ "STENCIL_FUNCTION_LEQUAL", RHIDefine::FN_LEQUAL },
		{ "STENCIL_FUNCTION_GREATER", RHIDefine::FN_GREATER },
		{ "STENCIL_FUNCTION_NOTEQUAL", RHIDefine::FN_NOTEQUAL },
		{ "STENCIL_FUNCTION_GEQUAL", RHIDefine::FN_GEQUAL },
		{ "STENCIL_FUNCTION_ALWAYS", RHIDefine::FN_ALWAYS },
	};
	static const std::unordered_map<String, RHIDefine::Operation> opMap =
	{
		{ "STENCIL_OPERATION_ZERO", RHIDefine::ON_ZERO, },
		{ "STENCIL_OPERATION_ONE", RHIDefine::ON_ONE, },
		{ "STENCIL_OPERATION_KEEP", RHIDefine::ON_KEEP, },
		{ "STENCIL_OPERATION_REPLACE", RHIDefine::ON_REPLACE, },
		{ "STENCIL_OPERATION_INCR", RHIDefine::ON_INCR, },
		{ "STENCIL_OPERATION_DECR", RHIDefine::ON_DECR, },
		{ "STENCIL_OPERATION_INVERT", RHIDefine::ON_INVERT, },
		{ "STENCIL_OPERATION_INCR_WRAP", RHIDefine::ON_INCR_WRAP, },
		{ "STENCIL_OPERATION_DECR_WRAP", RHIDefine::ON_DECR_WRAP, },
	};
	auto& renderState = passData.renderState;
	renderState.m_isStencil = RHIDefine::MB_FALSE;
	int stencilModeIndex = passJson.find_object_key(SALIT("STENCIL_MODE"));
	if (stencilModeIndex == passJson.get_length())
	{
		return true;
	}
	auto stencilModeArr = passJson.get_object_value(stencilModeIndex);
	if (stencilModeArr.get_type() != sajson::TYPE_ARRAY)
	{
		return true;
	}
	SetValueOrVar(renderState.m_isStencil, stencilModeArr, 0, boolMap, RHIDefine::MB_FALSE);
	SetValueOrVar(renderState.m_StencilMask, stencilModeArr, 1, (uint32_t)0);

	SetValueOrVar(renderState.m_StencilFuncFront, stencilModeArr, 2, funcMap, RHIDefine::FN_NEVER);
	SetValueOrVar(renderState.m_StencilFuncRefFront, stencilModeArr, 3, (uint32_t)0);
	SetValueOrVar(renderState.m_StencilFuncMaskFront, stencilModeArr, 4, (uint32_t)0);
	SetValueOrVar(renderState.m_StencilFailFront, stencilModeArr, 5, opMap, RHIDefine::ON_ZERO);
	SetValueOrVar(renderState.m_StencilZFailFront, stencilModeArr, 6, opMap, RHIDefine::ON_ZERO);
	SetValueOrVar(renderState.m_StencilZPassFront, stencilModeArr, 7, opMap, RHIDefine::ON_ZERO);

	if (stencilModeArr.get_length() >= 9)
	{
		SetValueOrVar(renderState.m_StencilFuncBack, stencilModeArr, 8, funcMap, RHIDefine::FN_NEVER);
		SetValueOrVar(renderState.m_StencilFuncRefBack, stencilModeArr, 9, (uint32_t)0);
		SetValueOrVar(renderState.m_StencilFuncMaskBack, stencilModeArr, 10, (uint32_t)0);
		SetValueOrVar(renderState.m_StencilFailBack, stencilModeArr, 11, opMap, RHIDefine::ON_ZERO);
		SetValueOrVar(renderState.m_StencilZFailBack, stencilModeArr, 12, opMap, RHIDefine::ON_ZERO);
		SetValueOrVar(renderState.m_StencilZPassBack, stencilModeArr, 13, opMap, RHIDefine::ON_ZERO);
	}
	return true;
}

static bool ParsePolygonModeState(
	ParseContext& context,
	const sajson::value& passJson,
	ParserMaterial::PassData& passData)
{
	static const std::unordered_map<String, RHIDefine::PolygonMode> polyModeMap =
	{
		{ "POLYGON_POINT", RHIDefine::PM_POINT },
		{ "POLYGON_LINE", RHIDefine::PM_LINE },
		{ "POLYGON_TRIANGLE", RHIDefine::PM_TRIANGLE },
	};
	auto& renderState = passData.renderState;
	renderState.m_PolygonMode = RHIDefine::PM_TRIANGLE;
	int polygonModeIndex = passJson.find_object_key(SALIT("POLYGON_MODE"));
	if (polygonModeIndex < passJson.get_length())
	{
		auto polyModeArr = passJson.get_object_value(polygonModeIndex);
		if (polyModeArr.get_type() == sajson::TYPE_ARRAY)
		{
			SetValueOrVar(renderState.m_PolygonMode, polyModeArr, 0, polyModeMap, RHIDefine::PM_TRIANGLE);
		}
	}
	SetValueOrVar(renderState.m_PointSize, passJson, SALIT("POINT_SIZE"), (uint32_t)1);
	SetValueOrVar(renderState.m_LineWidth, passJson, SALIT("LINE_WIDTH"), (uint32_t)1);
	return true;
}

static bool ParseScissorState(
	ParseContext& context,
	const sajson::value& passJson,
	ParserMaterial::PassData& passData)
{
	auto& state = passData.renderState;
	int scissorModeIndex = passJson.find_object_key(SALIT("SCISSOR_MODE"));
	if (scissorModeIndex == passJson.get_length())
	{
		return true;
	}
	auto scissorModeArr = passJson.get_object_value(scissorModeIndex);
	if (scissorModeArr.get_type() != sajson::TYPE_ARRAY)
	{
		return true;
	}
	if (scissorModeArr.get_length() != 4)
	{
		return true;
	}
	SetValueOrVar(state.m_ScissorX, scissorModeArr, 0, 0U);
	SetValueOrVar(state.m_ScissorY, scissorModeArr, 1, 0U);
	SetValueOrVar(state.m_ScissorZ, scissorModeArr, 2, 0U);
	SetValueOrVar(state.m_ScissorW, scissorModeArr, 3, 0U);
	return true;
}

static bool ParseRenderState(
	ParseContext& context,
	const sajson::value& passJson,
	ParserMaterial::PassData& passData)
{
	ParseAlphaState(context, passJson, passData);
	ParseDrawModeState(context, passJson, passData);
	ParseColMaskState(context, passJson, passData);
	ParseStencilState(context, passJson, passData);
	ParsePolygonModeState(context, passJson, passData);
	ParseScissorState(context, passJson, passData);
	return true;
}

static bool ParseUniforms(
	ParseContext& context,
	sajson::value& uniforms,
	Vector<ParserMaterial::UniformData>& uniformData)
{
	size_t size = uniforms.get_length();
	uniformData.reserve(size);
	for (int i = 0; i < size; ++i)
	{
		auto itemObj = uniforms.get_array_element(i);
		ParserMaterial::UniformData ud{
			itemObj.get_value_of_key(SALIT("varName")).as_string(),
			itemObj.get_value_of_key(SALIT("varType")).as_string(),
			(uint16_t)atoi(itemObj.get_value_of_key(SALIT("varNum")).as_cstring()),
			(uint16_t)atoi(itemObj.get_value_of_key(SALIT("varSit")).as_cstring()),
			(uint16_t)atoi(itemObj.get_value_of_key(SALIT("varRegIndex")).as_cstring()),
			(uint16_t)atoi(itemObj.get_value_of_key(SALIT("varRegCount")).as_cstring())
		};
		if (strcmp(ud.type.c_str(), "constBuffer") == 0)
		{
			int cbindex = itemObj.find_object_key(SALIT("subVar"));
			if (cbindex < itemObj.get_length())
			{
				auto uniformArr = itemObj.get_object_value(cbindex);
				if (uniformArr.get_type() == sajson::TYPE_ARRAY && uniformArr.get_length() > 0)
				{
					size_t size = uniformArr.get_length();
					ud.subIndexs.reserve(size);
					for (int i = 0; i < size; ++i)
					{
						ud.subIndexs.push_back(uniformArr.get_array_element(i).get_integer_value());
					}
				}
			}
		}
		uniformData.emplace_back(ud);
	}
	return true;
}

static bool ParseIntegerArray(
	ParseContext& context,
	const sajson::value& arr,
	Vector<int>& data)
{
	size_t size = arr.get_length();
	data.reserve(size);
	for (int i = 0; i < size; ++i)
	{
		data.push_back(arr.get_array_element(i).get_integer_value());
	}
	return true;
}

static bool ParseAttributes(
	ParseContext& context,
	const sajson::value& attrArr,
	Vector<ParserMaterial::AttributeData>& attributeData)
{
	size_t size = attrArr.get_length();
	attributeData.reserve(size);
	for (int i = 0; i < size; ++i)
	{
		auto itemObj = attrArr.get_array_element(i);
		attributeData.emplace_back(ParserMaterial::AttributeData{
			itemObj.get_value_of_key(SALIT("attName")).as_string(),
			(uint16_t)atoi(itemObj.get_value_of_key(SALIT("attID")).as_cstring())
			});
	}
	return true;
}

static bool ParseShader(
	ParseContext& context,
	const char* shaderCode,
	ParserMaterial::ShaderCode& shaderData)
{
	shaderData.compress = false;
	shaderData.blockSize = 0;
	shaderData.blockIndex = 0;
	shaderData.blockOffset = 0;
	shaderData.shaderCode = shaderCode;
	return true;
}

static bool ParseShader(
	ParseContext& context,
	const sajson::value& compArr,
	ParserMaterial::ShaderCode& shaderData)
{
	shaderData.compress = true;
	shaderData.blockIndex = compArr.get_array_element(0).get_integer_value();
	shaderData.blockOffset = compArr.get_array_element(1).get_integer_value();
	shaderData.blockSize = compArr.get_array_element(2).get_integer_value();
	return true;
}

static bool ParsePrograms(
	ParseContext& context,
	const sajson::value& passJson,
	ParserMaterial::PassData& passData)
{
	int passUniformsIndex = passJson.find_object_key(SALIT("UNIFORMS"));
	if (passUniformsIndex < passJson.get_length())
	{
		auto uniformArr = passJson.get_object_value(passUniformsIndex);
		if (uniformArr.get_length() > 0)
		{
			passData.indexingUniforms = true;
			ParseUniforms(context, uniformArr, passData.passUniforms);
		}
	}
	int passAttributesIndex = passJson.find_object_key(SALIT("ATTRIBUTES"));
	if (passAttributesIndex < passJson.get_length())
	{
		auto attributeArr = passJson.get_object_value(passAttributesIndex);
		if (attributeArr.get_length() > 0)
		{
			passData.indexingAttributes = true;
			ParseAttributes(context, attributeArr, passData.passAttributes);
		}
	}

	auto programArr = passJson.get_value_of_key(SALIT("Programs"));

	bool foundApi = false;
	for (int i = 0; i < programArr.get_length(); ++i)
	{
		auto programItem = programArr.get_array_element(i);
		auto api = programItem.get_value_of_key(SALIT("shaderApi")).as_string();
		if (context.shaderApi == api)
		{
			foundApi = true;
			break;
		}
	}
	if (!foundApi)
	{
		context.shaderApi = "gles2";
	}
	{
		passData.programDatas.resize(programArr.get_length());
	}
	for (int i = 0; i < programArr.get_length(); ++i)
	{
		auto programObj = programArr.get_array_element(i);
			
		auto& programData = passData.programDatas[i];
		bool hasVsUniform = false;
		bool hasPsUniform = false;

		programData.shaderAPI = context.shaderApi;

		int keyWordsIndex = programObj.find_object_key(SALIT("keyWords"));
		if (keyWordsIndex < programObj.get_length())
		{
			auto keyWordsArr = programObj.get_object_value(keyWordsIndex);
			if (keyWordsArr.get_type() == sajson::TYPE_ARRAY)
			{
				auto& keywordsData = programData.keyWords;
				keywordsData.reserve(keyWordsArr.get_length());
				for (int j = 0; j < keyWordsArr.get_length(); ++j)
				{
					keywordsData.emplace_back(keyWordsArr.get_array_element(j).as_string());
				}
			}
		}

		int vsUniformsIndex = programObj.find_object_key(SALIT("vsUniforms"));
		if (vsUniformsIndex < programObj.get_length())
		{
			auto uniformArr = programObj.get_object_value(vsUniformsIndex);
			if (uniformArr.get_type() == sajson::TYPE_ARRAY && uniformArr.get_length() > 0)
			{
				if (passData.indexingUniforms)
				{
					ParseIntegerArray(context, uniformArr, programData.vsUniformIndices);
				}
				else
				{
					ParseUniforms(context, uniformArr, programData.vsUniforms);
				}
				hasVsUniform = true;
			}
		}

		int psUniformsIndex = programObj.find_object_key(SALIT("psUniforms"));
		if (psUniformsIndex < programObj.get_length())
		{
			auto uniformArr = programObj.get_object_value(psUniformsIndex);
			if (uniformArr.get_type() == sajson::TYPE_ARRAY && uniformArr.get_length() > 0)
			{
				if (passData.indexingUniforms)
				{
					ParseIntegerArray(context, uniformArr, programData.psUniformIndices);
				}
				else
				{
					ParseUniforms(context, uniformArr, programData.psUniforms);
				}
				hasPsUniform = true;
			}
		}

		int vsAttributesIndex = programObj.find_object_key(SALIT("vsAttributes"));
		if (vsAttributesIndex < programObj.get_length())
		{
			auto attrArr = programObj.get_object_value(vsAttributesIndex);
			if (attrArr.get_type() == sajson::TYPE_ARRAY && attrArr.get_length() > 0)
			{
				if (passData.indexingAttributes)
				{
					ParseIntegerArray(context, attrArr, programData.vsAttributeIndices);
				}
				else
				{
					ParseAttributes(context, attrArr, programData.vsAttributes);
				}
			}
		}

		int vsShaderIndex = programObj.find_object_key(SALIT("vsShader"));
		if (vsShaderIndex < programObj.get_length())
		{
			auto shaderValue = programObj.get_object_value(vsShaderIndex);
			if (shaderValue.get_type() == sajson::TYPE_STRING)
			{
				ParseShader(context, shaderValue.as_cstring(), programData.vshaderCode);
			}
			else if (shaderValue.get_type() == sajson::TYPE_ARRAY)
			{
				ParseShader(context, shaderValue, programData.vshaderCode);
			}
		}

		int psShaderIndex = programObj.find_object_key(SALIT("psShader"));
		if (psShaderIndex < programObj.get_length())
		{
			auto shaderValue = programObj.get_object_value(psShaderIndex);
			if (shaderValue.get_type() == sajson::TYPE_STRING)
			{
				ParseShader(context, shaderValue.as_cstring(), programData.pshaderCode);
			}
			else if (shaderValue.get_type() == sajson::TYPE_ARRAY)
			{
				ParseShader(context, shaderValue, programData.pshaderCode);
			}
		}

		int vsBufferSizeIndex = programObj.find_object_key(SALIT("vsBufferSize"));
		if (vsBufferSizeIndex < programObj.get_length())
		{
			auto bufferSizeValue = programObj.get_object_value(vsBufferSizeIndex);
			if (bufferSizeValue.get_type() == sajson::TYPE_INTEGER)
			{
				programData.vsBufferSize = (uint16_t)bufferSizeValue.get_integer_value();
			}
		}

		int psBufferSizeIndex = programObj.find_object_key(SALIT("psBufferSize"));
		if (psBufferSizeIndex < programObj.get_length())
		{
			auto bufferSizeValue = programObj.get_object_value(psBufferSizeIndex);
			if (bufferSizeValue.get_type() == sajson::TYPE_INTEGER)
			{
				programData.psBufferSize = (uint16_t)bufferSizeValue.get_integer_value();
			}
		}
	}
	return true;
}

static bool ParsePass(
	ParseContext& context,
	const sajson::value& passJson,
	ParserMaterial::PassData& passData)
{
	ParseLightMode(context, passJson, passData);
	ParseRenderState(context, passJson, passData);
	ParsePrograms(context, passJson, passData);
	return true;
}

static bool ParsePasses(
	ParseContext& context,
	const sajson::value& root,
	ParserMaterial& matData)
{
	auto passArr = root.get_value_of_key(SALIT("PassNames"));
	for (int i = 0; i < passArr.get_length(); i++)
	{
		matData.PassDatas.emplace_back();
		auto& passData = matData.PassDatas.back();
		auto passName = passArr.get_array_element(i).as_string();
		passData.passName = passName;
		if (!ParsePass(context, root.get_value_of_key(SASTR(passName)), passData))
		{
			return false;
		}
	}
	return true;
}


bool JsonMaterialParse::ParseMaterial(const String& path, ParserMaterial& matData)
{
	IFile* readfile = IFileSystem::Instance()->FileFactory(path);
	if (!readfile->OpenFile(IFile::AT_READ))
	{
		IFileSystem::Instance()->RecycleBin(readfile);
		return false;
	}

	String tempcontext;
	tempcontext.resize(readfile->GetSize());
	readfile->ReadFile((void*)tempcontext.data(), readfile->GetSize());
	const sajson::document& doc = ParseJson(tempcontext);
	if (!doc.is_valid())
	{
		JY_ASSERT(false);
		JYLOGTAG(LogEngineRHI, "Json Parse Failed: (%d,%d) %s",
			doc.get_error_line(),
			doc.get_error_column(),
			doc.get_error_message_as_cstring());
		return false;
	}
	sajson::value root = doc.get_root();

	ParseContext context;
	//context.shaderApi = shaderApi;
	context.materialPath = path;
	{
		ParseProperties(context, root, matData);
		ParseRenderQueue(context, root, matData);
		ParsePasses(context, root, matData);
	}

	readfile->CloseFile();
	IFileSystem::Instance()->RecycleBin(readfile);
	return true;
}

NS_JYE_END