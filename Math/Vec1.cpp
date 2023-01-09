#include "Vec1.h"

NS_JYE_MATH_BEGIN

const Vec1 Vec1::ZERO(0.0f);
const Vec1 Vec1::ONE(1.0f);

std::string Vec1::toString() const
{
    char tempBuffer[256];
    sprintf(tempBuffer, "%g", x);
    return std::string(tempBuffer);
}

NS_JYE_MATH_END