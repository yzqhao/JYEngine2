#include "IntVec4.h"

NS_JYE_MATH_BEGIN

const IntVec4 IntVec4::ZERO(0, 0, 0, 0);
const IntVec4 IntVec4::ONE(1, 1, 1, 1);
const IntVec4 IntVec4::UNIT_X(1, 0, 0, 0);
const IntVec4 IntVec4::UNIT_Y(0, 1, 0, 0);
const IntVec4 IntVec4::UNIT_Z(0, 0, 1, 0);
const IntVec4 IntVec4::UNIT_W(0, 0, 0, 1);

std::string IntVec4::toString() const
{
    char tempBuffer[256];
    sprintf(tempBuffer, "%d %d %d %d", x, y, z, w);
    return std::string(tempBuffer);
}

NS_JYE_MATH_END