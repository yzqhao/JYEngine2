#pragma once

#pragma  once

#include "ParserMaterial.h"
#include "RHI/RHI.h"
#include "DefProperty.h"
#include "ShaderStructs.h"

NS_JYE_BEGIN

class RenderState;
class ParameterArray;

namespace MatUtil
{
	void SetDefPropertyValue(DefProperty& defProperty, ParserMaterial::ParamInfo& propertyInfo);
	bool GetShaderPathAndAPI(const String& metaPath, String& shaderPath, String api);
	bool IsTextureType(RHIDefine::VariablesType varType);
	bool IsArrayType(RHIDefine::VariablesType varType);
	void ConvertShaderCode(ShaderCode& outCode, const ParserMaterial::ShaderCode& shaderCode);
	bool IsNeedStage(RHIDefine::VariablesType varType);
	Vector<InputUniformInfo> UniformsFromParsed(const Vector<ParserMaterial::UniformData>& uniformData);
	Vector<InputAttributes> AttributesFromParsed(const Vector<ParserMaterial::AttributeData>& attributesData);

	void ConvertParserData(RenderState& rs, const ParserMaterial::ParserRenderState& ps);
	void ApplyStateVar(RenderState* rs, ParameterArray* paramArray);
}

NS_JYE_END