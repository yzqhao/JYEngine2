#pragma once

#include "Vec2.h"

NS_JYE_MATH_BEGIN

// 贝塞尔曲线三次方
class MATH_API CubicBezier
{
public:
	Vec2 _point1, _point2;
	float _aX, _bX, _cX, _aY, _bY, _cY;
	void init(float x1, float y1, float x2, float y2);		// 必须先初始化
	float getXValueFromPercent(float percent);
	float getYValueFromPercent(float percent);
};

NS_JYE_MATH_END