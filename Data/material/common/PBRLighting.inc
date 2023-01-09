#pragma once

#include "Common.inc"
#include "Shadow.inc"

// https://zhuanlan.zhihu.com/p/137039291 shader可以参考这个文章

struct SurfaceStandardInput
{
	float3 albedo;
	half metallic; // 0 = non-metal 1 = metal
	half smoothness; // 0 = rough 1 = smooth
	half occlusion; // occlusion
	float3 emission;
	float3 specularIbl;
	float3 diffuseIbl;
	float3 worldNormal;
	float3 worldPos;
	float atten;
	float alpha;
};

struct LightStandardInput
{
	float3 lightColor;
	float3 viewDir;
	float3 lightDir;
	float3 normalDir;
	float atten;
	float3 emission;
	float3 specularIbl;
	float3 diffuseIbl;
	float3 albedo;
	half metallic;
	half smoothness;
	half occlusion;
	float alpha;
};

inline half OneMinusReflectivityFromMetallic(half metallic)
{
#if GammaSpace
    half3 DielectricSpec = half3(0.220916301, 0.220916301, 0.220916301);
#else
	half3 DielectricSpec = half3(0.04, 0.04, 0.04);
#endif
    half oneMinusDielectricSpec = 1.0 - DielectricSpec.r;
    return oneMinusDielectricSpec - metallic * oneMinusDielectricSpec;
}

inline half3 DiffuseAndSpecularFromMetallic (half3 albedo, half metallic, out half3 specColor, out half oneMinusReflectivity)
{
#if GammaSpace
    half3 DielectricSpec = half3(0.220916301, 0.220916301, 0.220916301);
#else
	half3 DielectricSpec = half3(0.04, 0.04, 0.04);
#endif
	specColor = DielectricSpec.rgb * (1.0 - metallic) + albedo.rgb * metallic;
    oneMinusReflectivity = OneMinusReflectivityFromMetallic(metallic);
    return albedo * oneMinusReflectivity;
}

inline float3 safeNormalize(float3 inVec)
{
    float dp3 = max(0.001f, dot(inVec, inVec));
    return inVec * rsqrt(dp3);
}

inline half Pow5 (half x)
{
    return x*x * x*x * x;
}

half DisneyDiffuse(half NdotV, half NdotL, half LdotH, half perceptualRoughness)
{
    half fd90 = 0.5 + 2 * LdotH * LdotH * perceptualRoughness;
    // Two schlick fresnel term
    half lightScatter   = (1 + (fd90 - 1) * Pow5(1 - NdotL));
    half viewScatter    = (1 + (fd90 - 1) * Pow5(1 - NdotV));

    return lightScatter * viewScatter;
}

inline float SmithJointGGXVisibilityTern(float NdotL, float NdotV, float roughness)
{
	float a = roughness;
    float lambdaV = NdotL * (NdotV * (1 - a) + a);
    float lambdaL = NdotV * (NdotL * (1 - a) + a);

	return 0.5f / (lambdaV + lambdaL + 1e-4f);
}

inline float GGXTerm (float NdotH, float roughness)
{
    float a2 = roughness * roughness;
    float d = (NdotH * a2 - NdotH) * NdotH + 1.0f; // 2 mad
    return INV_PI * a2 / (d * d + 1e-7f); // This function is not intended to be running on Mobile,
                                            // therefore epsilon is smaller than what can be represented by half
}

inline half3 FresnelTerm (half3 F0, half cosA)
{
    half t = Pow5 (1 - cosA);   // ala Schlick interpoliation
    return F0 + (1-F0) * t;
}

inline half3 FresnelLerp (half3 F0, half3 F90, half cosA)
{
    half t = Pow5 (1 - cosA);   // ala Schlick interpoliation
	return F0 * (1.0 - t) + F90 * t;
}

inline half3 PreMultiplyAlpha(half3 diffColor,half alpha,half oneMinusReflectivity,out half outModifiedAlpha)
{
	#if defined(ALPHAPREMULTIPLY_ON)
		diffColor *= alpha;
		outModifiedAlpha = 1 - oneMinusReflectivity + alpha * oneMinusReflectivity;
	#else
		outModifiedAlpha = alpha;
	#endif
	return diffColor;
}

