#include "MathUtil.h"

NS_JYE_MATH_BEGIN

Vec2 MathUtil::Lerp(const Vec2& v1, const Vec2& v2, float t)
{
	return std::move(Math::Vec2(Math::Lerp(v1.x, v2.x, t), Math::Lerp(v1.y, v2.y, t)));
}

Vec3 MathUtil::Lerp(const Vec3& v1, const Vec3& v2, float t)
{
	return std::move(Math::Vec3(Math::Lerp(v1.x, v2.x, t), Math::Lerp(v1.y, v2.y, t), Math::Lerp(v1.z, v2.z, t)));
}

Vec4 MathUtil::Lerp(const Vec4& v1, const Vec4& v2, float t)
{
	return std::move(Math::Vec4(Math::Lerp(v1.x, v2.x, t), Math::Lerp(v1.y, v2.y, t), Math::Lerp(v1.z, v2.z, t), Math::Lerp(v1.w, v2.w, t)));
}

Mat4 MathUtil::Transformation(const Vec3& scale, const Quaternion& quat, const Vec3& trans)
{
	float const x2(quat.x + quat.x);
	float const y2(quat.y + quat.y);
	float const z2(quat.z + quat.z);

	float const xx2(quat.x * x2), xy2(quat.x * y2), xz2(quat.x * z2);
	float const yy2(quat.y * y2), yz2(quat.y * z2), zz2(quat.z * z2);
	float const wx2(quat.w * x2), wy2(quat.w * y2), wz2(quat.w * z2);

	return std::move(Mat4(
		scale.x * (1 - yy2 - zz2), scale.x * (xy2 + wz2), scale.x * (xz2 - wy2), 0,
		scale.y * (xy2 - wz2), scale.y * (1 - xx2 - zz2), scale.y * (yz2 + wx2), 0,
		scale.z * (xz2 + wy2), scale.z * (yz2 - wx2),  scale.z * (1 - xx2 - yy2), 0,
		trans.x, trans.y, trans.z, 1));
}

bool MathUtil::intersects(const Ray& ray, const AABB& box, Vec3& out)
{
	// if (box.Intersect(ray.GetRayOrigin()))//如果观察点在box里面
	// {
	// 	out = ray.GetRayOrigin();
	// 	return true;
	// }
	//依次检查各面的相交情况
	float t;
	float len = 0.0f;
	bool isInster = false;
	Vec3 hitpoint;
	//前面
	if (ray.GetRayOrigin().z > box.GetMax().z && ray.GetRayDirection().z < 0)
	{
		t = (box.GetMax().z - ray.GetRayOrigin().z) / ray.GetRayDirection().z;//计算投射到该平面的距离
		if (t > 0)
		{
			hitpoint = ray.GetRayOrigin() + ray.GetRayDirection() * t;
			//如果这个hitpoint在这个平面内
			if (hitpoint.x > box.GetMin().x && hitpoint.x < box.GetMax().x
				&& hitpoint.y < box.GetMax().y && hitpoint.y > box.GetMin().y)
			{
				if (len > t || !isInster)
				{
					isInster = true;
					len = t;
					out = hitpoint;
				}
			}
		}
	}
	//左面
	if (ray.GetRayOrigin().x < box.GetMin().x && ray.GetRayDirection().x > 0)
	{
		t = (box.GetMin().x - ray.GetRayOrigin().x) / ray.GetRayDirection().x;
		if (t > 0)
		{
			hitpoint = ray.GetRayOrigin() + ray.GetRayDirection() * t;
			if (hitpoint.z > box.GetMin().z && hitpoint.z < box.GetMax().z
				&& hitpoint.y < box.GetMax().y && hitpoint.y > box.GetMin().y)
			{
				if (len > t || !isInster)
				{
					isInster = true;
					len = t;
					out = hitpoint;
				}
			}
		}
	}
	//后面
	if (ray.GetRayOrigin().z < box.GetMin().z && ray.GetRayDirection().z > 0)
	{
		t = (box.GetMin().z - ray.GetRayOrigin().z) / ray.GetRayDirection().z;
		if (t > 0)
		{
			hitpoint = ray.GetRayOrigin() + ray.GetRayDirection() * t;
			if (hitpoint.x > box.GetMin().x && hitpoint.x < box.GetMax().x
				&& hitpoint.y < box.GetMax().y && hitpoint.y > box.GetMin().y)
			{
				if (len > t || !isInster)
				{
					isInster = true;
					len = t;
					out = hitpoint;
				}
			}
		}
	}
	//右面
	if (ray.GetRayOrigin().x > box.GetMax().x && ray.GetRayDirection().x < 0)
	{
		t = (box.GetMax().x - ray.GetRayOrigin().x) / ray.GetRayDirection().x;
		if (t > 0)
		{
			hitpoint = ray.GetRayOrigin() + ray.GetRayDirection() * t;
			if (hitpoint.z > box.GetMin().z && hitpoint.z < box.GetMax().z
				&& hitpoint.y < box.GetMax().y && hitpoint.y > box.GetMin().y)
			{
				if (len > t || !isInster)
				{
					isInster = true;
					len = t;
					out = hitpoint;
				}
			}
		}
	}
	//上面
	if (ray.GetRayOrigin().y > box.GetMax().y && ray.GetRayDirection().y < 0)
	{
		t = (box.GetMax().y - ray.GetRayOrigin().y) / ray.GetRayDirection().y;
		if (t > 0)
		{
			hitpoint = ray.GetRayOrigin() + ray.GetRayDirection() * t;
			if (hitpoint.x > box.GetMin().x && hitpoint.x < box.GetMax().x
				&& hitpoint.z > box.GetMin().z && hitpoint.z < box.GetMax().z)
			{
				if (len > t || !isInster)
				{
					isInster = true;
					len = t;
					out = hitpoint;
				}
			}
		}
	}
	//下面
	if (ray.GetRayOrigin().y < box.GetMin().y && ray.GetRayDirection().y > 0)
	{
		t = (box.GetMin().y - ray.GetRayOrigin().y) / ray.GetRayDirection().y;
		if (t > 0)
		{
			hitpoint = ray.GetRayOrigin() + ray.GetRayDirection() * t;
			if (hitpoint.x > box.GetMin().x && hitpoint.x < box.GetMax().x
				&& hitpoint.z > box.GetMin().z && hitpoint.z < box.GetMax().z)
			{
				if (len > t || !isInster)
				{
					isInster = true;
					len = t;
					out = hitpoint;
				}
			}
		}
	}
	return isInster;

}

