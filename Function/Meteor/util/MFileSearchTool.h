
#pragma once

#include "private/Define.h"
#include "System/Delegate.h"
#include "System/Singleton.hpp"

NS_JYE_BEGIN

class MMemuWindowBase;

class MFileSearchTool
{
	SINGLETON_DECLARE_ONCREATE_ONDESTROY(MFileSearchTool);
public:
	MFileSearchTool();
	~MFileSearchTool();

	void _OnCreate();
	void _OnDestroy();

	void GetFileTree(MMemuWindowBase* window, StaticDelegate<void()>& event);

private:

};

NS_JYE_END