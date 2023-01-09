#pragma once

#include "TextureFunc.inc"
#include "Instancing.inc"

float4x4 LOCALWORLD_TRANSFORM;
float4x4 WORLDLOCAL_TRANSFROM;
float4x4 CAMERA_VIEW;
float4x4 CAMERA_VIEWPROJ;
float4x4 CAMERA_PROJECTION;
float4x4 CAMERA_VIEWPROJ_INV;
float4x4 LOCALSCREEN_TRANSVIEWPROJ;
float3 CAMERA_WORLDPOSITION;
float3 LIGHT_GIVEN_DIRECTION;
float3 LIGHT_POSITION;
float3 WORLD_POSITION;
float LIGHT_RANGE_INV;
float4 LIGHT_ATTENUATION;
float3 LIGHT_COLOR;
float3 AMBIENT_COLOR;
float2 LIGHT_INNER_DIFF_INV;
float4 LIGHT_PARAM;
float4x4 LIGHT_CAMERA_VIEW;
float4x4 LIGHT_CAMERA_PROJECTION;
float DEVICE_COORDINATE_Y_FLIP;

const static MaterialFloat PI = 3.1415926535897932f;
const static MaterialFloat INV_PI = 0.31830988618f;
const static float MaxHalfFloat = 65504.0f;
const static float Max10BitsFloat = 64512.0f;
const static float4x4 MatrixIdentity = float4x4(1.0, 0.0, 0.0, 0.0,
                                                0.0, 1.0, 0.0, 0.0,
                                                0.0, 0.0, 1.0, 0.0,
                                                0.0, 0.0, 0.0, 1.0);

#if GPUSKIN4
    static float4x4 BONE_TRANSFROM;
    float4 ANIMATION_MATRIX_ARRAY[210];
    #define ANI_ATTRIBUTE float4 boneIndices:BLENDINDICES; float4 boneWeight:BLENDWEIGHT;
	                         
	#define COMPUTE_BONE_MATERIX(v) int boneIndex0 = int(v.boneIndices.x) * 3;\
						int boneIndex1 = int(v.boneIndices.y) * 3;\
						int boneIndex2 = int(v.boneIndices.z) * 3;\
						int boneIndex3 = int(v.boneIndices.w) * 3;\
						float4 row0;\
						float4 row1;\
						float4 row2;\
						float4 row3;\
						row0 = ANIMATION_MATRIX_ARRAY[boneIndex0] * v.boneWeight.x;;\
						row1 = ANIMATION_MATRIX_ARRAY[boneIndex0 + 1] * v.boneWeight.x;\
						row2 = ANIMATION_MATRIX_ARRAY[boneIndex0 + 2] * v.boneWeight.x;\
						row0 += ANIMATION_MATRIX_ARRAY[boneIndex1] * v.boneWeight.y;\
						row1 += ANIMATION_MATRIX_ARRAY[boneIndex1 + 1] * v.boneWeight.y;\
						row2 += ANIMATION_MATRIX_ARRAY[boneIndex1 + 2] * v.boneWeight.y;\
						row0 += ANIMATION_MATRIX_ARRAY[boneIndex2] * v.boneWeight.z;\
						row1 += ANIMATION_MATRIX_ARRAY[boneIndex2 + 1] * v.boneWeight.z;\
						row2 += ANIMATION_MATRIX_ARRAY[boneIndex2 + 2] * v.boneWeight.z;\
						row0 += ANIMATION_MATRIX_ARRAY[boneIndex3] * v.boneWeight.w;\
						row1 += ANIMATION_MATRIX_ARRAY[boneIndex3 + 1] * v.boneWeight.w;\
						row2 += ANIMATION_MATRIX_ARRAY[boneIndex3 + 2] * v.boneWeight.w;\
						row3 = float4(0.0, 0.0, 0.0, 1.0);\
						BONE_TRANSFROM = float4x4(row0.x, row1.x, row2.x, 0.0,\
												row0.y, row1.y, row2.y, 0.0,\
												row0.z, row1.z, row2.z, 0.0,\
												row0.w, row1.w, row2.w, 1.0)

    inline float4 ObjectToClipPos(float4 pos4)
    {
          return mul(mul(mul(pos4, BONE_TRANSFROM), ObjToWorld), CAMERA_VIEWPROJ);
    }
    
    inline float4 WorldToClipPos(float4 pos4)
    {
         return mul(mul(pos4, BONE_TRANSFROM), CAMERA_VIEWPROJ);
    }

    inline float3 ObjectToWorldNormal(float3 normal)
    {
        return normalize(mul(mul(normal, (float3x3)BONE_TRANSFROM), (float3x3)ObjToWorld));
    }

    inline float3 ObjectToWorldTangent(float3 tangent)
    {
        return normalize(mul(mul(tangent, (float3x3)BONE_TRANSFROM), (float3x3)ObjToWorld));
    }

    inline float3 ObjectToWorldDir(float3 dir)
    {
        return normalize(mul(mul(dir, (float3x3)BONE_TRANSFROM), (float3x3)ObjToWorld));
    }

    inline float4 ObjectToWorldPos(float4 pos4)
    {
       return mul(mul(pos4, BONE_TRANSFROM), ObjToWorld);
    }
    
    inline float4 ObjectToViewPos(float4 pos4)
    {
       return mul(mul(pos4, BONE_TRANSFROM), mul(ObjToWorld, CAMERA_VIEW));
    }
    
    inline float4 ObjectPos(float4 pos4)
    {
       return mul(pos4, BONE_TRANSFROM);
    }

    inline float4 WorldToObjectPos(float4 pos4)
    {
       return mul(pos4, WORLDLOCAL_TRANSFROM);
    }

