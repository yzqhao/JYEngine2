
#pragma once

#include "Core/Interface/Serialize/IDecoder.h"
#include "Core/Nonclearvector.hpp"

#include "generic_generated.h"

NS_JYE_BEGIN

class FlatSerializer;

class FlatDecoder : public IDecoder
{
private:
	typedef Nonclearvector< const vfbs::Member* > MemberVector;
	typedef flatbuffers::Vector<flatbuffers::Offset<vfbs::Member> > Members;
public:
	FlatDecoder(FlatSerializer* ser);
	virtual ~FlatDecoder();

	virtual uint GetSerializeLength() const;
	virtual const char* GetStringField(int index) const;
	virtual int GetIntField(int index) const;
	virtual float GetFloatFieldByName(const std::string& name) const;
	virtual int GetIntFieldByName(const std::string& name) const;
	virtual bool GetBoolFieldByName(const std::string& name) const;
	virtual IDecoder* GetClassFieldByName(const std::string& name) const;
	virtual uint64 GetUInt64Field(int index) const;
	virtual Serialize::DataType GetKeyType(int index) const;
	virtual Serialize::DataType GetValueType(int index) const;
	virtual bool AsBool(int index) const;
	virtual short AsShort(int index) const;
	virtual uint16 AsUShort(int index) const;
	virtual int AsInt(int index) const;
	virtual uint AsUInt(int index) const;
	virtual float AsFloat(int index) const;
	virtual int64 AsInt64(int index) const;
	virtual uint64 AsUInt64(int index) const;
	virtual double AsDouble(int index) const;
	virtual const char* AsString(int index, uint* out_len) const;
	virtual const IDecoder* AsClass(int index) const;
	virtual bool HasField(const std::string& name) const override;
public:
	void Initializer(const vfbs::TClass* c);
	void Clear();


private:
	MemberVector	m_MemberVector;
	FlatSerializer* m_rpFlatSerializer;
	uint			m_Size;
};

NS_JYE_END
