#include "Vec3.h"

NS_JYE_MATH_BEGIN

const Vec3 Vec3::ZERO(0.0f, 0.0f, 0.0f);
const Vec3 Vec3::ONE(1.0f, 1.0f, 1.0f);
const Vec3 Vec3::UNIT_X(1.0f, 0.0f, 0.0f);
const Vec3 Vec3::UNIT_Y(0.0f, 1.0f, 0.0f);
const Vec3 Vec3::UNIT_Z(0.0f, 0.0f, 1.0f);

std::string Vec3::toString() const
{
    char tempBuffer[256];
    sprintf(tempBuffer, "%g %g %g", x, y, z);
    return std::string(tempBuffer);
}

NS_JYE_MATH_END