#else
    #define ANI_ATTRIBUTE
    #define BONE_MATERIX
    #define COMPUTE_BONE_MATERIX(v)

	inline float4 ObjectToClipPos(in float4 pos)
	{
    	// More efficient than computing M*VP matrix product
    	return mul(pos, mul(ObjToWorld, CAMERA_VIEWPROJ));
	}

    inline float4 WorldToClipPos(in float4 pos)
    {
        return mul(pos, CAMERA_VIEWPROJ);
    }

    inline float3 ObjectToWorldNormal(float3 normal)
    {
        return normalize(mul(normal, (float3x3)ObjToWorld));
    }

    inline float3 ObjectToWorldTangent(float3 tangent)
    {
        return normalize(mul(tangent, (float3x3)ObjToWorld));
    }

    inline float3 ObjectToWorldDir(float3 dir)
    {
        return normalize(mul(dir, (float3x3)ObjToWorld));
    }

    inline float4 ObjectToWorldPos(in float4 pos)
    {
        return mul(pos, ObjToWorld);
    }

    inline float4 ObjectToViewPos(in float4 pos)
    {
        return mul(ObjectToWorldPos(pos), CAMERA_VIEW);
    }

    inline float4 ObjectPos(in float4 pos)
    {
        return pos;
    }

    inline float4 WorldToObjectPos(float4 pos4)
    {
        return mul(pos4, WORLDLOCAL_TRANSFROM);
    }
#endif

inline float3 AmbientColor()
{
	return AMBIENT_COLOR.xyz;
}

inline float3 WorldViewDir(float3 worldPos)
{
	float3 worldViewDir = CAMERA_WORLDPOSITION.xyz - worldPos.xyz;
	worldViewDir.xyz = normalize(worldViewDir.xyz);
	return worldViewDir;
}

inline float3 WorldReflectionVector(float3 worldPos, float3 worldNormal)
{
	float3 worldViewDir = CAMERA_WORLDPOSITION.xyz - worldPos.xyz;
	worldViewDir.xyz = normalize(worldViewDir.xyz);
	float3 reflectDir = reflect(-worldViewDir.xyz, worldNormal.xyz);
	return reflectDir;
}

inline float4 UniformNDC(float4 vertex)
{
	float4 scale = float4(
		1.0f,
		DEVICE_COORDINATE_Y_FLIP,
		1.0f,
		1.0f
	);
	float4 newVertex = scale*vertex;
	#if COMPILER_HLSL
		newVertex.z = 0.5 * (newVertex.z + newVertex.w);
	#endif
	#if COMPILER_VULKAN
		newVertex.z = 0.5 * (newVertex.z + newVertex.w);
		newVertex.y = -newVertex.y;
	#endif
	
	return  newVertex;
}

inline float GammaToLinearSpaceExact(float value)
{
    if (value <= 0.04045)
        return value / 12.92;
    else if (value < 1.0)
        return pow((value + 0.055)/1.055, 2.4);
    else
        return pow(value, 2.2);
}

inline float3 GammaToLinearSpace (float3 sRGB)
{
    return sRGB * (sRGB * (sRGB * 0.305306011 + 0.682171111) + 0.012522878);
}

inline float4 UniformInputColor(float4 color)
{
	#ifdef GammaSpace
		return color;
	#else
		return float4(GammaToLinearSpace(color.xyz), color.w);
	#endif
}

inline float4 ComputeScreenPos(float4 pos)
{
	float4 o = pos * 0.5f;
#if COMPILER_VULKAN
	o.xy = float2(o.x, -o.y) + o.ww;
#else
	o.xy = float2(o.x, o.y) + o.ww;
#endif
	o.y = 2.0 * o.w - o.y;
	o.zw = pos.zw;
	return o;
}

inline float2 TRANSFORM_TEX(float2 uv, float2 offset, float2 tilling)
{
    float2 newUV = uv * tilling + offset;
    return newUV;
}