bool MathUtil::intersects(const Ray& ray, const Sphere& sphere)
{
	Vec3 l = sphere.GetCenter() - ray.GetRayOrigin();
	float s = Dot(l, ray.GetRayDirection());
	float squaredL = Dot(l, l);
	float squaredRadius = sphere.GetRadius() * sphere.GetRadius();
	if (s < 0 && squaredL > squaredRadius)
		return false;
	float squaredM = squaredL - s * s;
	if (squaredM > squaredRadius)
		return false;

	return true;
}

bool MathUtil::intersects(const AABB& box1, const AABB& box2)
{
	Vec3 cen = box1.GetCenter() - box2.GetCenter();
	Vec3 hs = box1.GetExtent() + box2.GetExtent();
	return (std::abs(cen.x) <= hs.x) && (std::abs(cen.y) <= hs.y) && (std::abs(cen.z) <= hs.z);
}

bool MathUtil::intersects(const Frustum& frustum, const AABB& box)
{
	Vec3 point;
	for (int i = 0; i < Frustum::FACE_COUNT; ++i)
	{
		Vec3 normal = frustum.GetPlane(static_cast<Frustum::PalneName>(i)).m_normal;
		point.x = normal.x < 0 ? box._max.x : box._min.x;
		point.y = normal.y < 0 ? box._max.y : box._min.y;
		point.z = normal.z < 0 ? box._max.z : box._min.z;

		if (frustum.GetPlane(static_cast<Frustum::PalneName>(i)).GetSide(point) == PointSide::FRONT_PLANE)
		{
			return true;
		}
	}

	return false;
}

