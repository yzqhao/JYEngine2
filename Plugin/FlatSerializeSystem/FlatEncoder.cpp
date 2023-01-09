#include "FlatEncoder.h"
#include "FlatSerializer.h"

using namespace vfbs;

NS_JYE_BEGIN

FlatEncoder::FlatEncoder(FlatSerializer* ser, flatbuffers::FlatBufferBuilder* flat)
	: m_rpBuilder(flat)
	, m_rpFlatSerializer(ser)
{

}

FlatEncoder::~FlatEncoder()
{
}

IEncoder* FlatEncoder::GetChild()
{
	return m_rpFlatSerializer->GetEncoder();
}

void FlatEncoder::SetField(const char* name, uint nlen)
{
	auto flatname = m_rpBuilder->CreateString(name, nlen);
	auto flatkey = CreateTString(*m_rpBuilder, flatname);
	auto flatmember = CreateMember(*m_rpBuilder, Type_TString, flatkey.Union(), Type_TNull, NULL);
	m_Members.push_back(flatmember);
}

void FlatEncoder::SetField(const char* name, uint nlen, int v)
{
	auto flatname = m_rpBuilder->CreateString(name, nlen);
	auto flatkey = CreateTString(*m_rpBuilder, flatname);
	auto flatvalue = CreateTInt(*m_rpBuilder, v);
	auto flatmember = CreateMember(*m_rpBuilder, Type_TString, flatkey.Union(), Type_TInt, flatvalue.Union());
	m_Members.push_back(flatmember);
}

void FlatEncoder::SetField(const char* name, uint nlen, float v)
{
	auto flatname = m_rpBuilder->CreateString(name, nlen);
	auto flatkey = CreateTString(*m_rpBuilder, flatname);
	auto flatvalue = CreateTFloat(*m_rpBuilder, v);
	auto flatmember = CreateMember(*m_rpBuilder, Type_TString, flatkey.Union(), Type_TFloat, flatvalue.Union());
	m_Members.push_back(flatmember);
}

void FlatEncoder::SetField(const char* name, uint nlen, bool v)
{
	auto flatname = m_rpBuilder->CreateString(name, nlen);
	auto flatkey = CreateTString(*m_rpBuilder, flatname);
	auto flatvalue = CreateTBool(*m_rpBuilder, v);
	auto flatmember = CreateMember(*m_rpBuilder, Type_TString, flatkey.Union(), Type_TBool, flatvalue.Union());
	m_Members.push_back(flatmember);
}

void FlatEncoder::SetField(const char* name, uint nlen, const char* v, uint len)
{
	auto flatname = m_rpBuilder->CreateString(name, nlen);
	auto flatkey = CreateTString(*m_rpBuilder, flatname);
	auto flatstr = m_rpBuilder->CreateString(v, len);
	auto flatvalue = CreateTString(*m_rpBuilder, flatstr);
	auto flatmember = CreateMember(*m_rpBuilder, Type_TString, flatkey.Union(), Type_TString, flatvalue.Union());
	m_Members.push_back(flatmember);
}

void FlatEncoder::SetField(const char* name, uint nlen, const IEncoder* v)
{
	const FlatEncoder* flatencoder = static_cast<const FlatEncoder*>(v);
	auto flatname = m_rpBuilder->CreateString(name, nlen);
	auto flatkey = CreateTString(*m_rpBuilder, flatname);
	auto members = flatencoder->GetMemberVector();
	auto tclass = CreateTClass(*m_rpBuilder, members);
	auto flatmember = CreateMember(*m_rpBuilder, Type_TString, flatkey.Union(), Type_TClass, tclass.Union());
	m_Members.push_back(flatmember);
}

void FlatEncoder::SetField(const char* name, uint nlen, short v)
{
	auto flatname = m_rpBuilder->CreateString(name, nlen);
	auto flatkey = CreateTString(*m_rpBuilder, flatname);
	auto flatvalue = CreateTShort(*m_rpBuilder, v);
	auto flatmember = CreateMember(*m_rpBuilder, Type_TString, flatkey.Union(), Type_TShort, flatvalue.Union());
	m_Members.push_back(flatmember);
}

