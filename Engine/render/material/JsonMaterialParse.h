#pragma once

#include "ParserMaterial.h"

NS_JYE_BEGIN

class JsonMaterialParse
{
public:
	JsonMaterialParse();
	virtual ~JsonMaterialParse();

	bool ParseMaterial(const String& path, ParserMaterial& matData);
private:
	void _ParsePropertys(ParserMaterial& matData);
	void _ParseRenderQueue(ParserMaterial& matData);
};

NS_JYE_END