bool MathUtil::intersects(const Frustum& lhs, const Frustum& rhs)
{
	bool outside = false;
	bool inside_all = true;
	for (int i = 0; i < 6; ++i)
	{
		const Plane& p = lhs.GetPlane(static_cast<typename Frustum::PalneName>(i));

		float min_p, max_p;
		min_p = max_p = p.Dot(rhs.GetCorner(Frustum::FRUSTUM_CORNER_LBN));
		for (int j = 1; j < 8; ++j)
		{
			float tmp = p.Dot(rhs.GetCorner(static_cast<typename Frustum::CornerName>(j)));
			min_p = Min(min_p, tmp);
			max_p = Max(max_p, tmp);
		}

		outside |= (max_p < 0);
		inside_all &= (min_p >= 0);
	}
	if (outside)
	{
		return false;
	}
	if (inside_all)
	{
		return true;
	}

	for (int i = 0; i < 6; ++i)
	{
		Plane const& p = rhs.GetPlane(static_cast<typename Frustum::PalneName>(i));

		float max_p = p.Dot(lhs.GetCorner(Frustum::FRUSTUM_CORNER_LBN));
		for (int j = 1; j < 8; ++j)
		{
			float tmp = p.Dot(lhs.GetCorner(static_cast<typename Frustum::CornerName>(j)));
			max_p = Max(max_p, tmp);
		}

		outside |= (max_p < 0);
	}
	if (outside)
	{
		return false;
	}

	Math::Vec3 edge_axis_l[6];
	edge_axis_l[0] = rhs.GetCorner(Frustum::FRUSTUM_CORNER_LTF);
	edge_axis_l[1] = rhs.GetCorner(Frustum::FRUSTUM_CORNER_LBF);
	edge_axis_l[2] = rhs.GetCorner(Frustum::FRUSTUM_CORNER_RBF);
	edge_axis_l[3] = rhs.GetCorner(Frustum::FRUSTUM_CORNER_RTF);
	edge_axis_l[4] = rhs.GetCorner(Frustum::FRUSTUM_CORNER_LTF) - rhs.GetCorner(Frustum::FRUSTUM_CORNER_RBF);
	edge_axis_l[5] = rhs.GetCorner(Frustum::FRUSTUM_CORNER_RTF) - rhs.GetCorner(Frustum::FRUSTUM_CORNER_RBF);

	Math::Vec3 edge_axis_r[6];
	edge_axis_r[0] = lhs.GetCorner(Frustum::FRUSTUM_CORNER_LTF);
	edge_axis_r[1] = lhs.GetCorner(Frustum::FRUSTUM_CORNER_LBF);
	edge_axis_r[2] = lhs.GetCorner(Frustum::FRUSTUM_CORNER_RBF);
	edge_axis_r[3] = lhs.GetCorner(Frustum::FRUSTUM_CORNER_RTF);
	edge_axis_r[4] = lhs.GetCorner(Frustum::FRUSTUM_CORNER_LTF) - lhs.GetCorner(Frustum::FRUSTUM_CORNER_RBF);
	edge_axis_r[5] = lhs.GetCorner(Frustum::FRUSTUM_CORNER_RTF) - lhs.GetCorner(Frustum::FRUSTUM_CORNER_RBF);

	for (int i = 0; i < 6; ++i)
	{
		for (int j = 0; j < 6; ++j)
		{
			Math::Vec3 Axis = CrossProduct(edge_axis_l[i], edge_axis_r[j]);

			float min_l, max_l, min_r, max_r;
			min_l = max_l = Axis.Dot(rhs.GetCorner(Frustum::FRUSTUM_CORNER_LBN));
			min_r = max_r = Axis.Dot(lhs.GetCorner(Frustum::FRUSTUM_CORNER_LBN));
			for (int k = 1; k < 8; ++k)
			{
				float tmp = Axis.Dot(rhs.GetCorner(static_cast<typename Frustum::CornerName>(k)));
				min_l = Min(min_l, tmp);
				max_l = Max(max_l, tmp);

				tmp = Axis.Dot(lhs.GetCorner(static_cast<typename Frustum::CornerName>(k)));
				min_r = Min(min_r, tmp);
				max_r = Max(max_r, tmp);
			}

			outside |= min_l > max_r;
			outside |= min_r > max_l;
		}
	}
	if (outside)
	{
		return false;
	}
	return true;		// 相交一部分
}

//-----------------------------------------------------------------------------
// Compute the intersection of a ray (Origin, Direction) with a triangle
// (V0, V1, V2).  Return true if there is an intersection and also set *pDist
// to the distance along the ray to the intersection.
//-----------------------------------------------------------------------------
bool MathUtil::intersects(const Ray& ray, Vec3 v0, Vec3 v1, Vec3 v2, float* distance)
{
	// Find vectors for two edges sharing vert0
	Vec3 edge1 = v1 - v0;
	Vec3 edge2 = v2 - v0;

	// Begin calculating determinant - also used to calculate U parameter
	Vec3 pvec = CrossProduct(ray.GetRayDirection(), edge2);

	// If determinant is near zero, ray lies in plane of triangle
	float det = Dot(pvec, edge1);
	Vec3 tvec;
	if (det > 0)
	{
		tvec = ray.GetRayOrigin() - v0;
	}
	else
	{
		tvec = v0 - ray.GetRayOrigin();
		det = -det;
	}

	if (det < 0.0f)
	{
		return false;
	}

	// Calculate U parameter and test bounds
	float u = Dot(pvec, tvec);
	if (u < 0.0f || u > det)
	{
		return false;
	}

	// Prepare to test V parameter
	Vec3 qvec = CrossProduct(tvec, edge1);

	// Calculate V parameter and test bounds
	float v = Dot(qvec, ray.GetRayDirection());
	if (v < 0.0f || u + v > det)
	{
		return false;
	}

	// Calculate t, scale parameters, ray intersects triangle
	float t = Dot(qvec, edge2);
	float fInvDet = 1.0f / det;
	t *= fInvDet;

	*distance = t;

	return t > 0.f;
}

NS_JYE_MATH_END