void FlatEncoder::SetField(const char* name, uint nlen, uint16 v)
{
	auto flatname = m_rpBuilder->CreateString(name, nlen);
	auto flatkey = CreateTString(*m_rpBuilder, flatname);
	auto flatvalue = CreateTUShort(*m_rpBuilder, v);
	auto flatmember = CreateMember(*m_rpBuilder, Type_TString, flatkey.Union(), Type_TUShort, flatvalue.Union());
	m_Members.push_back(flatmember);
}

void FlatEncoder::SetField(const char* name, uint nlen, uint v)
{
	auto flatname = m_rpBuilder->CreateString(name, nlen);
	auto flatkey = CreateTString(*m_rpBuilder, flatname);
	auto flatvalue = CreateTUInt(*m_rpBuilder, v);
	auto flatmember = CreateMember(*m_rpBuilder, Type_TString, flatkey.Union(), Type_TUInt, flatvalue.Union());
	m_Members.push_back(flatmember);
}

void FlatEncoder::SetField(const char* name, uint nlen, int64 v)
{
	auto flatname = m_rpBuilder->CreateString(name, nlen);
	auto flatkey = CreateTString(*m_rpBuilder, flatname);
	auto flatvalue = CreateTInt64(*m_rpBuilder, v);
	auto flatmember = CreateMember(*m_rpBuilder, Type_TString, flatkey.Union(), Type_TInt64, flatvalue.Union());
	m_Members.push_back(flatmember);
}

void FlatEncoder::SetField(const char* name, uint nlen, uint64 v)
{
	auto flatname = m_rpBuilder->CreateString(name, nlen);
	auto flatkey = CreateTString(*m_rpBuilder, flatname);
	auto flatvalue = CreateTUInt64(*m_rpBuilder, v);
	auto flatmember = CreateMember(*m_rpBuilder, Type_TString, flatkey.Union(), Type_TUInt64, flatvalue.Union());
	m_Members.push_back(flatmember);
}

void FlatEncoder::SetField(const char* name, uint nlen, double v)
{
	auto flatname = m_rpBuilder->CreateString(name, nlen);
	auto flatkey = CreateTString(*m_rpBuilder, flatname);
	auto flatvalue = CreateTDouble(*m_rpBuilder, v);
	auto flatmember = CreateMember(*m_rpBuilder, Type_TString, flatkey.Union(), Type_TDouble, flatvalue.Union());
	m_Members.push_back(flatmember);
}

void FlatEncoder::SetField(int index)
{
	auto flatkey = CreateTInt(*m_rpBuilder, index);
	auto flatmember = CreateMember(*m_rpBuilder, Type_TInt, flatkey.Union(), Type_TNull, NULL);
	m_Members.push_back(flatmember);
}

void FlatEncoder::SetField(int index, int v)
{
	auto flatkey = CreateTInt(*m_rpBuilder, index);
	auto flatvalue = CreateTInt(*m_rpBuilder, v);
	auto flatmember = CreateMember(*m_rpBuilder, Type_TInt, flatkey.Union(), Type_TInt, flatvalue.Union());
	m_Members.push_back(flatmember);
}

void FlatEncoder::SetField(int index, float v)
{
	auto flatkey = CreateTInt(*m_rpBuilder, index);
	auto flatvalue = CreateTFloat(*m_rpBuilder, v);
	auto flatmember = CreateMember(*m_rpBuilder, Type_TInt, flatkey.Union(), Type_TFloat, flatvalue.Union());
	m_Members.push_back(flatmember);
}

void FlatEncoder::SetField(int index, bool v)
{
	auto flatkey = CreateTInt(*m_rpBuilder, index);
	auto flatvalue = CreateTBool(*m_rpBuilder, v);
	auto flatmember = CreateMember(*m_rpBuilder, Type_TInt, flatkey.Union(), Type_TBool, flatvalue.Union());
	m_Members.push_back(flatmember);
}

void FlatEncoder::SetField(int index, const char* v, uint len)
{
	auto flatkey = CreateTInt(*m_rpBuilder, index);
	auto flatstr = m_rpBuilder->CreateString(v, len);
	auto flatvalue = CreateTString(*m_rpBuilder, flatstr);
	auto flatmember = CreateMember(*m_rpBuilder, Type_TInt, flatkey.Union(), Type_TString, flatvalue.Union());
	m_Members.push_back(flatmember);
}

