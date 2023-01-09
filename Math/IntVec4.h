
#pragma once

#include "Math.h"

NS_JYE_MATH_BEGIN

class MATH_API IntVec4
{
public:
	union
	{
		struct
		{
			int x, y, z, w;
		};
        int m[4];
	};

    IntVec4() : x(0), y(0), z(0), w(0) {}
    IntVec4(int nx, int ny, int nz, int nw) : x(nx), y(ny), z(nz), w(nw) {}
    IntVec4(const IntVec4& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}

    IntVec4& operator = (const IntVec4& v) 
    {
        x = v.x; y = v.y; z = v.z; w = v.w;
        return *this;
    }

    bool operator == (const IntVec4& v) const { return x == v.x && y == v.y && z == v.z && w == v.w; }
    bool operator != (const IntVec4& v) const { return x != v.x || y != v.y || z != v.z || w != v.w; }

    IntVec4 operator - () const { return IntVec4(-x, -y, -z, -w); }

    IntVec4 operator + (const IntVec4& v) const { return IntVec4(x+v.x, y+v.y, z+v.z, w+v.w); }
    IntVec4 operator - (const IntVec4& v) const { return IntVec4(x-v.x, y-v.y, z-v.z, w-v.w); }
    IntVec4 operator * (int k) const { return IntVec4(x*k, y*k, z * k, w * k); }
    IntVec4 operator / (int k) const { return IntVec4(x/k, y/k, z / k, w / k); }

    IntVec4 operator * (const IntVec4& v) const { return IntVec4(x * v.x, y * v.y, z * v.z, w * v.w); }
    IntVec4 operator / (const IntVec4& v) const { return IntVec4(x / v.x, y / v.y, z / v.z, w * v.w); }

    IntVec4 operator += (const IntVec4& v) { x+=v.x, y+=v.y, z+=v.z, w+=v.w; return *this; }
    IntVec4 operator -= (const IntVec4& v) { x-=v.x, y-=v.y, z-=v.z, w-=v.w; return *this; }
    IntVec4 operator *= (int k) { x*=k, y*=k, z*=k, w*=k; return *this; }
	IntVec4 operator /= (int k) { x /= k, y /= k, z /= k, w /= k; return *this; }

	int& operator[] (int i) { return m[i]; }
	const int& operator[] (int i) const { return m[i]; }

	std::string toString() const;

	const int* GetPtr() const { return &x; }
	int* GetPtr() { return &x; }

	static int Size() { return 4; }

    static const IntVec4 ZERO;     // IntVec4(0,0,0,0)
    static const IntVec4 ONE;      // IntVec4(1,1,1,1)
    static const IntVec4 UNIT_X;   // IntVec4(1,0,0,0)
	static const IntVec4 UNIT_Y;   // IntVec4(0,1,0,0)
	static const IntVec4 UNIT_Z;   // IntVec4(0,0,1,0)
	static const IntVec4 UNIT_W;   // IntVec4(0,0,0,1)
};

NS_JYE_MATH_END