
#pragma once

#include "private/Define.h"
#include "MWindowBase.h"

NS_JYE_BEGIN

class MWindow
{
public:
	MWindow();
	~MWindow();

	void Update(float dt);

private:

	Vector<MWindowBase*> m_windows;
	MWindowIndex m_idx;
};

NS_JYE_END