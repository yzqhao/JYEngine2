
#pragma once

#include "Math.h"

NS_JYE_MATH_BEGIN

class MATH_API Vec2
{
public:
	union
	{
		struct
		{
			float x, y;
		};
		float m[2];
	};

    Vec2() : x(0.0), y(0.0) {}
    Vec2(float nx, float ny) : x(nx), y(ny) {}
    Vec2(const Vec2& v) : x(v.x), y(v.y) {}

    Vec2& operator = (const Vec2& v) 
    {
        x = v.x; y = v.y;
        return *this;
    }

    bool operator == (const Vec2& v) const { return  Math::Equals(x, v.x) &&  Math::Equals(y, v.y); }
    bool operator != (const Vec2& v) const { return !Math::Equals(x, v.x) || !Math::Equals(y, v.y); }

    Vec2 operator - () const { return Vec2(-x, -y); }

	Vec2 operator + (float v) const { return Vec2(x + v, y + v); }
	Vec2 operator - (float v) const { return Vec2(x - v, y - v); }
    Vec2 operator + (const Vec2& v) const { return Vec2(x+v.x, y+v.y); }
    Vec2 operator - (const Vec2& v) const { return Vec2(x-v.x, y-v.y); }
    Vec2 operator * (float k) const { return Vec2(x*k, y*k); }
    Vec2 operator / (float k) const { return Vec2(x/k, y/k); }

    Vec2 operator += (const Vec2& v) { x+=v.x, y+=v.y; return *this; }
    Vec2 operator -= (const Vec2& v) { x-=v.x, y-=v.y; return *this; }
    Vec2 operator *= (float k) { x*=k, y*=k; return *this; }
	Vec2 operator /= (float k) { x /= k, y /= k; return *this; }

	float& operator[] (int i) { return m[i]; }
	const float& operator[] (int i) const { return m[i]; }

    bool Equals(const Vec2& rhs) const { return Math::Equals(x, rhs.x) && Math::Equals(y, rhs.y); }

	const float* GetPtr() const { return &x; }
	float* GetPtr() { return &x; }

    void Normalize()
    {
        float magSq = x*x + y*y;
        if (magSq > 0.0f) 
        {   // check for divide-by-zero
			float oneOverMag = 1.0f / sqrt(magSq);
			x *= oneOverMag;
			y *= oneOverMag;
		}
    }

    Vec2 GetNormalized() const
    {
        float magSq = x*x + y*y;
        if (magSq > 0.0f) 
        {   // check for divide-by-zero
            float oneOverMag = 1.0f / sqrt(magSq);
            return Vec2(x*oneOverMag, y*oneOverMag);
        }
        return Vec2::ZERO;
    }

    float operator * (const Vec2& v) const 
    {
		return x*v.x + y*v.y;
	}

	void set(float nx, float ny) { x = nx; y = ny; }

    std::string toString() const;

    static int Size() { return 2; }

    static const Vec2 ZERO;     // Vec2(0,0)
    static const Vec2 ONE;      // Vec2(1,1)
    static const Vec2 UNIT_X;   // Vec2(1,0)
    static const Vec2 UNIT_Y;   // Vec2(0,1)
};

inline float LengthSquared(const Vec2& v) 
{
    return (v.x * v.x + v.y * v.y);
}

inline float Length(const Vec2& v)
{
	return sqrt(v.x * v.x + v.y * v.y);
}

inline float Distance(const Vec2 &a, const Vec2 &b) {
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	return sqrt(dx*dx + dy*dy);
}

inline float DistanceSquared(const Vec2 &a, const Vec2 &b) {
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	return dx*dx + dy*dy;
}

NS_JYE_MATH_END