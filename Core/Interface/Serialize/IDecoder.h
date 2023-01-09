#pragma once

#include "../../Core.h"
#include "DataType.h"

NS_JYE_BEGIN

class CORE_API IDecoder
{
public:
	IDecoder() {}
	virtual ~IDecoder() {}
public:
	virtual uint GetSerializeLength() const = 0;
	virtual const char* GetStringField(int index) const = 0;
	virtual int GetIntField(int index) const = 0;
	virtual uint64 GetUInt64Field(int index) const = 0;
	virtual Serialize::DataType GetKeyType(int index) const = 0;
	virtual Serialize::DataType GetValueType(int index) const = 0;
	virtual bool AsBool(int index) const = 0;
	virtual short AsShort(int index) const = 0;
	virtual uint16 AsUShort(int index) const = 0;
	virtual int AsInt(int index) const = 0;
	virtual uint AsUInt(int index) const = 0;
	virtual float AsFloat(int index) const = 0;
	virtual int64 AsInt64(int index) const = 0;
	virtual uint64 AsUInt64(int index) const = 0;
	virtual double AsDouble(int index) const = 0;
	virtual const char* AsString(int index, uint* out_len) const = 0;
	virtual const IDecoder* AsClass(int index) const = 0;
	virtual float GetFloatFieldByName(const std::string& name) const = 0;
	virtual int GetIntFieldByName(const std::string& name) const = 0;
	virtual bool GetBoolFieldByName(const std::string& name) const = 0;
	virtual IDecoder* GetClassFieldByName(const std::string& name) const = 0;
	virtual bool HasField(const std::string& name) const = 0;
};

NS_JYE_END