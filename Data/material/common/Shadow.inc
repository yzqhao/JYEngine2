#pragma once

#include "Common.inc"

float4 ApplyShadowBias(float4 clipPos)
{
    float shadowBias = LIGHT_PARAM.x;
	clipPos.z += (shadowBias / clipPos.w);
	return clipPos;
}