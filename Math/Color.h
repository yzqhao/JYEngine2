
#pragma once 

#include "Vec3.h"
#include "Vec4.h"
#include "../System/DataStruct.h"

NS_JYE_MATH_BEGIN

/**
 * Enum for the different kinds of gamma spaces we expect to need to convert from/to.
 */
enum class EGammaSpace : byte
{
    /** No gamma correction is applied to this space, the incoming colors are assumed to already be in linear space. */
    Linear,
    /** A simplified sRGB gamma correction is applied, pow(1/2.2). */
    Pow22,
    /** Use the standard sRGB conversion. */
    sRGB,
};

struct FColor;

/**
 * A linear, 32-bit/component floating point RGBA color.
 */
struct MATH_API FLinearColor
{
	float	R,
			G,
			B,
			A;

	/** Static lookup table used for FColor -> FLinearColor conversion. Pow(2.2) */
	static float Pow22OneOver255Table[256];

	/** Static lookup table used for FColor -> FLinearColor conversion. sRGB */
	static float sRGBToLinearTable[256];

	FORCEINLINE explicit FLinearColor()
		: R(0), G(0), B(0), A(0)
	{}
	constexpr FORCEINLINE FLinearColor(float InR, float InG, float InB, float InA = 1.0f) : R(InR), G(InG), B(InB), A(InA) {}

	/**
	 * Converts an FColor which is assumed to be in sRGB space, into linear color space.
	 * @param Color The sRGB color that needs to be converted into linear space.
	 */
	FLinearColor(const FColor& Color);

	FLinearColor(const Vec3& Vector);

	explicit FLinearColor(const Vec4& Vector);

	const float* GetPtr() const { return &R; }
	float* GetPtr() { return &R; }

	// Conversions.
	FColor ToRGBE() const;

	/**
	 * Converts an FColor coming from an observed sRGB output, into a linear color.
	 * @param Color The sRGB color that needs to be converted into linear space.
	 */
	static FLinearColor FromSRGBColor(const FColor& Color);

	/**
	 * Converts an FColor coming from an observed Pow(1/2.2) output, into a linear color.
	 * @param Color The Pow(1/2.2) color that needs to be converted into linear space.
	 */
	static FLinearColor FromPow22Color(const FColor& Color);

	// Operators.

	FORCEINLINE float& Component(int Index)
	{
		return (&R)[Index];
	}

	FORCEINLINE const float& Component(int Index) const
	{
		return (&R)[Index];
	}

	FORCEINLINE FLinearColor operator+(const FLinearColor& ColorB) const
	{
		return FLinearColor(
			this->R + ColorB.R,
			this->G + ColorB.G,
			this->B + ColorB.B,
			this->A + ColorB.A
		);
	}
	FORCEINLINE FLinearColor& operator+=(const FLinearColor& ColorB)
	{
		R += ColorB.R;
		G += ColorB.G;
		B += ColorB.B;
		A += ColorB.A;
		return *this;
	}

	FORCEINLINE FLinearColor operator-(const FLinearColor& ColorB) const
	{
		return FLinearColor(
			this->R - ColorB.R,
			this->G - ColorB.G,
			this->B - ColorB.B,
			this->A - ColorB.A
		);
	}
	FORCEINLINE FLinearColor& operator-=(const FLinearColor& ColorB)
	{
		R -= ColorB.R;
		G -= ColorB.G;
		B -= ColorB.B;
		A -= ColorB.A;
		return *this;
	}

	FORCEINLINE FLinearColor operator*(const FLinearColor& ColorB) const
	{
		return FLinearColor(
			this->R * ColorB.R,
			this->G * ColorB.G,
			this->B * ColorB.B,
			this->A * ColorB.A
		);
	}
	FORCEINLINE FLinearColor& operator*=(const FLinearColor& ColorB)
	{
		R *= ColorB.R;
		G *= ColorB.G;
		B *= ColorB.B;
		A *= ColorB.A;
		return *this;
	}

	FORCEINLINE FLinearColor operator*(float Scalar) const
	{
		return FLinearColor(
			this->R * Scalar,
			this->G * Scalar,
			this->B * Scalar,
			this->A * Scalar
		);
	}

