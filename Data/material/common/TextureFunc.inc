#pragma once

#include "Platform.inc"

// #if !PIXELSHADER && !defined(USE_FORCE_TEXTURE_MIP)
// 	#define USE_FORCE_TEXTURE_MIP 1
// #endif
#ifndef USE_FORCE_TEXTURE_MIP
	#define USE_FORCE_TEXTURE_MIP 0
#endif
#ifndef FORCED_TEXTURE_MIP
	#define FORCED_TEXTURE_MIP 0.0f
#endif
static float GlobalTextureMipBias = 0;

// These types are used for material translator generated code, or any functions the translated code can call
#if PIXELSHADER
	#define MaterialFloat half
	#define MaterialFloat2 half2
	#define MaterialFloat3 half3
	#define MaterialFloat4 half4
	#define MaterialFloat3x3 half3x3
	#define MaterialFloat4x4 half4x4 
	#define MaterialFloat4x3 half4x3 
#else
	// Material translated vertex shader code always uses floats, 
	// Because it's used for things like world position and UVs
	#define MaterialFloat float
	#define MaterialFloat2 float2
	#define MaterialFloat3 float3
	#define MaterialFloat4 float4
	#define MaterialFloat3x3 float3x3
	#define MaterialFloat4x4 float4x4 
	#define MaterialFloat4x3 float4x3 
#endif

// This would need to be a #define in GLSL to ignore the SamplerState, however, it is currently a function call in HLSL
// for type checking of the parameters - ironically the type checking is really only needed in GLSL!
MaterialFloat4 Texture1DSample(Texture1D Tex, SamplerState Sampler, float UV)
{
#if USE_FORCE_TEXTURE_MIP
	return Tex.SampleLevel(Sampler, UV, 0);
#else
	return Tex.Sample(Sampler, UV);
#endif
}

MaterialFloat4 Texture2DSample(Texture2D Tex, SamplerState Sampler, float2 UV)
{
#if USE_FORCE_TEXTURE_MIP
	return Tex.SampleLevel(Sampler, UV, FORCED_TEXTURE_MIP + GlobalTextureMipBias);
#else
	return Tex.Sample(Sampler, UV);
#endif
}

MaterialFloat4 Texture3DSample(Texture3D Tex, SamplerState Sampler, float3 UV)
{
#if USE_FORCE_TEXTURE_MIP
	return Tex.SampleLevel(Sampler, UV, 0);
#else
	return Tex.Sample(Sampler, UV);
#endif
}
MaterialFloat4 TextureCubeSample(TextureCube Tex, SamplerState Sampler, float3 UV)
{
#if USE_FORCE_TEXTURE_MIP
	return Tex.SampleLevel(Sampler, UV, 0);
#else
	return Tex.Sample(Sampler, UV);
#endif
}
MaterialFloat4 Texture2DArraySample(Texture2DArray Tex, SamplerState Sampler, float3 UV)
{
#if USE_FORCE_TEXTURE_MIP
	return Tex.SampleLevel(Sampler, UV, 0);
#else
	return Tex.Sample(Sampler, UV);
#endif
}
MaterialFloat4 Texture1DSampleLevel(Texture1D Tex, SamplerState Sampler, float UV, MaterialFloat Mip)
{
	return Tex.SampleLevel(Sampler, UV, Mip);
}
MaterialFloat4 Texture2DSampleLevel(Texture2D Tex, SamplerState Sampler, float2 UV, MaterialFloat Mip)
{
	return Tex.SampleLevel(Sampler, UV, Mip);
}
MaterialFloat4 Texture2DSampleBias(Texture2D Tex, SamplerState Sampler, float2 UV, MaterialFloat MipBias)
{
#if USE_FORCE_TEXTURE_MIP
	return Tex.SampleLevel(Sampler, UV, FORCED_TEXTURE_MIP + MipBias + GlobalTextureMipBias);
#else
	return Tex.SampleBias(Sampler, UV, MipBias);
#endif
}
MaterialFloat4 Texture2DSampleGrad(Texture2D Tex, SamplerState Sampler, float2 UV, MaterialFloat2 DDX, MaterialFloat2 DDY)
{
	return Tex.SampleGrad(Sampler, UV, DDX, DDY);
}
MaterialFloat4 Texture3DSampleLevel(Texture3D Tex, SamplerState Sampler, float3 UV, MaterialFloat Mip)
{
	return Tex.SampleLevel(Sampler, UV, Mip);
}
MaterialFloat4 Texture3DSampleBias(Texture3D Tex, SamplerState Sampler, float3 UV, MaterialFloat MipBias)
{
#if USE_FORCE_TEXTURE_MIP
	return Tex.SampleLevel(Sampler, UV, 0);
#else
	return Tex.SampleBias(Sampler, UV, MipBias);
#endif
}
MaterialFloat4 Texture3DSampleGrad(Texture3D Tex, SamplerState Sampler, float3 UV, MaterialFloat3 DDX, MaterialFloat3 DDY)
{
	return Tex.SampleGrad(Sampler, UV, DDX, DDY);
}
MaterialFloat4 TextureCubeSampleLevel(TextureCube Tex, SamplerState Sampler, float3 UV, MaterialFloat Mip)
{
	return Tex.SampleLevel(Sampler, UV, Mip);
}
MaterialFloat TextureCubeSampleDepthLevel(TextureCube TexDepth, SamplerState Sampler, float3 UV, MaterialFloat Mip)
{
	return TexDepth.SampleLevel(Sampler, UV, Mip).x;
}
MaterialFloat4 TextureCubeSampleBias(TextureCube Tex, SamplerState Sampler, float3 UV, MaterialFloat MipBias)
{
#if USE_FORCE_TEXTURE_MIP
	return Tex.SampleLevel(Sampler, UV, 0);
#else
	return Tex.SampleBias(Sampler, UV, MipBias);
#endif
}
MaterialFloat4 TextureCubeSampleGrad(TextureCube Tex, SamplerState Sampler, float3 UV, MaterialFloat3 DDX, MaterialFloat3 DDY)
{
	return Tex.SampleGrad(Sampler, UV, DDX, DDY);
}

