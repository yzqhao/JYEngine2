
#pragma once

#include "Math.h"

NS_JYE_MATH_BEGIN

float MathInstance::GetFastSin(unsigned int i)
{
	return FastSin[i];
}
float MathInstance::GetFastCos(unsigned int i)
{
	return FastCos[i];
}

bool VSInitMath()
{
	MathInstance::GetMathInstance();
	return true;
}

void MathInstance::VSInitCRCTable()
{
#define POLYNOMIAL 0x04c11db7L
	int i, j;
	unsigned int crc_accum;
	for (i = 0; i < 256; i++)
	{
		crc_accum = ((unsigned int)i << 24);
		for (j = 0; j < 8; j++)
		{
			if (crc_accum & 0x80000000L) crc_accum = (crc_accum << 1) ^ POLYNOMIAL;
			else crc_accum = (crc_accum << 1);
		}
		CRCTable[i] = crc_accum;
	}
}

uint MathInstance::CRC32Compute(const void* pData, uint uiDataSize)
{
	unsigned int uiResult;

	if (uiDataSize == 0) return 0;

	const unsigned char* pDataTemp = (const unsigned char*)pData;
	uiResult = *pDataTemp++ << 24;
	if (uiDataSize > 1)
	{
		uiResult |= *pDataTemp++ << 16;
		if (uiDataSize > 2)
		{
			uiResult |= *pDataTemp++ << 8;
			if (uiDataSize > 3) uiResult |= *pDataTemp++;
		}
	}
	uiResult = ~uiResult;

	for (unsigned int i = 4; i < uiDataSize; i++)
	{
		uiResult = (uiResult << 8 | *pDataTemp++) ^ CRCTable[uiResult >> 24];
	}

	return ~uiResult;
}

uint MathInstance::CRC32Compute(const void* InData, int Length, uint CRC)
{
	uint uiResult = CRC32Compute(InData, Length);

	const unsigned char* pDataTemp = (const unsigned char*)&CRC;
	for (int i = 0; i < 4; i++)
	{
		uiResult = (uiResult << 8 | *pDataTemp++) ^ CRCTable[uiResult >> 24];
	}

	return ~uiResult;
}

MathInstance::MathInstance()
{
	VSInitCRCTable();
	for (unsigned int i = 0; i <= 360; i++)
	{
		float iRadian = MATH_DEG_TO_RAD(float(i));
		FastSin[i] = sin(iRadian);
		FastCos[i] = cos(iRadian);
	}
}

NS_JYE_MATH_END
