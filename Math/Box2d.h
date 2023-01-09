
#pragma once

#include "Vec2.h"
#include "../System/DataStruct.h"

NS_JYE_MATH_BEGIN

/**
 * Implements a rectangular 2D Box.
 */
struct FBox2D
{
public:
	/** Holds the box's minimum point. */
	Vec2 Min;

	/** Holds the box's maximum point. */
	Vec2 Max;

	/** Holds a flag indicating whether this box is valid. */
	bool bIsValid;

public:

	/** Default constructor (no initialization). */
	FBox2D() { }

	/**
	 * Creates and initializes a new box.
	 *
	 * The box extents are initialized to zero and the box is marked as invalid.
	 *
	 * @param EForceInit Force Init Enum.
	 */
	explicit FBox2D(EForceInit)
	{
		Init();
	}

	/**
	 * Creates and initializes a new box from the specified parameters.
	 *
	 * @param InMin The box's minimum point.
	 * @param InMax The box's maximum point.
	 */
	FBox2D(const Vec2& InMin, const Vec2& InMax)
		: Min(InMin)
		, Max(InMax)
		, bIsValid(true)
	{ }

	/**
	 * Creates and initializes a new box from the given set of points.
	 *
	 * @param Points Array of Points to create for the bounding volume.
	 * @param Count The number of points.
	 */
	MATH_API FBox2D(const Vec2* Points, const int Count);

	/**
	 * Creates and initializes a new box from an array of points.
	 *
	 * @param Points Array of Points to create for the bounding volume.
	 */
	MATH_API FBox2D(const Vector<Vec2>& Points);

public:

	/**
	 * Compares two boxes for equality.
	 *
	 * @param Other The other box to compare with.
	 * @return true if the boxes are equal, false otherwise.
	 */
	bool operator==(const FBox2D& Other) const
	{
		return (Min == Other.Min) && (Max == Other.Max);
	}

	/**
	* Compares two boxes for inequality.
	*
	* @param Other The other box to compare with.
	* @return true if the boxes are not equal, false otherwise.
	*/
	bool operator!=(const FBox2D& Other) const
	{
		return !(*this == Other);
	}

	/**
	 * Adds to this bounding box to include a given point.
	 *
	 * @param Other The point to increase the bounding volume to.
	 * @return Reference to this bounding box after resizing to include the other point.
	 */
	FORCEINLINE FBox2D& operator+=(const Vec2& Other);

	/**
	 * Gets the result of addition to this bounding volume.
	 *
	 * @param Other The other point to add to this.
	 * @return A new bounding volume.
	 */
	FBox2D operator+(const Vec2& Other) const
	{
		return FBox2D(*this) += Other;
	}

	/**
	 * Adds to this bounding box to include a new bounding volume.
	 *
	 * @param Other The bounding volume to increase the bounding volume to.
	 * @return Reference to this bounding volume after resizing to include the other bounding volume.
	 */
	FORCEINLINE FBox2D& operator+=(const FBox2D& Other);

	/**
	 * Gets the result of addition to this bounding volume.
	 *
	 * @param Other The other volume to add to this.
	 * @return A new bounding volume.
	 */
	FBox2D operator+(const FBox2D& Other) const
	{
		return FBox2D(*this) += Other;
	}

	/**
	 * Gets reference to the min or max of this bounding volume.
	 *
	 * @param Index The index into points of the bounding volume.
	 * @return A reference to a point of the bounding volume.
	 */
	Vec2& operator[](int Index)
	{
		JY_ASSERT((Index >= 0) && (Index < 2));

		if (Index == 0)
		{
			return Min;
		}

		return Max;
	}

public:

	/**
	 * Calculates the distance of a point to this box.
	 *
	 * @param Point The point.
	 * @return The distance.
	 */
	FORCEINLINE float ComputeSquaredDistanceToPoint(const Vec2& Point) const
	{
		// Accumulates the distance as we iterate axis
		float DistSquared = 0.f;

		if (Point.x < Min.x)
		{
			DistSquared += Square(Point.x - Min.x);
		}
		else if (Point.x > Max.x)
		{
			DistSquared += Square(Point.x - Max.x);
		}

		if (Point.y < Min.y)
		{
			DistSquared += Square(Point.y - Min.y);
		}
		else if (Point.y > Max.y)
		{
			DistSquared += Square(Point.y - Max.y);
		}

		return DistSquared;
	}

	/**
	 * Increase the bounding box volume.
	 *
	 * @param W The size to increase volume by.
	 * @return A new bounding box increased in size.
	 */
	FBox2D ExpandBy(const float W) const
	{
		return FBox2D(Min - Vec2(W, W), Max + Vec2(W, W));
	}

	/**
	 * Gets the box area.
	 *
	 * @return Box area.
	 * @see GetCenter, GetCenterAndExtents, GetExtent, GetSize
	 */
	float GetArea() const
	{
		return (Max.x - Min.x) * (Max.y - Min.y);
	}

	/**
	 * Gets the box's center point.
	 *
	 * @return Th center point.
	 * @see GetArea, GetCenterAndExtents, GetExtent, GetSize
	 */
	Vec2 GetCenter() const
	{
		return Vec2((Min + Max) * 0.5f);
	}

