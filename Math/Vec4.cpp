#include "Vec4.h"

NS_JYE_MATH_BEGIN

const Vec4 Vec4::ZERO(0.0f, 0.0f, 0.0f, 0.0f);
const Vec4 Vec4::ONE(1.0f, 1.0f, 1.0f, 1.0f);

std::string Vec4::toString() const
{
    char tempBuffer[256];
    sprintf(tempBuffer, "%g %g %g %g", x, y, z, w);
    return std::string(tempBuffer);
}

NS_JYE_MATH_END