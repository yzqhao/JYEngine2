
#pragma once

#include "Vec3.h"
#include "Quaternion.h"
#include "Mat4.h"
#include "MathUtil.h"

NS_JYE_MATH_BEGIN

class MATH_API Transform
{
public:
	Vec3    m_position{ Vec3::ZERO };
	Vec3 m_scale{ Vec3::ONE };
	Quaternion m_rotation{ Quaternion::IDENTITY };

	Transform() = default;
	Transform(const Vec3& position, const Quaternion & rotation, const Vec3& scale) : m_position{ position }, m_scale{ scale }, m_rotation{ rotation } {}

	Mat4 GetMatrix() const
	{
		Mat4 temp;
		MathUtil::Transformation(m_scale, m_rotation, m_position);
		return temp;
	}
};

NS_JYE_MATH_END