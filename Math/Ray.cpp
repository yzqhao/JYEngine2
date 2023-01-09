#include "Ray.h"
#include "MathUtil.h"

NS_JYE_MATH_BEGIN

bool Ray::Intersect(const Math::AABB& box) const 
{ 
	Vec3 out;
	return MathUtil::intersects(*this, box, out);
}

NS_JYE_MATH_END