#pragma once

#include "Vec3.h"

NS_JYE_MATH_BEGIN

class AABB;

class MATH_API Sphere
{
public:
	float	_radius;
	Vec3	_center;
public:
	Sphere() { _radius = 0.0f; }
	Sphere(const Vec3& p0, float r) { set(p0, r); }
	void set(const Vec3& p0) { _center = p0;	_radius = 0; }
	void set(const Vec3& p0, float r) { _center = p0;	_radius = r; }
	void set(const Vec3& p0, const Vec3& p1);
	void set(const Vec3* inVertices, int inHowmany);	// 相对绝对坐标系计算
	void set(const AABB& aabb);		// 包围AABB最小球
	Vec3& GetCenter() { return _center; }
	const Vec3& GetCenter()const { return _center; }
	float& GetRadius() { return _radius; }
	const float& GetRadius()const { return _radius; }
	bool IsInside(const Sphere& inSphere)const;
};

float CalculateSqrDistance(const Vec3& p, const Sphere& s);
bool Intersect(const Sphere& inSphere0, const Sphere& inSphere1);

inline void Sphere::set(const Vec3& p0, const Vec3& p1){
	Vec3 dhalf = (p1 - p0) * 0.5;
	_center = dhalf + p0;
	_radius = dhalf.Length();
}
inline bool Sphere::IsInside(const Sphere& inSphere)const{
	float sqrDist = (GetCenter() - inSphere.GetCenter()).LengthSquared();
	if (SQUARE(GetRadius()) > sqrDist + SQUARE(inSphere.GetRadius()))
		return true;
	else
		return false;
}
inline bool Intersect(const Sphere& inSphere0, const Sphere& inSphere1){
	float sqrDist = (inSphere0.GetCenter() - inSphere1.GetCenter()).LengthSquared();
	if (SQUARE(inSphere0.GetRadius() + inSphere1.GetRadius()) > sqrDist)
		return true;
	else
		return false;
}
inline float CalculateSqrDistance(const Vec3& p, const Sphere& s){
	return Max(0.0f, (p - s.GetCenter()).LengthSquared() - SQUARE(s.GetRadius()));
}

NS_JYE_MATH_END