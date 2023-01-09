#pragma once

#include "../Core.h"

NS_JYE_BEGIN

class CORE_API IConfig
{
public:

	IConfig() {};
	virtual ~IConfig() {};

	virtual bool HasMember(const String& name) = 0;
	virtual int GetInt(const String& name) = 0;
	virtual bool GetBool(const String& name) = 0;
	virtual double GetDouble(const String& name) = 0;
	virtual String GetString(const String& name) = 0;
	virtual Vector<int> GetArrayInt(const String& name) = 0;
	virtual Vector<bool> GetArrayBool(const String& name) = 0;
	virtual Vector<double> GetArrayDouble(const String& name) = 0;
	virtual Vector<String> GetArrayString(const String& name) = 0;
	virtual std::map<String, String> GetObjectString(const String& name) = 0;
	virtual bool isString(const String& name) = 0;
	virtual bool isArrayString(const String& name) = 0;
	virtual bool isObjectString(const String& name) = 0;
};

NS_JYE_END