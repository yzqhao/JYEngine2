
#pragma once

#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Mat4.h"
#include "Quaternion.h"
#include "Color.h"
#include "AABB.h"
#include "Ray.h"
#include "Frustum.h"
#include "Sphere.h"

NS_JYE_MATH_BEGIN

namespace MathUtil
{
	MATH_API FORCEINLINE unsigned short FloatToHalf(float Value);
	MATH_API FORCEINLINE float HalfToFloat(unsigned short Value);
	MATH_API FORCEINLINE unsigned int CompressUnitFloat(float f, unsigned int Bit = 16);	//f must [0,1]
	MATH_API FORCEINLINE unsigned int CompressFloat(float f, float Max, float Min, unsigned int Bit = 16);
	MATH_API FORCEINLINE float DecompressUnitFloat(unsigned int quantized, unsigned int Bit = 16);
	MATH_API FORCEINLINE float DecompressFloat(unsigned int quantized, float Max, float Min, unsigned int Bit = 16);
	// ��ֵVec2
	MATH_API Vec2 Lerp(const Vec2& v1, const Vec2& v2, float t);
	// ��ֵVec3
	MATH_API Vec3 Lerp(const Vec3& v1, const Vec3& v2, float t);
	// ��ֵVec4
	MATH_API Vec4 Lerp(const Vec4& v1, const Vec4& v2, float t);
	// SRT �õ�����
	MATH_API Mat4 Transformation(const Vec3& scale, const Quaternion& quat, const Vec3& trans);
	// Ray��AABB�ཻ��⣬����ཻ����distance����������㵽�������
	MATH_API bool intersects(const Ray& ray, const AABB& box, Vec3& hitpoint);
	// Ray�����ཻ��⣬����ཻ����distance����������㵽�������
	MATH_API bool intersects(const Ray& ray, const Sphere& sph);
	// AABB��AABB�ཻ��⣬����ཻ
	MATH_API bool intersects(const AABB& box1, const AABB& box2);
	// AABB��Frustum�Ƿ��ཻ
	MATH_API bool intersects(const Frustum& frustum, const AABB& box);
	// Frustum��Frustum�Ƿ��ཻ
	MATH_API bool intersects(const Frustum& f1, const Frustum& f2);
	// �������������ཻ��⣬����ཻ����Dist����������㵽�������
	MATH_API bool intersects(const Ray& ray, Vec3 v0, Vec3 v1, Vec3 v2, float* distance = nullptr);
};

#include "MathUtil.inl"

NS_JYE_MATH_END
