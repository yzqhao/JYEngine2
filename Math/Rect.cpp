
#include "Rect.h"

NS_JYE_MATH_BEGIN

const Rect Rect::FULL(-1.0f, -1.0f, 1.0f, 1.0f);
const Rect Rect::POSITIVE(0.0f, 0.0f, 1.0f, 1.0f);
const Rect Rect::ZERO(0.0f, 0.0f, 0.0f, 0.0f);

std::string Rect::toString() const
{
    char tempBuffer[256];
    sprintf(tempBuffer, "%g %g %g %g", min_.x, min_.y, max_.x, max_.y);
    return std::string(tempBuffer);
}

void Rect::Clip(const Rect& rect)
{
    if (rect.min_.x > min_.x)
        min_.x = rect.min_.x;
    if (rect.max_.x < max_.x)
        max_.x = rect.max_.x;
    if (rect.min_.y > min_.y)
        min_.y = rect.min_.y;
    if (rect.max_.y < max_.y)
        max_.y = rect.max_.y;

    if (min_.x > max_.x || min_.y > max_.y)
    {
        min_ = Vec2(M_INFINITY, M_INFINITY);
        max_ = Vec2(-M_INFINITY, -M_INFINITY);
    }
}

NS_JYE_MATH_END
