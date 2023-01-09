#include "Box2D.h"

NS_JYE_MATH_BEGIN

FBox2D::FBox2D(const Vec2* Points, const int Count)
	: Min(0.f, 0.f)
	, Max(0.f, 0.f)
	, bIsValid(false)
{
	for (int PointItr = 0; PointItr < Count; PointItr++)
	{
		*this += Points[PointItr];
	}
}


FBox2D::FBox2D(const Vector<Vec2>& Points)
	: Min(0.f, 0.f)
	, Max(0.f, 0.f)
	, bIsValid(false)
{
	for (const Vec2& EachPoint : Points)
	{
		*this += EachPoint;
	}
}

NS_JYE_MATH_END