void FlatEncoder::SetField(int index, const IEncoder* v)
{
	const FlatEncoder* flatencoder = static_cast<const FlatEncoder*>(v);
	auto flatkey = CreateTInt(*m_rpBuilder, index);
	auto members = flatencoder->GetMemberVector();
	auto tclass = CreateTClass(*m_rpBuilder, members);
	auto flatmember = CreateMember(*m_rpBuilder, Type_TInt, flatkey.Union(), Type_TClass, tclass.Union());
	m_Members.push_back(flatmember);
}

void FlatEncoder::SetField(int index, short v)
{
	auto flatkey = CreateTInt(*m_rpBuilder, index);
	auto flatvalue = CreateTShort(*m_rpBuilder, v);
	auto flatmember = CreateMember(*m_rpBuilder, Type_TInt, flatkey.Union(), Type_TShort, flatvalue.Union());
	m_Members.push_back(flatmember);
}

void FlatEncoder::SetField(int index, uint16 v)
{
	auto flatkey = CreateTInt(*m_rpBuilder, index);
	auto flatvalue = CreateTUShort(*m_rpBuilder, v);
	auto flatmember = CreateMember(*m_rpBuilder, Type_TInt, flatkey.Union(), Type_TUShort, flatvalue.Union());
	m_Members.push_back(flatmember);
}

void FlatEncoder::SetField(int index, uint v)
{
	auto flatkey = CreateTInt(*m_rpBuilder, index);
	auto flatvalue = CreateTUInt(*m_rpBuilder, v);
	auto flatmember = CreateMember(*m_rpBuilder, Type_TInt, flatkey.Union(), Type_TUInt, flatvalue.Union());
	m_Members.push_back(flatmember);
}

void FlatEncoder::SetField(int index, int64 v)
{
	auto flatkey = CreateTInt(*m_rpBuilder, index);
	auto flatvalue = CreateTInt64(*m_rpBuilder, v);
	auto flatmember = CreateMember(*m_rpBuilder, Type_TInt, flatkey.Union(), Type_TInt64, flatvalue.Union());
	m_Members.push_back(flatmember);
}

void FlatEncoder::SetField(int index, uint64 v)
{
	auto flatkey = CreateTInt(*m_rpBuilder, index);
	auto flatvalue = CreateTUInt64(*m_rpBuilder, v);
	auto flatmember = CreateMember(*m_rpBuilder, Type_TInt, flatkey.Union(), Type_TUInt64, flatvalue.Union());
	m_Members.push_back(flatmember);
}

void FlatEncoder::SetField(int index, double v)
{
	auto flatkey = CreateTInt(*m_rpBuilder, index);
	auto flatvalue = CreateTDouble(*m_rpBuilder, v);
	auto flatmember = CreateMember(*m_rpBuilder, Type_TInt, flatkey.Union(), Type_TDouble, flatvalue.Union());
	m_Members.push_back(flatmember);
}

void FlatEncoder::SetField(uint64 index)
{
	auto flatkey = CreateTUInt64(*m_rpBuilder, index);
	auto flatmember = CreateMember(*m_rpBuilder, Type_TUInt64, flatkey.Union(), Type_TNull, NULL);
	m_Members.push_back(flatmember);
}

void FlatEncoder::SetField(uint64 index, int v)
{
	auto flatkey = CreateTUInt64(*m_rpBuilder, index);
	auto flatvalue = CreateTInt(*m_rpBuilder, v);
	auto flatmember = CreateMember(*m_rpBuilder, Type_TUInt64, flatkey.Union(), Type_TInt, flatvalue.Union());
	m_Members.push_back(flatmember);
}

void FlatEncoder::SetField(uint64 index, float v)
{
	auto flatkey = CreateTUInt64(*m_rpBuilder, index);
	auto flatvalue = CreateTFloat(*m_rpBuilder, v);
	auto flatmember = CreateMember(*m_rpBuilder, Type_TUInt64, flatkey.Union(), Type_TFloat, flatvalue.Union());
	m_Members.push_back(flatmember);
}

