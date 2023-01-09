
#pragma once

#include "RHI/RHI.h"
#include "ShaderResource.h"
#include "RenderState.h"

NS_JYE_BEGIN

class IShaderPass
{
public:
	RHIDefine::PassType passType;
	String passName;
};

class ShaderPass : public IShaderPass
{
public:
	RenderState state;
	Vector<ShaderResource> entitys;
};

NS_JYE_END
