
#pragma once

#if COMPILER_METAL
	// Helps with iteration when changing Metal shader code generation backend.
	#include "Platform/Metal/MetalCommon.ush"
#elif COMPILER_VULKAN
	// Helps with iteration when changing Vulkan shader code generation backend.
	#include "Platform/Vulkan/VulkanCommon.ush"
#elif COMPILER_D3D
	#include "Platform/D3D/D3DCommon.ush"
#endif

#if COMPILER_HLSLCC
#define log10(x)  log((x)) / log(10.0)
#endif



float min3( float a, float b, float c )
{
	return min( a, min( b, c ) );
}

float max3( float a, float b, float c )
{
	return max( a, max( b, c ) );
}

float2 min3( float2 a, float2 b, float2 c )
{
	return float2(
		min3( a.x, b.x, c.x ),
		min3( a.y, b.y, c.y )
	);
}

float2 max3( float2 a, float2 b, float2 c )
{
	return float2(
		max3( a.x, b.x, c.x ),
		max3( a.y, b.y, c.y )
	);
}

float3 max3( float3 a, float3 b, float3 c )
{
	return float3(
		max3( a.x, b.x, c.x ),
		max3( a.y, b.y, c.y ),
		max3( a.z, b.z, c.z )
	);
}

float3 min3( float3 a, float3 b, float3 c )
{
	return float3(
		min3( a.x, b.x, c.x ),
		min3( a.y, b.y, c.y ),
		min3( a.z, b.z, c.z )
	);
}

float4 min3( float4 a, float4 b, float4 c )
{
	return float4(
		min3( a.x, b.x, c.x ),
		min3( a.y, b.y, c.y ),
		min3( a.z, b.z, c.z ),
		min3( a.w, b.w, c.w )
	);
}

float4 max3( float4 a, float4 b, float4 c )
{
	return float4(
		max3( a.x, b.x, c.x ),
		max3( a.y, b.y, c.y ),
		max3( a.z, b.z, c.z ),
		max3( a.w, b.w, c.w )
	);
}

