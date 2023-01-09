#pragma once

#include "Engine/private/Define.h"

NS_JYE_BEGIN

typedef int ShaderKey;

#define MaxKeyWords 256

namespace KeyWordUtil
{
	enum BuiltIn
	{
		NoLight = 0,
		SpotLight = 1,
		DirLight = 2,
		PointLight = 3,
		ShadowOff = 4,
		ShadowOn = 5,
		GPUSKIN4 = 6,
		GPUINSTANCING = 7,
	};
	void Init();
	void Clean();

	ShaderKey Create(const char* name);
	const char* GetKeyName(ShaderKey key);
}

NS_JYE_END