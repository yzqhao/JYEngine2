
#include "JsonConfig.h"
#include "Core/Interface/ILogSystem.h"

using namespace rapidjson;

NS_JYE_BEGIN

JsonConfigure::JsonConfigure(void)
	:m_pDocument(_NEW Document)
{
}
//---------------------------------------------------------------------------------------------------------
JsonConfigure::~JsonConfigure(void)
{
	SAFE_DELETE(m_pDocument);
}
//---------------------------------------------------------------------------------------------------------
bool JsonConfigure::Parse(const String& doc)
{
	m_pDocument->Parse(doc.c_str());
	return !m_pDocument->HasParseError();
}
//---------------------------------------------------------------------------------------------------------
int JsonConfigure::GetInt(const String& name)
{
	int res = 0;
	if (m_pDocument->HasMember(name.c_str()))
	{
		const Value& nodeValue = (*m_pDocument)[name.c_str()];
		res = nodeValue.GetInt();
	}
	else
	{
		JYERROR("Unkown json member %s", name.c_str());
	}
	return res;
}
//---------------------------------------------------------------------------------------------------------
bool JsonConfigure::GetBool(const String& name)
{
	bool res = false;
	if (m_pDocument->HasMember(name.c_str()))
	{
		const Value& nodeValue = (*m_pDocument)[name.c_str()];
		res = nodeValue.GetBool();
	}
	else
	{
		JYERROR("Unkown json member %s", name.c_str());
	}
	return res;
}
//---------------------------------------------------------------------------------------------------------
double JsonConfigure::GetDouble(const String& name)
{
	double res = 0.0;
	if (m_pDocument->HasMember(name.c_str()))
	{
		const Value& nodeValue = (*m_pDocument)[name.c_str()];
		res = nodeValue.GetDouble();
	}
	else
	{
		JYERROR("Unkown json member %s", name.c_str());
	}
	return res;
}
//---------------------------------------------------------------------------------------------------------
String JsonConfigure::GetString(const String& name)
{
	String res;
	if (m_pDocument->HasMember(name.c_str()))
	{
		const Value& nodeValue = (*m_pDocument)[name.c_str()];
		res = nodeValue.GetString();
	}
	else
	{
		JYERROR("Unkown json member %s", name.c_str());
	}
	return res;
}
//---------------------------------------------------------------------------------------------------------
Vector<int> JsonConfigure::GetArrayInt(const String& name)
{
	Vector<int> res;
	if (m_pDocument->HasMember(name.c_str()))
	{
		const Value& arraynode = (*m_pDocument)[name.c_str()];
		int nCount = arraynode.Size();
		res.reserve(nCount);
		for (int i = 0; i < nCount; ++i)
		{
			res.push_back(arraynode[i].GetInt());
		}
	}
	else
	{
		JYERROR("Unkown json member %s", name.c_str());
	}
	return res;
}
//---------------------------------------------------------------------------------------------------------
Vector<bool> JsonConfigure::GetArrayBool(const String& name)
{
	Vector<bool> res;
	if (m_pDocument->HasMember(name.c_str()))
	{
		const Value& arraynode = (*m_pDocument)[name.c_str()];
		int nCount = arraynode.Size();
		res.reserve(nCount);
		for (int i = 0; i < nCount; ++i)
		{
			res.push_back(arraynode[i].GetBool());
		}
	}
	else
	{
		JYERROR("Unkown json member %s", name.c_str());
	}
	return res;
}
//---------------------------------------------------------------------------------------------------------
Vector<double> JsonConfigure::GetArrayDouble(const String& name)
{
	Vector<double> res;
	if (m_pDocument->HasMember(name.c_str()))
	{
		const Value& arraynode = (*m_pDocument)[name.c_str()];
		int nCount = arraynode.Size();
		res.reserve(nCount);
		for (int i = 0; i < nCount; ++i)
		{
			res.push_back(arraynode[i].GetDouble());
		}
	}
	else
	{
		JYERROR("Unkown json member %s", name.c_str());
	}
	return res;
}
//---------------------------------------------------------------------------------------------------------
Vector<String> JsonConfigure::GetArrayString(const String& name)
{
	Vector<String> res;
	if (m_pDocument->HasMember(name.c_str()))
	{
		const Value& arraynode = (*m_pDocument)[name.c_str()];
		int nCount = arraynode.Size();
		res.reserve(nCount);
		for (int i = 0; i < nCount; ++i)
		{
			res.push_back(arraynode[i].GetString());
		}
	}
	else
	{
		JYERROR("Unkown json member %s", name.c_str());
	}
	return res;
}
//---------------------------------------------------------------------------------------------------------
bool JsonConfigure::HasMember(const String& name)
{
	return m_pDocument->HasMember(name.c_str());
}
//---------------------------------------------------------------------------------------------------------
bool JsonConfigure::isString(const String& name)
{
	if (m_pDocument->HasMember(name.c_str()))
	{
		const Value& nodeValue = (*m_pDocument)[name.c_str()];
		return nodeValue.IsString();
	}
	JYERROR("Unkown json member %s", name.c_str());
	return false;
}
//---------------------------------------------------------------------------------------------------------
bool JsonConfigure::isArrayString(const String& name)
{
	bool res = true;;
	if (m_pDocument->HasMember(name.c_str()))
	{
		const Value& arraynode = (*m_pDocument)[name.c_str()];
		if (arraynode.IsArray())
		{
			int nCount = arraynode.Size();
			for (int i = 0; i < nCount; ++i)
			{
				res = res && arraynode[i].IsString();
			}
		}
		else
		{
			res = false;
		}
	}
	else
	{
		res = false;
		JYERROR("Unkown json member %s", name.c_str());
	}
	return res;
}
//---------------------------------------------------------------------------------------------------------
bool JsonConfigure::isObjectString(const String& name)
{
	bool res = true;;
	if (m_pDocument->HasMember(name.c_str()))
	{
		const Value& arraynode = (*m_pDocument)[name.c_str()];
		if (arraynode.IsObject())
		{
			const rapidjson::Document& objnode =
				static_cast<const rapidjson::Document&>(arraynode);

			for (Value::ConstMemberIterator itr = objnode.MemberBegin();
				itr != objnode.MemberEnd(); ++itr)
			{

				res = res && itr->name.IsString();
				res = res && itr->value.IsString();
			}
		}
		else
		{
			res = false;
		}
	}
	else
	{
		res = false;
		JYERROR("Unkown json member %s", name.c_str());
	}
	return res;
}
//---------------------------------------------------------------------------------------------------------
Map<String, String> JsonConfigure::GetObjectString(const String& name)
{
	Map<String, String> res;
	if (m_pDocument->HasMember(name.c_str()))
	{
		const Value& arraynode = (*m_pDocument)[name.c_str()];
		const rapidjson::Document& objnode =
			static_cast<const rapidjson::Document&>(arraynode);

		for (Value::ConstMemberIterator itr = objnode.MemberBegin();
			itr != objnode.MemberEnd(); ++itr)
		{

			res.insert({ itr->name.GetString(),itr->value.GetString() });
		}
	}
	else
	{
		JYERROR("Unkown json member %s", name.c_str());
	}
	return res;
}

NS_JYE_END