	FORCEINLINE FLinearColor& operator*=(float Scalar)
	{
		R *= Scalar;
		G *= Scalar;
		B *= Scalar;
		A *= Scalar;
		return *this;
	}

	FORCEINLINE FLinearColor operator/(const FLinearColor& ColorB) const
	{
		return FLinearColor(
			this->R / ColorB.R,
			this->G / ColorB.G,
			this->B / ColorB.B,
			this->A / ColorB.A
		);
	}
	FORCEINLINE FLinearColor& operator/=(const FLinearColor& ColorB)
	{
		R /= ColorB.R;
		G /= ColorB.G;
		B /= ColorB.B;
		A /= ColorB.A;
		return *this;
	}

	FORCEINLINE FLinearColor operator/(float Scalar) const
	{
		const float	InvScalar = 1.0f / Scalar;
		return FLinearColor(
			this->R * InvScalar,
			this->G * InvScalar,
			this->B * InvScalar,
			this->A * InvScalar
		);
	}
	FORCEINLINE FLinearColor& operator/=(float Scalar)
	{
		const float	InvScalar = 1.0f / Scalar;
		R *= InvScalar;
		G *= InvScalar;
		B *= InvScalar;
		A *= InvScalar;
		return *this;
	}

	// clamped in 0..1 range
	FORCEINLINE FLinearColor GetClamped(float InMin = 0.0f, float InMax = 1.0f) const
	{
		FLinearColor Ret;

		Ret.R = Clamp(R, InMin, InMax);
		Ret.G = Clamp(G, InMin, InMax);
		Ret.B = Clamp(B, InMin, InMax);
		Ret.A = Clamp(A, InMin, InMax);

		return Ret;
	}

	/** Comparison operators */
	FORCEINLINE bool operator==(const FLinearColor& ColorB) const
	{
		return this->R == ColorB.R && this->G == ColorB.G && this->B == ColorB.B && this->A == ColorB.A;
	}
	FORCEINLINE bool operator!=(const FLinearColor& Other) const
	{
		return this->R != Other.R || this->G != Other.G || this->B != Other.B || this->A != Other.A;
	}

	// Error-tolerant comparison.
	FORCEINLINE bool Equals(const FLinearColor& ColorB, float Tolerance = M_EPSILON) const
	{
		return (ABS(this->R - ColorB.R) < Tolerance) && (ABS(this->G - ColorB.G) < Tolerance) && (ABS(this->B - ColorB.B) < Tolerance) && (ABS(this->A - ColorB.A) < Tolerance);
	}

	FLinearColor CopyWithNewOpacity(float NewOpacicty) const
	{
		FLinearColor NewCopy = *this;
		NewCopy.A = NewOpacicty;
		return NewCopy;
	}

	/**
	 * Converts byte hue-saturation-brightness to floating point red-green-blue.
	 */
	static FLinearColor MakeFromHSV8(byte H, byte S, byte V);

	/**
	* Makes a random but quite nice color.
	*/
	static FLinearColor MakeRandomColor();

	/**
	* Converts temperature in Kelvins of a black body radiator to RGB chromaticity.
	*/
	static FLinearColor MakeFromColorTemperature(float Temp);

	/**
	 * Euclidean distance between two points.
	 */
	static inline float Dist(const FLinearColor& V1, const FLinearColor& V2)
	{
		return sqrt(Square(V2.R - V1.R) + Square(V2.G - V1.G) + Square(V2.B - V1.B) + Square(V2.A - V1.A));
	}

	/**
	 * Generates a list of sample points on a Bezier curve defined by 2 points.
	 *
	 * @param	ControlPoints	Array of 4 Linear Colors (vert1, controlpoint1, controlpoint2, vert2).
	 * @param	NumPoints		Number of samples.
	 * @param	OutPoints		Receives the output samples.
	 * @return					Path length.
	 */
	static float EvaluateBezier(const FLinearColor* ControlPoints, int NumPoints, Vector<FLinearColor>& OutPoints);

	/** Converts a linear space RGB color to an HSV color */
	FLinearColor LinearRGBToHSV() const;

	/** Converts an HSV color to a linear space RGB color */
	FLinearColor HSVToLinearRGB() const;

