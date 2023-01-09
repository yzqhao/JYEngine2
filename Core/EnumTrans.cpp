
#include "EnumTrans.h"

NS_JYE_BEGIN

EnumTrans& EnumTrans::GetInstance()
{
	static EnumTrans EnumTrans;
	return EnumTrans;
}

EnumTrans::EnumTrans()
{

}

EnumTrans::~EnumTrans()
{

}

void EnumTrans::Add(const char* EnumName, const char* Value)
{
	auto i = m_EnumSaver.find(EnumName);
	if (i != m_EnumSaver.end())
	{
		i->second.push_back(Value);
		return;
	}
	m_EnumSaver.insert({ EnumName, {Value} });
}

void EnumTrans::Get(const char* EnumName, Vector<String>& AS)
{
	auto i = m_EnumSaver.find(EnumName);
	if (i != m_EnumSaver.end())
	{
		AS = i->second;
	}
}

NS_JYE_END