// 最终计算光照
float3 PBR_Lighting(LightStandardInput input)
{
    half3 specColor = half3(0.0, 0.0, 0.0);
	half oneMinusReflectivity = 0.0;

	half3 albedo = DiffuseAndSpecularFromMetallic(input.albedo, input.metallic, specColor, oneMinusReflectivity);
	input.alpha = 0.0;
	albedo = PreMultiplyAlpha(albedo,input.alpha,oneMinusReflectivity, input.alpha);
	//input.alpha = outputAlpha;
	float perceptualRoughness = 1.0 - input.smoothness;
	float3 halfDir = safeNormalize(input.lightDir + input.viewDir);
    float dotNV = abs(dot(input.viewDir, input.normalDir));
	float dotNL = saturate(dot(input.normalDir, input.lightDir));
	float dotNH = saturate(dot(input.normalDir, halfDir));

	half dotLV = saturate(dot(input.lightDir, input.viewDir));
	half dotLH = saturate(dot(input.lightDir, halfDir));
	half diffuseTerm = DisneyDiffuse(dotNV, dotNL, dotLH, perceptualRoughness) * dotNL;
	
	float roughness = perceptualRoughness * perceptualRoughness;
	roughness = max(roughness, 0.002);
	float G = SmithJointGGXVisibilityTern(dotNL, dotNV, roughness);
	float D = GGXTerm(dotNH, roughness);

	float specularTerm = D * G * 3.14159265;

#if GammaSpace
	specularTerm = sqrt(max(1e-4f, specularTerm));
#endif
    specularTerm = max(0.0, specularTerm * dotNL);

	half surfaceReduction = 0.0;
#if GammaSpace
	surfaceReduction = 1.0-0.28*roughness*perceptualRoughness;
#else
	surfaceReduction = 1.0 / (roughness * roughness + 1.0);
#endif
    half grazingTerm = saturate(input.smoothness + (1-oneMinusReflectivity));

	float3 color = albedo * input.lightColor * diffuseTerm + specularTerm * input.lightColor * FresnelTerm(specColor, dotLH);
	color *= input.atten;
	color += input.emission;//* input.albedo;
	color += albedo * input.diffuseIbl;

	float3 specularEnv = surfaceReduction * FresnelLerp (specColor, grazingTerm.xxx, dotNV);
    color += specularEnv * input.specularIbl;

    return color;
}

