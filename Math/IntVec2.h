
#pragma once

#include "Math.h"

NS_JYE_MATH_BEGIN

class MATH_API IntVec2
{
public:
    int x, y;

    IntVec2() : x(0), y(0) {}
    IntVec2(int nx, int ny) : x(nx), y(ny) {}
    IntVec2(const IntVec2& v) : x(v.x), y(v.y) {}

    IntVec2& operator = (const IntVec2& v) 
    {
        x = v.x; y = v.y;
        return *this;
    }

    bool operator == (const IntVec2& v) const { return x == v.x && y == v.y; }
    bool operator != (const IntVec2& v) const { return x != v.x || y != v.y; }

    IntVec2 operator - () const { return IntVec2(-x, -y); }

    IntVec2 operator + (const IntVec2& v) const { return IntVec2(x+v.x, y+v.y); }
    IntVec2 operator - (const IntVec2& v) const { return IntVec2(x-v.x, y-v.y); }
    IntVec2 operator * (int k) const { return IntVec2(x*k, y*k); }
    IntVec2 operator / (int k) const { return IntVec2(x/k, y/k); }

	IntVec2 operator * (const IntVec2& v) const { return IntVec2(x * v.x, y * v.y); }
	IntVec2 operator / (const IntVec2& v) const { return IntVec2(x / v.x, y / v.y); }

	std::string toString() const;

	const int* GetPtr() const { return &x; }
	int* GetPtr() { return &x; }

    static const IntVec2 ZERO;     // IntVec2(0,0)
    static const IntVec2 ONE;      // IntVec2(1,1)
    static const IntVec2 UNIT_X;   // IntVec2(1,0)
    static const IntVec2 UNIT_Y;   // IntVec2(0,1)
};

NS_JYE_MATH_END