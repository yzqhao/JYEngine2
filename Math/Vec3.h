
#pragma once

#include "Math.h"

NS_JYE_MATH_BEGIN

class MATH_API Vec3
{
public:
	union
	{
		struct
		{
			float x, y, z;
		};
		float m[3];
	};

    Vec3() : x(0.0), y(0.0), z(0.0) {}
    Vec3(float nv) : x(nv), y(nv), z(nv) {}
    Vec3(float nx, float ny, float nz) : x(nx), y(ny), z(nz) {}
    Vec3(const Vec3& v) : x(v.x), y(v.y), z(v.z) {}

    Vec3& operator = (const Vec3& v) 
    {
        x = v.x; y = v.y; z = v.z;
        return *this;
    }

    bool operator == (const Vec3& v) const { return  Math::Equals(x, v.x) &&  Math::Equals(y, v.y) &&  Math::Equals(z, v.z); }
    bool operator != (const Vec3& v) const { return !Math::Equals(x, v.x) || !Math::Equals(y, v.y) || !Math::Equals(z, v.z); }

    Vec3 operator - () const { return Vec3(-x, -y, -z); }

    Vec3 operator + (const Vec3& v) const { return Vec3(x+v.x, y+v.y, z+v.z); }
    Vec3 operator - (const Vec3& v) const { return Vec3(x-v.x, y-v.y, z-v.z); }
    Vec3 operator * (const Vec3& v) const { return Vec3(x * v.x, y * v.y, z * v.z); }
	Vec3 operator / (const Vec3& v) const { return Vec3(x / v.x, y / v.y, z / v.z); }
	Vec3 operator * (float k) const { return Vec3(x * k, y * k, z * k); }
	Vec3 operator / (float k) const { return Vec3(x / k, y / k, z / k); }

    Vec3 operator += (const Vec3& v) { x+=v.x, y+=v.y, z+=v.z; return *this; }
    Vec3 operator -= (const Vec3& v) { x-=v.x, y-=v.y, z-=v.z; return *this; }
    Vec3 operator *= (float k) { x*=k, y*=k, z*=k; return *this; }
    Vec3 operator /= (float k) { x/=k, y/=k, z/=k; return *this; }

	bool operator < (const Vec3& rhs) const { return (x < rhs.x && y < rhs.y && z < rhs.z); }
	bool operator > (const Vec3& rhs) const { return (x > rhs.x && y > rhs.y && z > rhs.z); }

	float& operator[] (int i) { return m[i]; }
	const float& operator[] (int i) const { return m[i]; }

    bool Equals(const Vec3& rhs) const { return Math::Equals(x, rhs.x) && Math::Equals(y, rhs.y) && Math::Equals(z, rhs.z); }

    void Set(float nx, float ny, float nz) { x = nx, y = ny, z = nz; }

	const float* GetPtr() const { return &x; }
    float* GetPtr() { return &x; }

    void Normalize()
    {
        float magSq = x*x + y*y + z*z;
        if (magSq > 0.0f) 
        {   // check for divide-by-zero
			float oneOverMag = 1.0f / sqrt(magSq);
			x *= oneOverMag;
			y *= oneOverMag;
			z *= oneOverMag;
		}
    }

    Vec3 GetNormalized() const
    {
        float magSq = x*x + y*y + z*z;
        if (magSq > 0.0f) 
        {   // check for divide-by-zero
            float oneOverMag = 1.0f / sqrt(magSq);
            return Vec3(x*oneOverMag, y*oneOverMag, z*oneOverMag);
        }
        return Vec3::ZERO;
    }

	float LengthSquared() const { return (x * x + y * y + z * z); }
	float Length() const { return sqrt(x * x + y * y + z * z); }
    float Dot(const Vec3& rv) const { return x* rv.x + y * rv.y + z * rv.z; }
    Vec3 Cross(const Vec3& rv) const { return Vec3(y * rv.z - z * rv.y, z * rv.x - x * rv.z, x * rv.y - y * rv.x); }

	std::string toString() const;

	static int Size() { return 3; }

    static const Vec3 ZERO;     // Vec3(0,0,0)
    static const Vec3 ONE;      // Vec3(1,1,1)
    static const Vec3 UNIT_X;   // Vec3(1,0,0)
    static const Vec3 UNIT_Y;   // Vec3(0,1,0)
    static const Vec3 UNIT_Z;   // Vec3(0,0,1)
};

inline float Dot(const Vec3& v1, const Vec3& v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline Vec3 CrossProduct(const Vec3 &a, const Vec3 &b) {
	return Vec3(
		a.y*b.z - a.z*b.y,
		a.z*b.x - a.x*b.z,
		a.x*b.y - a.y*b.x
	);
}

inline float Distance(const Vec3 &a, const Vec3 &b) {
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	float dz = a.z - b.z;
	return sqrt(dx*dx + dy*dy + dz*dz);
}

inline float DistanceSquared(const Vec3 &a, const Vec3 &b) {
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	float dz = a.z - b.z;
	return dx*dx + dy*dy + dz*dz;
}

NS_JYE_MATH_END