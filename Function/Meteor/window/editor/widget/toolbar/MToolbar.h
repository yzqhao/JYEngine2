
#pragma once

#include "private/Define.h"
#include "System/Delegate.h"
#include "../MMemuWindowBase.h"

NS_JYE_BEGIN

class MToolbar : public MMemuWindowBase
{
public:
	MToolbar();
	~MToolbar();

	virtual bool OnGui(float dt);

	void DrawToolbar();

private:
};

NS_JYE_END