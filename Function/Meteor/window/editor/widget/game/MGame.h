
#pragma once

#include "private/Define.h"
#include "System/Delegate.h"
#include "../MMemuWindowBase.h"
#include "Math/3DMath.h"

NS_JYE_BEGIN

class MGame : public MMemuWindowBase
{
public:
	MGame();
	~MGame();

	virtual bool OnGui(float dt);

	void Menu();
private:
	float m_ratio;
	Math::IntVec2 m_windowSize;
};

NS_JYE_END