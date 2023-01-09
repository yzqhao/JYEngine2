
#include "RttiManager.h"

NS_JYE_BEGIN

RttiManager::RttiManager()
{

}

RttiManager::~RttiManager()
{

}

RttiManager* RttiManager::Instance()
{
	static RttiManager instance;
	return &instance;
}

void RttiManager::RegisterClass(const String& name, Rtti* rtti)
{
	m_mapRtti.insert({name, rtti});
}

const Rtti* RttiManager::GetRttiByName(const String& RttiName)
{
	auto i = m_mapRtti.find(RttiName);
	if (i != m_mapRtti.end())
	{
		return i->second;
	}
	return nullptr;
}

const RttiInfoIn* RttiManager::GetRttInfoByName(const String& RttiName)
{
	auto i = m_mapRttiInfo.find(RttiName);
	if (i != m_mapRttiInfo.end())
	{
		return i->second;
	}
	return nullptr;
}

void RttiManager::GetChildrenName(const String& name, Vector<String>& outChildrenNames)
{
	const RttiInfoIn* info = GetRttInfoByName(name);
	JY_ASSERT(info);
	outChildrenNames.resize(info->children.size());
	for (int i = 0; i < outChildrenNames.size(); ++i)
	{
		outChildrenNames[i] = info->children[i]->cur->GetName();
	}
}

void RttiManager::Initlize()
{
	for (auto iter = m_mapRtti.begin(); iter != m_mapRtti.end(); ++iter)
	{
		Rtti* rtti = iter->second;
		Rtti* base = rtti->GetBase();
		m_mapRttiInfo.insert({ iter->first, _NEW RttiInfoIn(rtti, base) });
	}
	for (auto iter = m_mapRtti.begin(); iter != m_mapRtti.end(); ++iter)
	{
		Rtti* rtti = iter->second;
		Rtti* base = rtti->GetBase();
		if (base)
		{
			m_mapRttiInfo[base->GetName()]->children.push_back(m_mapRttiInfo[rtti->GetName()]);
		}
	}
}

void RttiManager::Destory()
{
	for (auto& it : m_mapRttiInfo)
	{
		SAFE_DELETE(it.second);
	}
	m_mapRttiInfo.clear();
}

NS_JYE_END
