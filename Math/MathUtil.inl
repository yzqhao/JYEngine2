
namespace MathUtil
{
	FORCEINLINE unsigned short FloatToHalf
	(
		float Value
	)
	{
		unsigned int Result;

		unsigned int IValue = ((unsigned int*)(&Value))[0];
		unsigned int Sign = (IValue & 0x80000000U) >> 16U;
		IValue = IValue & 0x7FFFFFFFU;      // Hack off the sign

		if (IValue > 0x47FFEFFFU)
		{
			// The number is too large to be represented as a half.  Saturate to infinity.
			Result = 0x7FFFU;
		}
		else
		{
			if (IValue < 0x38800000U)
			{
				// The number is too small to be represented as a normalized half.
				// Convert it to a denormalized value.
				unsigned int Shift = 113U - (IValue >> 23U);
				IValue = (0x800000U | (IValue & 0x7FFFFFU)) >> Shift;
			}
			else
			{
				// Rebias the exponent to represent the value as a normalized half.
				IValue += 0xC8000000U;
			}

			Result = ((IValue + 0x0FFFU + ((IValue >> 13U) & 1U)) >> 13U) & 0x7FFFU;
		}
		return (unsigned short)(Result | Sign);
	}
	FORCEINLINE float HalfToFloat
	(
		unsigned short Value
	)
	{


		unsigned int Mantissa;
		unsigned int Exponent;
		unsigned int Result;

		Mantissa = (unsigned int)(Value & 0x03FF);

		if ((Value & 0x7C00) != 0)  // The value is normalized
		{
			Exponent = (unsigned int)((Value >> 10) & 0x1F);
		}
		else if (Mantissa != 0)     // The value is denormalized
		{
			// Normalize the value in the resulting float
			Exponent = 1;

			do
			{
				Exponent--;
				Mantissa <<= 1;
			} while ((Mantissa & 0x0400) == 0);

			Mantissa &= 0x03FF;
		}
		else                        // The value is zero
		{
			Exponent = (unsigned int)-112;
		}

		Result = ((Value & 0x8000) << 16) | // Sign
			((Exponent + 112) << 23) | // Exponent
			(Mantissa << 13);          // Mantissa

		return *(float*)&Result;
	}
	//f must [0,1]
	FORCEINLINE unsigned int CompressUnitFloat(float f, unsigned int Bit)
	{
		unsigned int nIntervals = 1 << Bit;
		float scaled = f * (nIntervals - 1);
		unsigned int rounded = (unsigned int)(scaled + 0.5f);
		if (rounded > nIntervals - 1)
		{
			rounded = nIntervals - 1;
		}
		return rounded;

	}
	FORCEINLINE unsigned int CompressFloat(float f, float Max, float Min, unsigned int Bit)
	{
		float Unitf = (f - Min) / (Max - Min);
		return CompressUnitFloat(Unitf, Bit);
	}
	FORCEINLINE float DecompressUnitFloat(unsigned int quantized, unsigned int Bit)
	{
		unsigned int nIntervals = 1 << Bit;
		float IntervalSize = 1.0f / (nIntervals - 1);

		return quantized * IntervalSize;
	}
	FORCEINLINE float DecompressFloat(unsigned int quantized, float Max, float Min, unsigned int Bit)
	{
		float Unitf = DecompressUnitFloat(quantized, Bit);
		return (Min + Unitf * (Max - Min));
	}
};

