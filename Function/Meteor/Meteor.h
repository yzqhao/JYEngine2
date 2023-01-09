
#pragma once

#include "private/Define.h"
#include "System/Singleton.hpp"

NS_JYE_BEGIN

class MWindow;

class METEOR_API Meteor
{
	SINGLETON_DECLARE_ONCREATE_ONDESTROY(Meteor);
public:
	Meteor();
	~Meteor();

	void Update();

private:
	void _OnCreate();
	void _OnDestroy();

	bool _MakeSureInit();

	MWindow* m_wnd;
	bool m_bInit;
};

NS_JYE_END