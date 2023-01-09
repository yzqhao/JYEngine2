#include "IntVec2.h"

NS_JYE_MATH_BEGIN

const IntVec2 IntVec2::ZERO(0, 0);
const IntVec2 IntVec2::ONE(1, 1);
const IntVec2 IntVec2::UNIT_X(1, 0);
const IntVec2 IntVec2::UNIT_Y(0, 1);

std::string IntVec2::toString() const
{
    char tempBuffer[256];
    sprintf(tempBuffer, "%d %d", x, y);
    return std::string(tempBuffer);
}

NS_JYE_MATH_END