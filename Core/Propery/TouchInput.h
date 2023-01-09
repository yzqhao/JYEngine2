#pragma once

#include "../Core.h"

NS_JYE_BEGIN

struct CORE_API TouchInput
{
public:
	enum TouchType
	{
		TT_PRESS = 1,
		TT_MOVE,
		TT_RELEASE,
		TT_TOUCH_SCALE,
		TT_TOUCH_ROTATE,
	};
public:
	uint		button;
	uint		id;
	TouchType	type;
	float		x;
	float		y;
public:
	TouchInput(uint i, uint b, TouchType t, float fx, float fy) :id(i), button(b), type(t), x(fx), y(fy) {};
	virtual ~TouchInput(void) {};
};

typedef Vector<TouchInput>	TouchInputs;

NS_JYE_END