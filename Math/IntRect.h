
#pragma once

#include "Math.h"
#include "IntVec2.h"

NS_JYE_MATH_BEGIN

class MATH_API IntRect
{
public:
	IntVec2 Min;
	IntVec2 Max;

	IntRect() {}
	IntRect(IntVec2 nmin, IntVec2 nmax) : Min(nmin), Max(nmax) {}
	IntRect(int nx, int ny, int xx, int xy) : Min(nx, ny), Max(xx, xy) {}
	IntRect(const IntRect& v) : Min(v.Min), Max(v.Max) {}

	IntRect& operator = (const IntRect& v)
	{
		Min = v.Min; Min = v.Min;
		return *this;
	}

	bool operator == (const IntRect& v) { return Min == v.Min && Max == v.Max; }
	bool operator != (const IntRect& v) { return Min != v.Min || Max != v.Max; }

	IntRect operator - () const { return IntRect(-Min, -Max); }

	int Width() const { return Max.x - Min.x; }
	int Height() const { return Max.y - Min.y; }
	int Area() const { return (Max.x - Min.x) * (Max.y - Min.y); }
	IntVec2 Size() const { return IntVec2(Max.x - Min.x, Max.y - Min.y); }

	std::string toString() const;
};

NS_JYE_MATH_END