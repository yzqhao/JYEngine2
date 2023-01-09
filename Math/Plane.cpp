#include "Plane.h"

NS_JYE_MATH_BEGIN

void Plane::Normalize()
{
    m_d /= m_normal.LengthSquared();
    m_normal.Normalize();
}

float Plane::Dist2Plane(const Vec3& p) const
{
    return Math::Dot(m_normal, p) + m_d;
}

PointSide Plane::GetSide(const Vec3& point) const
{
    float dist = Dist2Plane(point);
    if (dist > 0)
        return PointSide::FRONT_PLANE;
    else if (dist < 0)
        return PointSide::BEHIND_PLANE;
    else
        return PointSide::IN_PLANE;
}

NS_JYE_MATH_END