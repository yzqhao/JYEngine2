#include "CubicBezier.h"

NS_JYE_MATH_BEGIN

void CubicBezier::init(float x1, float y1, float x2, float y2)
{
	_point1.set(x1, y1);
	_point2.set(x2, y2);

	_aX = 3 * x1 - 3 * x2 + 1;
	_bX = 3 * x2 - 6 * x1;
	_cX	= 3 * x1;

	_aY = 3 * y1 - 3 * y2 + 1;
	_bY = 3 * y2 - 6 * y1;
	_cY = 3 * y1;
}

float CubicBezier::getXValueFromPercent(float percent)
{
	return ((_aX * percent + _bX) * percent + _cX) * percent;
}

float CubicBezier::getYValueFromPercent(float percent)
{
	return ((_aY * percent + _bY) * percent + _cY) * percent;
}

NS_JYE_MATH_END