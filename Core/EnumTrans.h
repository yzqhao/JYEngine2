
#pragma once

#include "Core.h"

NS_JYE_BEGIN

class CORE_API EnumTrans
{
public:
	EnumTrans();
	~EnumTrans();
	static EnumTrans& GetInstance();
	void Add(const char* EnumName, const char* Value);
	void Get(const char* EnumName, Vector<String>& AS);
protected:
	HashMap<String, Vector<String>> m_EnumSaver;
};

#define ADD_ENUM(EnumName,Value) \
	EnumTrans::GetInstance().Add((#EnumName), (#Value));
#define GET_ENUMARRAY(EnumName,Array) \
	EnumTrans::GetInstance().Get((#EnumName),Array);

NS_JYE_END