// Re-routed texture sampling for decals
// On iOS and Android MALI devices decal UVs has issues with deriavatives on polygon edges resulting in 'wireframe' rendering artifacts (UE-50971)
// to workaround we always sample top mip level
MaterialFloat4 Texture1DSample_Decal(Texture1D Tex, SamplerState Sampler, float UV)
{
	return Texture1DSample(Tex, Sampler, UV);
}
MaterialFloat4 Texture2DSample_Decal(Texture2D Tex, SamplerState Sampler, float2 UV)
{
#if METAL_PROFILE || COMPILER_GLSL_ES3_1
	return Texture2DSampleGrad(Tex, Sampler, UV, 0.f, 0.f);
#else
	return Texture2DSample(Tex, Sampler, UV);
#endif
}
MaterialFloat4 Texture3DSample_Decal(Texture3D Tex, SamplerState Sampler, float3 UV)
{
#if METAL_PROFILE || COMPILER_GLSL_ES3_1
	return Texture3DSampleGrad(Tex, Sampler, UV, 0.f, 0.f);
#else
	return Texture3DSample(Tex, Sampler, UV);
#endif
}
MaterialFloat4 TextureCubeSample_Decal(TextureCube Tex, SamplerState Sampler, float3 UV)
{
#if METAL_PROFILE || COMPILER_GLSL_ES3_1
	return TextureCubeSampleGrad(Tex, Sampler, UV, 0.f, 0.f);
#else
	return TextureCubeSample(Tex, Sampler, UV);
#endif
}

MaterialFloat4 Texture2DArraySampleLevel(Texture2DArray Tex, SamplerState Sampler, float3 UV, MaterialFloat Mip)
{
	return Tex.SampleLevel(Sampler, UV, Mip);
}
MaterialFloat4 Texture2DArraySampleBias(Texture2DArray Tex, SamplerState Sampler, float3 UV, MaterialFloat MipBias)
{
#if USE_FORCE_TEXTURE_MIP
	return Tex.SampleLevel(Sampler, UV, 0);
#else
	return Tex.SampleBias(Sampler, UV, MipBias);
#endif
}
MaterialFloat4 Texture2DArraySampleGrad(Texture2DArray Tex, SamplerState Sampler, float3 UV, MaterialFloat2 DDX, MaterialFloat2 DDY)
{
	return Tex.SampleGrad(Sampler, UV, DDX, DDY);
}