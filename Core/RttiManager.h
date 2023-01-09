
#pragma once

#include "Rtti.h"

NS_JYE_BEGIN


class CORE_API RttiManager
{
public:
	RttiManager();
	~RttiManager();
	static RttiManager* Instance();

	void RegisterClass(const String& name, Rtti* rtti);
	const Rtti* GetRttiByName(const String& RttiName);
	const RttiInfoIn* GetRttInfoByName(const String& RttiName);
	void GetChildrenName(const String& name, Vector<String>& outChildrenNames);

	void Initlize();
	void Destory();

private:
	HashMap<String, RttiInfoIn*>	m_mapRttiInfo;
	HashMap<String, Rtti*> m_mapRtti;
};

NS_JYE_END
