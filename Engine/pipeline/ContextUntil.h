#pragma once

#include "Engine/private/Define.h"

NS_JYE_BEGIN

class RenderContext;
struct ShaderUniform;
class ParameterArray;

class ContextUntil
{
public:
	
	static bool SetShaderParam(RenderContext& con, ShaderUniform& uniform, ParameterArray* spa, ParameterArray* jpa);
};

NS_JYE_END