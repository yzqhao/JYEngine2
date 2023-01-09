
#pragma once

#include "Vec2.h"
#include "Vec4.h"
#include "IntVec2.h"

NS_JYE_MATH_BEGIN

/// Two-dimensional bounding rectangle.
class MATH_API Rect
{
public:
    /// Construct an undefined rect.
    Rect() noexcept :
        min_(M_INFINITY, M_INFINITY),
        max_(-M_INFINITY, -M_INFINITY)
    {
    }

    /// Construct from minimum and maximum vectors.
    Rect(const Vec2& min, const Vec2& max) noexcept :
        min_(min),
        max_(max)
    {
    }

    /// Construct from coordinates.
    Rect(float left, float top, float right, float bottom) noexcept :
        min_(left, top),
        max_(right, bottom)
    {
    }

    /// Construct from a Vec4.
    explicit Rect(const Vec4& vector) noexcept :
        min_(vector.x, vector.y),
        max_(vector.z, vector.w)
    {
    }

    /// Construct from a float array.
    explicit Rect(const float* data) noexcept :
        min_(data[0], data[1]),
        max_(data[2], data[3])
    {
    }

    /// Test for equality with another rect.
    bool operator ==(const Rect& rhs) const { return min_ == rhs.min_ && max_ == rhs.max_; }

    /// Test for inequality with another rect.
    bool operator !=(const Rect& rhs) const { return min_ != rhs.min_ || max_ != rhs.max_; }

    /// Divide by scalar inplace.
    Rect& operator /=(float value)
    {
        min_ /= value;
        max_ /= value;
        return *this;
    }

    /// Multiply by scalar inplace.
    Rect& operator *=(float value)
    {
        min_ *= value;
        max_ *= value;
        return *this;
    }

    /// Divide by scalar.
    Rect operator /(float value) const
    {
        return Rect(min_ / value, max_ / value);
    }

    /// Multiply by scalar.
    Rect operator *(float value) const
    {
        return Rect(min_ * value, max_ * value);
    }

    /// Define from another rect.
    void Define(const Rect& rect)
    {
        min_ = rect.min_;
        max_ = rect.max_;
    }

    /// Define from minimum and maximum vectors.
    void Define(const Vec2& min, const Vec2& max)
    {
        min_ = min;
        max_ = max;
    }

    /// Define from a point.
    void Define(const Vec2& point)
    {
        min_ = max_ = point;
    }

    /// Merge a point.
    void Merge(const Vec2& point)
    {
        if (point.x < min_.x)
            min_.x = point.x;
        if (point.x > max_.x)
            max_.x = point.x;
        if (point.y < min_.y)
            min_.y = point.y;
        if (point.y > max_.y)
            max_.y = point.y;
    }

    /// Merge a rect.
    void Merge(const Rect& rect)
    {
        if (rect.min_.x < min_.x)
            min_.x = rect.min_.x;
        if (rect.min_.y < min_.y)
            min_.y = rect.min_.y;
        if (rect.max_.x > max_.x)
            max_.x = rect.max_.x;
        if (rect.max_.y > max_.y)
            max_.y = rect.max_.y;
    }

    /// Clear to undefined state.
    void Clear()
    {
        min_ = Vec2(M_INFINITY, M_INFINITY);
        max_ = Vec2(-M_INFINITY, -M_INFINITY);
    }

    /// Clip with another rect.
    void Clip(const Rect& rect);

    /// Return true if this rect is defined via a previous call to Define() or Merge().
    bool Defined() const
    {
        return min_.x != M_INFINITY;
    }

    /// Return center.
    Vec2 Center() const { return (max_ + min_) * 0.5f; }

    /// Return size.
    Vec2 Size() const { return max_ - min_; }

    /// Return half-size.
    Vec2 HalfSize() const { return (max_ - min_) * 0.5f; }

    /// Test for equality with another rect with epsilon.
    bool Equals(const Rect& rhs) const { return min_.Equals(rhs.min_) && max_.Equals(rhs.max_); }

    /// Test whether a point is inside.
    int IsInside(const Vec2& point) const
    {
        if (point.x < min_.x || point.y < min_.y || point.x > max_.x || point.y > max_.y)
            return JY_OUT;
        else
            return JY_IN;
    }

    /// Test if another rect is inside, outside or intersects.
    int IsInside(const Rect& rect) const
    {
        if (rect.max_.x < min_.x || rect.min_.x > max_.x || rect.max_.y < min_.y || rect.min_.y > max_.y)
            return JY_OUT;
        else if (rect.min_.x < min_.x || rect.max_.x > max_.x || rect.min_.y < min_.y || rect.max_.y > max_.y)
            return JY_INTERSECT;
        else
            return JY_IN;
    }

    /// Return float data.
    const float* Data() const { return &min_.x; }

    /// Return as a vector.
    Vec4 ToVector4() const { return Vec4(min_.x, min_.y, max_.x, max_.y); }

    /// Return as string.
    std::string toString() const;

    /// Return left-top corner position.
    Vec2 Min() const { return min_; }

    /// Return right-bottom corner position.
    Vec2 Max() const { return max_; }

    /// Return left coordinate.
    float Left() const { return min_.x; }

    /// Return top coordinate.
    float Top() const { return min_.y; }

    /// Return right coordinate.
    float Right() const { return max_.x; }

    /// Return bottom coordinate.
    float Bottom() const { return max_.y; }

    /// Minimum vector.
    Vec2 min_;
    /// Maximum vector.
    Vec2 max_;

    /// Rect in the range (-1, -1) - (1, 1).
    static const Rect FULL;
    /// Rect in the range (0, 0) - (1, 1).
    static const Rect POSITIVE;
    /// Zero-sized rect.
    static const Rect ZERO;
};

NS_JYE_MATH_END