void FlatEncoder::SetField(uint64 index, bool v)
{
	auto flatkey = CreateTUInt64(*m_rpBuilder, index);
	auto flatvalue = CreateTBool(*m_rpBuilder, v);
	auto flatmember = CreateMember(*m_rpBuilder, Type_TUInt64, flatkey.Union(), Type_TBool, flatvalue.Union());
	m_Members.push_back(flatmember);
}

void FlatEncoder::SetField(uint64 index, const char* v, uint len)
{
	auto flatkey = CreateTUInt64(*m_rpBuilder, index);
	auto flatstr = m_rpBuilder->CreateString(v, len);
	auto flatvalue = CreateTString(*m_rpBuilder, flatstr);
	auto flatmember = CreateMember(*m_rpBuilder, Type_TUInt64, flatkey.Union(), Type_TString, flatvalue.Union());
	m_Members.push_back(flatmember);
}

void FlatEncoder::SetField(uint64 index, const IEncoder* v)
{
	const FlatEncoder* flatencoder = static_cast<const FlatEncoder*>(v);
	auto flatkey = CreateTUInt64(*m_rpBuilder, index);
	auto members = flatencoder->GetMemberVector();
	auto tclass = CreateTClass(*m_rpBuilder, members);
	auto flatmember = CreateMember(*m_rpBuilder, Type_TUInt64, flatkey.Union(), Type_TClass, tclass.Union());
	m_Members.push_back(flatmember);
}

void FlatEncoder::SetField(uint64 index, short v)
{
	auto flatkey = CreateTUInt64(*m_rpBuilder, index);
	auto flatvalue = CreateTShort(*m_rpBuilder, v);
	auto flatmember = CreateMember(*m_rpBuilder, Type_TUInt64, flatkey.Union(), Type_TShort, flatvalue.Union());
	m_Members.push_back(flatmember);
}

void FlatEncoder::SetField(uint64 index, uint16 v)
{
	auto flatkey = CreateTUInt64(*m_rpBuilder, index);
	auto flatvalue = CreateTUShort(*m_rpBuilder, v);
	auto flatmember = CreateMember(*m_rpBuilder, Type_TUInt64, flatkey.Union(), Type_TUShort, flatvalue.Union());
	m_Members.push_back(flatmember);
}

void FlatEncoder::SetField(uint64 index, uint v)
{
	auto flatkey = CreateTUInt64(*m_rpBuilder, index);
	auto flatvalue = CreateTUInt(*m_rpBuilder, v);
	auto flatmember = CreateMember(*m_rpBuilder, Type_TUInt64, flatkey.Union(), Type_TUInt, flatvalue.Union());
	m_Members.push_back(flatmember);
}

void FlatEncoder::SetField(uint64 index, int64 v)
{
	auto flatkey = CreateTUInt64(*m_rpBuilder, index);
	auto flatvalue = CreateTInt64(*m_rpBuilder, v);
	auto flatmember = CreateMember(*m_rpBuilder, Type_TUInt64, flatkey.Union(), Type_TInt64, flatvalue.Union());
	m_Members.push_back(flatmember);
}

void FlatEncoder::SetField(uint64 index, uint64 v)
{
	auto flatkey = CreateTUInt64(*m_rpBuilder, index);
	auto flatvalue = CreateTUInt64(*m_rpBuilder, v);
	auto flatmember = CreateMember(*m_rpBuilder, Type_TUInt64, flatkey.Union(), Type_TUInt64, flatvalue.Union());
	m_Members.push_back(flatmember);
}

void FlatEncoder::SetField(uint64 index, double v)
{
	auto flatkey = CreateTUInt64(*m_rpBuilder, index);
	auto flatvalue = CreateTDouble(*m_rpBuilder, v);
	auto flatmember = CreateMember(*m_rpBuilder, Type_TUInt64, flatkey.Union(), Type_TDouble, flatvalue.Union());
	m_Members.push_back(flatmember);
}

flatbuffers::Offset< flatbuffers::Vector< flatbuffers::Offset< vfbs::Member > > > FlatEncoder::GetMemberVector() const
{
	return m_rpBuilder->CreateVector(m_Members.data(), m_Members.size());
}

void FlatEncoder::Clear()
{
	m_Members.clear();
}

NS_JYE_END