
#pragma once

#include "private/Define.h"

NS_JYE_BEGIN

class MaterialEntity;

namespace MatTool
{
	void CreateMat(const String& path);
	void CreateMat(const String& path, const String& shaderpath);
	void SaveMat(MaterialEntity* mat);
}

NS_JYE_END