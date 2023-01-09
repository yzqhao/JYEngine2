
#include "Sphere.h"
#include "AABB.h"

NS_JYE_MATH_BEGIN

void Sphere::set(const Vec3* inVertices, int inHowmany)
{
	_radius = 0.0f;
	_center = Vec3(0, 0, 0);
	for (int i = 0; i < inHowmany; i++)
		_radius = std::max<float>(_radius, inVertices[i].Length());
	_radius = sqrt(_radius);
}

void Sphere::set(const AABB& aabb) 
{
	_center = Vec3((aabb._max.x + aabb._min.x) / 2.0f, (aabb._max.y + aabb._min.y) / 2.0f, (aabb._max.z + aabb._min.z) / 2.0f);
	float deltaX = aabb._max.x - _center.x;
	float deltaY = aabb._max.y - _center.y;
	float deltaZ = aabb._max.z - _center.z;
	_radius = sqrtf(SQUARE(deltaX) + SQUARE(deltaY) + SQUARE(deltaZ));
}

NS_JYE_MATH_END