
#pragma once

#include "Math.h"

NS_JYE_MATH_BEGIN

class MATH_API IntVec3
{
public:
    int x, y, z;

    IntVec3() : x(0), y(0), z(0) {}
    IntVec3(int nx, int ny, int nz) : x(nx), y(ny), z(nz) {}
    IntVec3(const IntVec3& v) : x(v.x), y(v.y), z(v.z) {}

    IntVec3& operator = (const IntVec3& v) 
    {
        x = v.x; y = v.y, z = v.z;
        return *this;
    }

    bool operator == (const IntVec3& v) { return x == v.x && y == v.y && z == v.z; }
    bool operator != (const IntVec3& v) { return x != v.x || y != v.y || z != v.z; }

    IntVec3 operator - () const { return IntVec3(-x, -y, -z); }

    IntVec3 operator + (const IntVec3& v) const { return IntVec3(x+v.x, y+v.y, z+v.z); }
    IntVec3 operator - (const IntVec3& v) const { return IntVec3(x-v.x, y-v.y, z-v.z); }
    IntVec3 operator * (int k) const { return IntVec3(x*k, y*k, z * k); }
    IntVec3 operator / (int k) const { return IntVec3(x/k, y/k, z / k); }

    IntVec3 operator += (const IntVec3& v) { x+=v.x, y+=v.y, z+=v.z; return *this; }
    IntVec3 operator -= (const IntVec3& v) { x-=v.x, y-=v.y, z-=v.z; return *this; }
    IntVec3 operator *= (int k) { x*=k, y*=k, z*=k; return *this; }
    IntVec3 operator /= (int k) { x/=k, y/=k, z/=k; return *this; }

	std::string toString() const;

	const int* GetPtr() const { return &x; }
	int* GetPtr() { return &x; }

    static const IntVec3 ZERO;     // IntVec3(0,0,0)
    static const IntVec3 ONE;      // IntVec3(1,1,1)
    static const IntVec3 UNIT_X;   // IntVec3(1,0,0)
	static const IntVec3 UNIT_Y;   // IntVec3(0,1,0)
	static const IntVec3 UNIT_Z;   // IntVec3(0,0,1)
};

NS_JYE_MATH_END