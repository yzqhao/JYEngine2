
#pragma once

#include "Math.h"

NS_JYE_MATH_BEGIN

class MATH_API Vec1
{
public:
	union
	{
		struct
		{
			float x;
		};
		float m[1];
	};

    Vec1() : x(0.0) {}
    Vec1(float nx) : x(nx) {}
    Vec1(const Vec1& v) : x(v.x) {}

    Vec1& operator = (const Vec1& v) 
    {
        x = v.x;
        return *this;
    }

    bool operator == (const Vec1& v) const { return x == v.x; }
    bool operator != (const Vec1& v) const { return x != v.x; }

    Vec1 operator - () const { return Vec1(-x); }

    Vec1 operator + (const Vec1& v) const { return Vec1(x+v.x); }
    Vec1 operator - (const Vec1& v) const { return Vec1(x-v.x); }
    Vec1 operator * (float k) const { return Vec1(x*k); }
    Vec1 operator / (float k) const { return Vec1(x/k); }

	Vec1 operator += (const Vec1& v) { x += v.x; return *this; }
	Vec1 operator -= (const Vec1& v) { x -= v.x; return *this; }
	Vec1 operator *= (float k) { x *= k; return *this; }
	Vec1 operator /= (float k) { x /= k; return *this; }

    bool Equals(const Vec1& rhs) const { return Math::Equals(x, rhs.x); }

    const float* getPtr() const { return &x; }

    void set(float nx) { x = nx; }

    std::string toString() const;

    static int Size() { return 1; }

    static const Vec1 ZERO;     // Vec1(0)
    static const Vec1 ONE;      // Vec1(1)
};

NS_JYE_MATH_END