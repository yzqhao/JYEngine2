#include "IntRect.h"

NS_JYE_MATH_BEGIN

std::string IntRect::toString() const
{
	char tempBuffer[256];
	sprintf(tempBuffer, "%d %d %d %d", Min.x, Min.y, Max.x, Max.y);
	return std::string(tempBuffer);
}

NS_JYE_MATH_END