	/**
	 * Linearly interpolates between two colors by the specified progress amount.  The interpolation is performed in HSV color space
	 * taking the shortest path to the new color's hue.  This can give better results than Lerp(), but is much more expensive.
	 * The incoming colors are in RGB space, and the output color will be RGB.  The alpha value will also be interpolated.
	 *
	 * @param	From		The color and alpha to interpolate from as linear RGBA
	 * @param	To			The color and alpha to interpolate to as linear RGBA
	 * @param	Progress	Scalar interpolation amount (usually between 0.0 and 1.0 inclusive)
	 * @return	The interpolated color in linear RGB space along with the interpolated alpha value
	 */
	static FLinearColor LerpUsingHSV(const FLinearColor& From, const FLinearColor& To, const float Progress);

	/** Quantizes the linear color and returns the result as a FColor.  This bypasses the SRGB conversion. */
	FColor Quantize() const;

	/** Quantizes the linear color with rounding and returns the result as a FColor.  This bypasses the SRGB conversion. */
	FColor QuantizeRound() const;

	/** Quantizes the linear color and returns the result as a FColor with optional sRGB conversion and quality as goal. */
	FColor ToFColor(const bool bSRGB) const;

	/**
	 * Returns a desaturated color, with 0 meaning no desaturation and 1 == full desaturation
	 *
	 * @param	Desaturation	Desaturation factor in range [0..1]
	 * @return	Desaturated color
	 */
	FLinearColor Desaturate(float Desaturation) const;

	/** Computes the perceptually weighted luminance value of a color. */
	inline float ComputeLuminance() const
	{
		return R * 0.3f + G * 0.59f + B * 0.11f;
	}

	/**
	 * Returns the maximum value in this color structure
	 *
	 * @return The maximum color channel value
	 */
	FORCEINLINE float GetMax() const
	{
		return Max(Max(Max(R, G), B), A);
	}

	/** useful to detect if a light contribution needs to be rendered */
	bool IsAlmostBlack() const
	{
		return Square(R) < M_EPSILON && Square(G) < M_EPSILON && Square(B) < M_EPSILON;
	}

	/**
	 * Returns the minimum value in this color structure
	 *
	 * @return The minimum color channel value
	 */
	FORCEINLINE float GetMin() const
	{
		return Min(Min(Min(R, G), B), A);
	}

	FORCEINLINE float GetLuminance() const
	{
		return R * 0.3f + G * 0.59f + B * 0.11f;
	}

	String ToString() const
	{
		return StringFormat(("(R=%f,G=%f,B=%f,A=%f)"), R, G, B, A);
	}

	// Common colors.	
	static const FLinearColor White;
	static const FLinearColor Gray;
	static const FLinearColor Black;
	static const FLinearColor Transparent;
	static const FLinearColor Red;
	static const FLinearColor Green;
	static const FLinearColor Blue;
	static const FLinearColor Yellow;
};

FORCEINLINE FLinearColor operator*(float Scalar, const FLinearColor& Color)
{
	return Color.operator*(Scalar);
}

//
//	FColor
//	Stores a color with 8 bits of precision per channel.  
//	Note: Linear color values should always be converted to gamma space before stored in an FColor, as 8 bits of precision is not enough to store linear space colors!
//	This can be done with FLinearColor::ToFColor(true) 
//

struct MATH_API FColor
{
public:

	union { struct { byte A, R, G, B; }; uint AlignmentDummy; };

	const byte* GetPtr() const { return &A; }

	uint& DWColor(void) { return *((uint*)this); }
	const uint& DWColor(void) const { return *((uint*)this); }

	// Constructors.
	FORCEINLINE explicit FColor()
	{
		// put these into the body for proper ordering with INTEL vs non-INTEL_BYTE_ORDER
		R = G = B = A = 0;
	}
	constexpr FORCEINLINE FColor(byte InR, byte InG, byte InB, byte InA = 255)
		: A(InA), R(InR), G(InG), B(InB)
	{}

	FORCEINLINE explicit FColor(uint InColor)
	{
		DWColor() = InColor;
	}

	// Operators.
	FORCEINLINE bool operator==(const FColor& C) const
	{
		return DWColor() == C.DWColor();
	}

	FORCEINLINE bool operator!=(const FColor& C) const
	{
		return DWColor() != C.DWColor();
	}

