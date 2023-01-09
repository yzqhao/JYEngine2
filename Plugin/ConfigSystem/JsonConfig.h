
#pragma once

#include "private/Define.h"
#include "Core/Interface/IConfig.h"

#include <rapidjson/document.h>     // rapidjson's DOM-style API

NS_JYE_BEGIN

class JsonConfigure : public IConfig
{
private:
	rapidjson::Document* m_pDocument;
public:
	JsonConfigure(void);
	virtual ~JsonConfigure(void);
public:
	virtual bool HasMember(const String& name);
	virtual int GetInt(const String& name);
	virtual bool GetBool(const String& name);
	virtual double GetDouble(const String& name);
	virtual String GetString(const String& name);
	virtual Vector<int> GetArrayInt(const String& name);
	virtual Vector<bool> GetArrayBool(const String& name);
	virtual Vector<double> GetArrayDouble(const String& name);
	virtual Vector<String> GetArrayString(const String& name);
	virtual std::map<String, String> GetObjectString(const String& name);
	virtual bool isString(const String& name);
	virtual bool isArrayString(const String& name);
	virtual bool isObjectString(const String& name);
public:
	bool Parse(const String& doc);
};

NS_JYE_END
