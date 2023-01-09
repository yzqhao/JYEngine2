
#pragma once

#include "Core/Interface/Serialize/IEncoder.h"
#include "Core/Nonclearvector.hpp"

#include "generic_generated.h"

NS_JYE_BEGIN

class FlatSerializer;

class FlatEncoder : public IEncoder
{
private:
	typedef Nonclearvector< flatbuffers::Offset<vfbs::Member> > MemberVector;
private:
	flatbuffers::FlatBufferBuilder* m_rpBuilder;
	FlatSerializer* m_rpFlatSerializer;
	MemberVector					m_Members;
public:
	FlatEncoder(FlatSerializer* ser, flatbuffers::FlatBufferBuilder* flat);
	virtual ~FlatEncoder();

public:
	virtual IEncoder* GetChild();
	virtual void SetField(const char* name, uint nlen);//null±‰¡ø
	virtual void SetField(const char* name, uint nlen, bool v);
	virtual void SetField(const char* name, uint nlen, short v);
	virtual void SetField(const char* name, uint nlen, uint16 v);
	virtual void SetField(const char* name, uint nlen, int v);
	virtual void SetField(const char* name, uint nlen, uint v);
	virtual void SetField(const char* name, uint nlen, float v);
	virtual void SetField(const char* name, uint nlen, int64 v);
	virtual void SetField(const char* name, uint nlen, uint64 v);
	virtual void SetField(const char* name, uint nlen, double v);
	virtual void SetField(const char* name, uint nlen, const char* v, uint len);
	virtual void SetField(const char* name, uint nlen, const IEncoder* v);
	virtual void SetField(int index);
	virtual void SetField(int index, bool v);
	virtual void SetField(int index, short v);
	virtual void SetField(int index, uint16 v);
	virtual void SetField(int index, int v);
	virtual void SetField(int index, uint v);
	virtual void SetField(int index, float v);
	virtual void SetField(int index, int64 v);
	virtual void SetField(int index, uint64 v);
	virtual void SetField(int index, double v);
	virtual void SetField(int index, const char* v, uint len);
	virtual void SetField(int index, const IEncoder* v);
	virtual void SetField(uint64 index);
	virtual void SetField(uint64 index, bool v);
	virtual void SetField(uint64 index, short v);
	virtual void SetField(uint64 index, uint16 v);
	virtual void SetField(uint64 index, int v);
	virtual void SetField(uint64 index, uint v);
	virtual void SetField(uint64 index, float v);
	virtual void SetField(uint64 index, int64 v);
	virtual void SetField(uint64 index, uint64 v);
	virtual void SetField(uint64 index, double v);
	virtual void SetField(uint64 index, const char* v, uint len);
	virtual void SetField(uint64 index, const IEncoder* v);
public:
	void Clear();
	flatbuffers::Offset< flatbuffers::Vector< flatbuffers::Offset< vfbs::Member > > > GetMemberVector() const;

};

NS_JYE_END