#if PointLight
float3 PBR_LightCalc(SurfaceStandardInput input)
{
	float3 lightDir = LIGHT_POSITION.xyz - input.worldPos.xyz;
	float dis = sqrt(dot(lightDir.xyz, lightDir.xyz));
	float disAtten = clamp(dis * LIGHT_RANGE_INV, 0.0, 1.0);
	float attenation = (1.0 - disAtten) /  ( LIGHT_ATTENUATION.x + disAtten * LIGHT_ATTENUATION.y + disAtten * disAtten * LIGHT_ATTENUATION.z );
	float3 worldViewDir = CAMERA_WORLDPOSITION.xyz - input.worldPos.xyz;
	worldViewDir.xyz = normalize(worldViewDir.xyz);
	lightDir.xyz = normalize(lightDir.xyz);

	LightStandardInput lightParam;
	lightParam.albedo = input.albedo;
	lightParam.metallic = input.metallic;
	lightParam.smoothness = input.smoothness;
	lightParam.normalDir.xyz = input.worldNormal;
	lightParam.viewDir.xyz = worldViewDir.xyz;
	lightParam.lightDir.xyz = lightDir.xyz;
	lightParam.atten = attenation * input.atten;
	lightParam.lightColor = LIGHT_COLOR.xyz;
	lightParam.emission = input.emission;
	lightParam.specularIbl = input.specularIbl;
	lightParam.diffuseIbl = input.diffuseIbl;
	lightParam.alpha = input.alpha;
	
	return PBR_Lighting(lightParam);
}
#elif DirLight
float3 PBR_LightCalc(SurfaceStandardInput input)
{
	float3 lightDir = -LIGHT_GIVEN_DIRECTION.xyz;
	float atten = 1.0;
    float3 worldViewDir = CAMERA_WORLDPOSITION.xyz - input.worldPos.xyz;
	worldViewDir.xyz = normalize(worldViewDir.xyz);

	LightStandardInput lightParam;
	lightParam.albedo = input.albedo;
	lightParam.metallic = input.metallic;
	lightParam.smoothness = input.smoothness;
	lightParam.normalDir.xyz = input.worldNormal;
	lightParam.emission = input.emission.xyz;
	lightParam.specularIbl = input.specularIbl;
	lightParam.diffuseIbl = input.diffuseIbl;
	lightParam.viewDir.xyz = worldViewDir.xyz;
	lightParam.lightDir.xyz = lightDir.xyz;
	lightParam.atten = atten * input.atten;
	lightParam.lightColor = LIGHT_COLOR.xyz;
	lightParam.alpha = input.alpha;
	
	return PBR_Lighting(lightParam);
}
#elif SpotLight
float3 PBR_LightCalc(SurfaceStandardInput input)
{
	float3 lightDir = LIGHT_POSITION.xyz - input.worldPos.xyz;
	float dis = sqrt(dot(lightDir.xyz, lightDir.xyz));
	float disAtten = clamp(dis * LIGHT_RANGE_INV, 0.0, 1.0);
	float attenation = (1.0 - disAtten) /  ( LIGHT_ATTENUATION.x + disAtten * LIGHT_ATTENUATION.y + disAtten * disAtten * LIGHT_ATTENUATION.z );
	float attenAngle = 1.0;

	lightDir.xyz = normalize(lightDir.xyz);
	attenAngle = clamp( 1.0 - ( LIGHT_INNER_DIFF_INV.x - dot(lightDir.xyz, -LIGHT_GIVEN_DIRECTION.xyz) ) * LIGHT_INNER_DIFF_INV.y, 0.0, 1.0 );
	attenation *= attenAngle;

	float3 worldViewDir = CAMERA_WORLDPOSITION.xyz - input.worldPos.xyz;
	worldViewDir.xyz = normalize(worldViewDir.xyz);

	LightStandardInput lightParam;
	lightParam.albedo = input.albedo;
	lightParam.metallic = input.metallic;
	lightParam.smoothness = input.smoothness;
	lightParam.normalDir.xyz = input.worldNormal;
	lightParam.emission = input.emission.xyz;
	lightParam.specularIbl = input.specularIbl;
	lightParam.diffuseIbl = input.diffuseIbl;
	lightParam.viewDir.xyz = worldViewDir.xyz;
	lightParam.lightDir.xyz = lightDir.xyz;
	lightParam.atten = attenation * input.atten;
	lightParam.lightColor = LIGHT_COLOR.xyz;
	lightParam.alpha = input.alpha;
	
	return PBR_Lighting(lightParam);
}
#elif NoLight
float3 PBR_LightCalc(SurfaceStandardInput input)
{
    float3 worldViewDir = CAMERA_WORLDPOSITION.xyz - input.worldPos.xyz;
	worldViewDir.xyz = normalize(worldViewDir.xyz);

	LightStandardInput lightParam;
	lightParam.albedo = input.albedo;
	lightParam.metallic = input.metallic;
	lightParam.smoothness = input.smoothness;
	lightParam.emission = input.emission.xyz;
	lightParam.specularIbl = input.specularIbl;
	lightParam.diffuseIbl = input.diffuseIbl;
	lightParam.normalDir.xyz = input.worldNormal;
	lightParam.viewDir.xyz = worldViewDir;
	lightParam.lightDir.xyz = float3(0.0, 0.0, 1.0);
	lightParam.atten = 1.0;
	lightParam.lightColor = float3(0.0, 0.0, 0.0);
	lightParam.alpha = input.alpha;
	
	return PBR_Lighting(lightParam);
}
#endif