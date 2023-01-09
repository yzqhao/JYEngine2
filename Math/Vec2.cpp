#include "Vec2.h"

NS_JYE_MATH_BEGIN

const Vec2 Vec2::ZERO(0.0f, 0.0f);
const Vec2 Vec2::ONE(1.0f, 1.0f);
const Vec2 Vec2::UNIT_X(1.0f, 0.0f);
const Vec2 Vec2::UNIT_Y(0.0f, 1.0f);

std::string Vec2::toString() const
{
    char tempBuffer[256];
    sprintf(tempBuffer, "%g %g", x, y);
    return std::string(tempBuffer);
}

NS_JYE_MATH_END