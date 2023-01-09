
#pragma once

#include "vec3.h"

NS_JYE_MATH_BEGIN

class MATH_API AABB
{
public:
    Vec3 _min;
    Vec3 _max;
public:
    AABB() { Reset(); }
    AABB(const Vec3& min, const Vec3& max) : _min(min), _max(max) {}

    void Reset();
    bool IsEmpty() const;
    Vec3 GetCenter() const { return (_max + _min) * 0.5f; }
	Vec3 GetExtent() const { return (_max - _min) * 0.5f; }
	const Vec3& GetMin() const { return _min; }
	const Vec3& GetMax() const { return _max; }

    void Merge(const Vec3& point)
    {
        if (point.x < _min.x) _min.x = point.x;
        if (point.y < _min.y) _min.y = point.y;
        if (point.z < _min.z) _min.z = point.z;
        if (point.x > _max.x) _max.x = point.x;
        if (point.y > _max.y) _max.y = point.y;
        if (point.z > _max.z) _max.z = point.z;
    }

    void Merge(const AABB& box)
    {
        if (box._min.x < _min.x) _min.x = box._min.x;
        if (box._min.y < _min.y) _min.y = box._min.y;
        if (box._min.z < _min.z) _min.z = box._min.z;
        if (box._max.x > _max.x) _max.x = box._max.x;
        if (box._max.y > _max.y) _max.y = box._max.y;
        if (box._max.z > _max.z) _max.z = box._max.z;
	}
	
    float GetDiagonalLength() const
	{
		return (GetMax() - GetMin()).Length();
	}

	void UpdateMinMax(const Vec3* point, std::uint32_t num);

    bool IsInside(const Vec3& point) const
    {
        if (point.x < _min.x || point.x > _max.x || point.y < _min.y || point.y > _max.y ||
            point.z < _min.z || point.z > _max.z)
            return false;
        else
            return true;
    }

    std::string toString() const;
};

NS_JYE_MATH_END