	FORCEINLINE void operator+=(const FColor& C)
	{
		R = (byte)Min((int)R + (int)C.R, 255);
		G = (byte)Min((int)G + (int)C.G, 255);
		B = (byte)Min((int)B + (int)C.B, 255);
		A = (byte)Min((int)A + (int)C.A, 255);
	}

	FLinearColor FromRGBE() const;

	/**
	 * Makes a random but quite nice color.
	 */
	static FColor MakeRandomColor();

	/**
	 * Makes a color red->green with the passed in scalar (e.g. 0 is red, 1 is green)
	 */
	static FColor MakeRedToGreenColorFromScalar(float Scalar);

	/**
	* Converts temperature in Kelvins of a black body radiator to RGB chromaticity.
	*/
	static FColor MakeFromColorTemperature(float Temp);

	/**
	 *	@return a new FColor based of this color with the new alpha value.
	 *	Usage: const FColor& MyColor = FColorList::Green.WithAlpha(128);
	 */
	FColor WithAlpha(byte Alpha) const
	{
		return FColor(R, G, B, Alpha);
	}

	/**
	 * Reinterprets the color as a linear color.
	 *
	 * @return The linear color representation.
	 */
	FORCEINLINE FLinearColor ReinterpretAsLinear() const
	{
		return FLinearColor(R / 255.f, G / 255.f, B / 255.f, A / 255.f);
	}

	/**
	 * Converts this color value to a hexadecimal string.
	 *
	 * The format of the string is RRGGBBAA.
	 *
	 * @return Hexadecimal string.
	 * @see FromHex, ToString
	 */
	FORCEINLINE String ToHex() const
	{
		return StringFormat(("%02X%02X%02X%02X"), R, G, B, A);
	}

	/**
	 * Converts this color value to a string.
	 *
	 * @return The string representation.
	 * @see ToHex
	 */
	FORCEINLINE String ToString() const
	{
		return StringFormat(("(R=%i,G=%i,B=%i,A=%i)"), R, G, B, A);
	}

	/**
	 * Gets the color in a packed uint format packed in the order ARGB.
	 */
	FORCEINLINE uint ToPackedARGB() const
	{
		return (A << 24) | (R << 16) | (G << 8) | (B << 0);
	}

	/**
	 * Gets the color in a packed uint format packed in the order ABGR.
	 */
	FORCEINLINE uint ToPackedABGR() const
	{
		return (A << 24) | (B << 16) | (G << 8) | (R << 0);
	}

	/**
	 * Gets the color in a packed uint format packed in the order RGBA.
	 */
	FORCEINLINE uint ToPackedRGBA() const
	{
		return (R << 24) | (G << 16) | (B << 8) | (A << 0);
	}

	/**
	 * Gets the color in a packed uint format packed in the order BGRA.
	 */
	FORCEINLINE uint ToPackedBGRA() const
	{
		return (B << 24) | (G << 16) | (R << 8) | (A << 0);
	}

	/** Some pre-inited colors, useful for debug code */
	static const FColor White;
	static const FColor Black;
	static const FColor Gray;
	static const FColor Transparent;
	static const FColor Red;
	static const FColor Green;
	static const FColor Blue;
	static const FColor Yellow;
	static const FColor Cyan;
	static const FColor Magenta;
	static const FColor Orange;
	static const FColor Purple;
	static const FColor Turquoise;
	static const FColor Silver;
	static const FColor Emerald;

private:
	/**
	 * Please use .ToFColor(true) on FLinearColor if you wish to convert from FLinearColor to FColor,
	 * with proper sRGB conversion applied.
	 *
	 * Note: Do not implement or make public.  We don't want people needlessly and implicitly converting between
	 * FLinearColor and FColor.  It's not a free conversion.
	 */
	explicit FColor(const FLinearColor& LinearColor);
};

FORCEINLINE uint GetTypeHash(const FColor& Color)
{
	return Color.DWColor();
}

FORCEINLINE uint GetTypeHash(const FLinearColor& LinearColor)
{
	// Note: this assumes there's no padding in FLinearColor that could contain uncompared data.
	return MathInstance::GetMathInstance().CRC32Compute(&LinearColor, sizeof(LinearColor));
}

NS_JYE_MATH_END