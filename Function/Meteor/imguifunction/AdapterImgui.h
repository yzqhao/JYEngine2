
#pragma once

#include "private/Define.h"
#include "System/Singleton.hpp"

NS_JYE_BEGIN

class AdapterWindow;

class AdapterImgui
{
	SINGLETON_DECLARE_ONCREATE_ONDESTROY(AdapterImgui);
private:
	double				m_CumulateTime;
	AdapterWindow* m_pAdapterWindow;
public:
	AdapterImgui();
	~AdapterImgui();
private:
	void _OnCreate();
	void _OnDestroy();
private:
	double _GetTimespan();
public:
	void EarlyUpdate();
	void LateUpdate();
};

NS_JYE_END