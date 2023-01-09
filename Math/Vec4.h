
#pragma once

#include "Vec3.h"

NS_JYE_MATH_BEGIN

class MATH_API Vec4
{
public:
	union
	{
		struct
		{
			float x, y, z, w;
		};
        struct
        {
            float r, g, b, a;
        };
		float m[4];
	};

    Vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
    Vec4(float nx, float ny, float nz, float nw) : x(nx), y(ny), z(nz), w(nw) {}
	Vec4(const Vec3& v) : x(v.x), y(v.y), z(v.z), w(0.0f) {}
	Vec4(const Vec3& v, float nw) : x(v.x), y(v.y), z(v.z), w(nw) {}
    Vec4(const Vec4& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}

    Vec4& operator = (const Vec4& v) 
    {
        x = v.x; y = v.y; z = v.z; w = v.w;
        return *this;
    }

    bool operator == (const Vec4& v) const { return  Math::Equals(x, v.x) && Math::Equals(y, v.y) &&  Math::Equals(z, v.z) &&  Math::Equals(w, v.w); }
    bool operator != (const Vec4& v) const { return !Math::Equals(x, v.x) || !Math::Equals(y, v.y) || !Math::Equals(z, v.z) || !Math::Equals(w, v.w); }

    Vec4 operator - () const { return Vec4(-x, -y, -z, -w); }

    Vec4 operator + (const Vec4& v) const { return Vec4(x+v.x, y+v.y, z+v.z, w+v.w); }
	Vec4 operator - (const Vec4& v) const { return Vec4(x - v.x, y - v.y, z - v.z, w - v.w); }
	Vec4 operator * (float k) const { return Vec4(x * k, y * k, z * k, w * k); }
	Vec4 operator / (float k) const { return Vec4(x / k, y / k, z / k, w * k); }

    Vec4 operator += (const Vec4& v) { x+=v.x, y+=v.y, z+=v.z, w+=v.w; return *this; }
    Vec4 operator -= (const Vec4& v) { x-=v.x, y-=v.y, z-=v.z, w-=v.w; return *this; }

    Vec4 operator *= (float s) { x*=s, y*=s, z*=s, w*=s; return *this; }
    Vec4 operator /= (float s) { float ss = 1.0f / s;  x *= ss, y *= ss, z *= ss, w *= ss; return *this; }

	float& operator[] (int i) { return m[i]; }
	const float& operator[] (int i) const { return m[i]; }

	bool Equals(const Vec4& rhs) const { return Math::Equals(x, rhs.x) && Math::Equals(y, rhs.y) && Math::Equals(z, rhs.z) && Math::Equals(w, rhs.w); }

	void Set(float nx, float ny, float nz, float nw = 1.0f) { x = nx, y = ny, z = nz, w = nw; }

	const float* GetPtr() const { return &x; }
	float* GetPtr() { return &x; }

	std::string toString() const;

	static int Size() { return 4; }

    static const Vec4 ZERO;     // Vec4(0,0,0,0)
    static const Vec4 ONE;      // Vec4(1,1,1,1)
};

NS_JYE_MATH_END
