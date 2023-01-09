#include "FlatDecoder.h"
#include "FlatSerializer.h"
#include "Core/Interface/ILogSystem.h"

using namespace vfbs;

NS_JYE_BEGIN

using namespace Serialize;

template<typename T> T _As(const Member* member)
{
	switch (member->value_type())
	{
	case Type_TShort: return static_cast<T>(member->value_as_TShort()->value());
	case Type_TUShort:return static_cast<T>(member->value_as_TUShort()->value());
	case Type_TInt:return static_cast<T>(member->value_as_TInt()->value());
	case Type_TUInt:return static_cast<T>(member->value_as_TUInt()->value());
	case Type_TInt64:return static_cast<T>(member->value_as_TInt64()->value());
	case Type_TUInt64:return static_cast<T>(member->value_as_TUInt64()->value());
	case Type_TFloat:return static_cast<T>(member->value_as_TFloat()->value());
	case Type_TDouble:return static_cast<T>(member->value_as_TDouble()->value());
	default:JYERROR("decoder convert fail");
	}
	return T(0);
}

FlatDecoder::FlatDecoder(FlatSerializer* ser)
	: m_rpFlatSerializer(ser)
	, m_Size(0)
{

}

FlatDecoder::~FlatDecoder()
{
}

void FlatDecoder::Initializer(const TClass* c)
{
	auto pmembers = c->members();
	if (NULL != pmembers)
	{
		m_Size = pmembers->size();
		for (unsigned int i = 0; i < pmembers->size(); ++i)
		{
			auto member = pmembers->Get(i);
			m_MemberVector.push_back(member);
		}
	}
}

uint FlatDecoder::GetSerializeLength() const
{
	return m_Size;
}

const char* FlatDecoder::GetStringField(int index) const
{
	auto member = m_MemberVector[index];
	auto tf = static_cast<const TString*>(member->key());
	return tf->value()->c_str();
}

int FlatDecoder::GetIntField(int index) const
{
	auto member = m_MemberVector[index];
	auto tf = static_cast<const TInt*>(member->key());
	return tf->value();
}

float FlatDecoder::GetFloatFieldByName(const std::string& name) const
{
	int length = GetSerializeLength();
	float data = 0;
	for (int i = 1; i < length; i++)
	{
		if (GetValueType(i) == DT_CLASS)
		{
			data = AsClass(i)->GetFloatFieldByName(name);
		}
		else if (GetValueType(i) == DT_DOUBLE)
		{
			char* fieldname = const_cast<char*>(GetStringField(i));
			if (strcmp(fieldname, name.c_str()) == 0)
			{
				data = AsFloat(i);
				return data;
			}
		}
	}
	return data;
}

int FlatDecoder::GetIntFieldByName(const std::string& name) const
{
	int length = GetSerializeLength();
	int data = 0;
	for (int i = 1; i < length; i++)
	{
		if (GetValueType(i) == DT_CLASS)
		{
			data = AsClass(i)->GetIntFieldByName(name);
		}
		else if (GetValueType(i) == DT_DOUBLE)
		{
			char* fieldname = const_cast<char*>(GetStringField(i));
			if (strcmp(fieldname, name.c_str()) == 0)
			{
				data = AsInt(i);
				return data;
			}
		}
	}
	return data;
}

bool FlatDecoder::GetBoolFieldByName(const std::string& name) const
{
	int length = GetSerializeLength();
	bool data = false;
	for (int i = 1; i < length; i++)
	{
		if (GetValueType(i) == DT_CLASS)
		{
			data = AsClass(i)->GetIntFieldByName(name);
		}
		else if (GetValueType(i) == DT_BOOL)
		{
			char* fieldname = const_cast<char*>(GetStringField(i));
			if (strcmp(fieldname, name.c_str()) == 0)
			{
				data = AsBool(i);
				return data;
			}
		}
	}
	return data;
}

IDecoder* FlatDecoder::GetClassFieldByName(const std::string& name) const
{
	int length = GetSerializeLength();
	IDecoder* data = 0;
	for (int i = 1; i < length; i++)
	{
		if (GetValueType(i) == DT_CLASS)
		{
			char* fieldname = const_cast<char*>(GetStringField(i));
			if (strcmp(fieldname, name.c_str()) == 0)
			{
				data = const_cast<IDecoder*>(AsClass(i));
				return data;
			}
			else
			{
				data = AsClass(i)->GetClassFieldByName(name);
			}
		}
	}
	return data;
}

uint64 FlatDecoder::GetUInt64Field(int index) const
{
	auto member = m_MemberVector[index];
	auto tf = static_cast<const TUInt64*>(member->key());
	return tf->value();
}

DataType FlatDecoder::GetKeyType(int index) const
{
	auto member = m_MemberVector[index];
	return static_cast<DataType>(member->key_type() - 1);//看两个头文件
}

DataType FlatDecoder::GetValueType(int index) const
{
	auto member = m_MemberVector[index];
	return static_cast<DataType>(member->value_type() - 1);//看两个头文件
}

bool FlatDecoder::AsBool(int index) const
{
	auto member = m_MemberVector[index];
	auto tf = static_cast<const TBool*>(member->value());
	return tf->value();
}

int FlatDecoder::AsInt(int index) const
{
	auto member = m_MemberVector[index];
	return _As<int>(member);
}

float FlatDecoder::AsFloat(int index) const
{
	auto member = m_MemberVector[index];
	return _As<float>(member);
}

const char* FlatDecoder::AsString(int index, uint* out_len) const
{
	auto member = m_MemberVector[index];
	auto tf = static_cast<const TString*>(member->value());
	*out_len = tf->value()->size();
	return tf->value()->c_str();
}

short FlatDecoder::AsShort(int index) const
{
	auto member = m_MemberVector[index];
	return _As<short>(member);
}

uint16 FlatDecoder::AsUShort(int index) const
{
	auto member = m_MemberVector[index];
	return _As<uint16>(member);
}

uint FlatDecoder::AsUInt(int index) const
{
	auto member = m_MemberVector[index];
	return _As<uint>(member);
}

int64 FlatDecoder::AsInt64(int index) const
{
	auto member = m_MemberVector[index];
	return _As<int64>(member);
}

uint64 FlatDecoder::AsUInt64(int index) const
{
	auto member = m_MemberVector[index];
	return _As<uint64>(member);
}

double FlatDecoder::AsDouble(int index) const
{
	auto member = m_MemberVector[index];
	return _As<double>(member);
}

const IDecoder* FlatDecoder::AsClass(int index) const
{
	auto member = m_MemberVector[index];
	auto son = static_cast<const TClass*>(member->value());
	IDecoder* res = m_rpFlatSerializer->GetDecoder(son);
	return res;
}

void FlatDecoder::Clear()
{
	m_Size = 0;
	m_MemberVector.clear();
}

bool FlatDecoder::HasField(const std::string& name) const
{
	int length = GetSerializeLength();
	for (int i = 1; i < length; i++)
	{
		if (GetValueType(i) == DT_CLASS)
		{
			if (AsClass(i)->HasField(name))
			{
				return true;
			}
		}
		else
		{
			char* fieldname = const_cast<char*>(GetStringField(i));
			if (strcmp(fieldname, name.c_str()) == 0)
			{
				return true;
			}
		}
	}
	return false;
}

NS_JYE_END