#include "IntVec3.h"

NS_JYE_MATH_BEGIN

const IntVec3 IntVec3::ZERO(0, 0, 0);
const IntVec3 IntVec3::ONE(1, 1, 1);
const IntVec3 IntVec3::UNIT_X(1, 0, 0);
const IntVec3 IntVec3::UNIT_Y(0, 1, 0);
const IntVec3 IntVec3::UNIT_Z(0, 0, 1);

std::string IntVec3::toString() const
{
    char tempBuffer[256];
    sprintf(tempBuffer, "%d %d %d", x, y, z);
    return std::string(tempBuffer);
}

NS_JYE_MATH_END