	/**
	 * Get the center and extents
	 *
	 * @param center[out] reference to center point
	 * @param Extents[out] reference to the extent around the center
	 * @see GetArea, GetCenter, GetExtent, GetSize
	 */
	void GetCenterAndExtents(Vec2& center, Vec2& Extents) const
	{
		Extents = GetExtent();
		center = Min + Extents;
	}

	/**
	 * Calculates the closest point on or inside the box to a given point in space.
	 *
	 * @param Point The point in space.
	 *
	 * @return The closest point on or inside the box.
	 */
	FORCEINLINE Vec2 GetClosestPointTo(const Vec2& Point) const;

	/**
	 * Gets the box extents around the center.
	 *
	 * @return Box extents.
	 * @see GetArea, GetCenter, GetCenterAndExtents, GetSize
	 */
	Vec2 GetExtent() const
	{
		return (Max - Min) * 0.5;
	}


	/**
	 * Gets the box size.
	 *
	 * @return Box size.
	 * @see GetArea, GetCenter, GetCenterAndExtents, GetExtent
	 */
	Vec2 GetSize() const
	{
		return (Max - Min);
	}

	/**
	 * Set the initial values of the bounding box to Zero.
	 */
	void Init()
	{
		Min = Max = Vec2::ZERO;
		bIsValid = false;
	}

	/**
	 * Checks whether the given box intersects this box.
	 *
	 * @param other bounding box to test intersection
	 * @return true if boxes intersect, false otherwise.
	 */
	FORCEINLINE bool Intersect(const FBox2D& other) const;

	/**
	 * Checks whether the given point is inside this box.
	 *
	 * @param Point The point to test.
	 * @return true if the point is inside this box, otherwise false.
	 */
	bool IsInside(const Vec2& TestPoint) const
	{
		return ((TestPoint.x > Min.x) && (TestPoint.x < Max.x) && (TestPoint.y > Min.y) && (TestPoint.y < Max.y));
	}

	/**
	 * Checks whether the given box is fully encapsulated by this box.
	 *
	 * @param Other The box to test for encapsulation within the bounding volume.
	 * @return true if box is inside this volume, false otherwise.
	 */
	bool IsInside(const FBox2D& Other) const
	{
		return (IsInside(Other.Min) && IsInside(Other.Max));
	}

	/**
	 * Shift bounding box position.
	 *
	 * @param The offset vector to shift by.
	 * @return A new shifted bounding box.
	 */
	FBox2D ShiftBy(const Vec2& Offset) const
	{
		return FBox2D(Min + Offset, Max + Offset);
	}

	/**
	 * Get a textual representation of this box.
	 *
	 * @return A string describing the box.
	 */
	std::string toString() const;
};


/* FBox2D inline functions
 *****************************************************************************/

FORCEINLINE FBox2D& FBox2D::operator+=(const Vec2& Other)
{
	if (bIsValid)
	{
		Min.x = Math::Min(Min.x, Other.x);
		Min.y = Math::Min(Min.y, Other.y);

		Max.x = Math::Max(Max.x, Other.x);
		Max.y = Math::Max(Max.y, Other.y);

	}
	else
	{
		Min = Max = Other;
		bIsValid = true;
	}

	return *this;
}


FORCEINLINE FBox2D& FBox2D::operator+=(const FBox2D& Other)
{
	if (bIsValid && Other.bIsValid)
	{
		Min.x = Math::Min(Min.x, Other.Min.x);
		Min.y = Math::Min(Min.y, Other.Min.y);

		Max.x = Math::Max(Max.x, Other.Max.x);
		Max.y = Math::Max(Max.y, Other.Max.y);
	}
	else if (Other.bIsValid)
	{
		*this = Other;
	}

	return *this;
}


FORCEINLINE Vec2 FBox2D::GetClosestPointTo(const Vec2& Point) const
{
	// start by considering the point inside the box
	Vec2 ClosestPoint = Point;

	// now clamp to inside box if it's outside
	if (Point.x < Min.x)
	{
		ClosestPoint.x = Min.x;
	}
	else if (Point.x > Max.x)
	{
		ClosestPoint.x = Max.x;
	}

	// now clamp to inside box if it's outside
	if (Point.y < Min.y)
	{
		ClosestPoint.y = Min.y;
	}
	else if (Point.y > Max.y)
	{
		ClosestPoint.y = Max.y;
	}

	return ClosestPoint;
}


FORCEINLINE bool FBox2D::Intersect(const FBox2D& Other) const
{
	if ((Min.x > Other.Max.x) || (Other.Min.x > Max.x))
	{
		return false;
	}

	if ((Min.y > Other.Max.y) || (Other.Min.y > Max.y))
	{
		return false;
	}

	return true;
}


FORCEINLINE std::string FBox2D::toString() const
{
	return StringFormat("bIsValid=%s, Min=(%s), Max=(%s)", bIsValid ? ("true") : ("false"), Min.toString().c_str(), Max.toString().c_str());
}

NS_JYE_MATH_END
