
#DEF_PARAMS
_MainTex = { "Main Color", TEXTURE2D, "white" },
_BumpMap = {"Normal Map", TEXTURE2D, "bump"},
_MetallicGlossMap = {"Metallic Gloss", TEXTURE2D, "white"},
_Smoothness = {"Smoothness", FLOAT, "0.5" },
_Metallic = { "Metallic", FLOAT, "0.5" },
_EmissionMap = {"Emission Map",TEXTURE2D,"white"},
_EmissionColor = {"Emission Color", COLOR, "0.0,0.0,0.0,1.0"},
_Opacity = { "Opacity", FLOAT, "1.0" },
_CutOff = { "CutOff", FLOAT, "0.0" },

[Keywords(ALPHAPREMULTIPLY_ON, ALPHAPREMULTIPLY_OFF)]
_AlphaPreMultiply = {"Alpha Pre Multiply", FLOAT, "1.0"},
[Keywords(ALPHATEST_ON, ALPHATEST_OFF)]
_ALPHATEST_ON = {"Alpha Test On", FLOAT, "1.0"},

#END_PARAMS

#DEFTAG
ShaderName = "standardpbr"
RenderQueue = "Opaque"
#END

#DEF_PASSES ForwardBase
COLOR_MASK = { COLOR_RGBA }
ALPAH_MODE = { ALPAH_OFF }
DRAW_MODE = { CULL_FACE_OFF, DEPTH_MASK_ON, DEPTH_TEST_ON, DEPTH_FUNCTION_LEQUAL }
STENCIL_MODE = { STENCIL_OFF }
LIGHT_MODE = { FORWARDBASE }

CGPROGRAM
#pragma vertex vert
#pragma fragment frag
#pragma multi_compile_fwdbase
#pragma multi_compile ALPHAPREMULTIPLY_ON ALPHAPREMULTIPLY_OFF
#pragma multi_compile ALPHATEST_ON ALPHATEST_OFF

#define GammaSpace 1
#include "Common.inc"
#include "PBRLighting.inc"

struct appdata
{
    float4 vertex : POSITION;
    float3 normal : NORMAL;
    float3 binoraml : BINORMAL;
    float3 tangent : TANGENT;
    float2 uv : TEXCOORD0;
    ANI_ATTRIBUTE
};

struct v2f
{
    float2 uv : TEXCOORD0;
    float4 worldnormal : TEXCOORD1;
    float4 worldtangent : TEXCOORD2;
    float4 worldbinormal : TEXCOORD3;
    float4 screenPos : TEXCOORD5;
    float4 vertex : SV_POSITION;
};

Texture2D _MainTex;
SamplerState _MainSampler;
Texture2D _BumpMap;
SamplerState _BumpMapSampler;
Texture2D _MetallicGlossMap;
SamplerState _MetallicGlossMapSampler;
Texture2D _EmissionMap;
SamplerState _EmissionMapSampler;

float _Smoothness;
float _Metallic;
float4 _EmissionColor;
float _Opacity;
float _CutOff;

v2f vert(appdata v)
{
	COMPUTE_BONE_MATERIX(v);

    v2f o;
    float4 pos = ObjectToClipPos(v.vertex);
    o.vertex = UniformNDC(pos);
    o.uv = v.uv.xy;
    float3 worldpos = ObjectToWorldPos(v.vertex).xyz;
    o.screenPos = ComputeScreenPos(pos);
    o.worldnormal.xyz = ObjectToWorldNormal(v.normal).xyz;
    o.worldtangent.xyz = ObjectToWorldDir(v.tangent).xyz;
    o.worldbinormal.xyz = ObjectToWorldDir(v.binoraml).xyz;

    o.worldnormal.w = worldpos.x;
    o.worldtangent.w = worldpos.y;
    o.worldbinormal.w = worldpos.z;

    return o;
}

void frag(in v2f i, out float4 outColor : SV_Target0)
{
	// sample the texture
    float4 col = _MainTex.Sample(_MainSampler, i.uv);
    float2 mg = _MetallicGlossMap.Sample(_MetallicGlossMapSampler, i.uv).ra;
    float smoothness = mg.g * _Smoothness;
    float metallic = mg.r * _Metallic;
    float3 emissionMapColor = _EmissionMap.Sample(_EmissionMapSampler, i.uv).rgb;

    float3 normalTangent = _BumpMap.Sample(_BumpMapSampler, i.uv).rgb;
    normalTangent.xyz = normalTangent.xyz * 2.0 - 1.0;
    float3 normalWorld = normalTangent.x * i.worldtangent.xyz + normalTangent.y * i.worldbinormal.xyz + normalTangent.z * i.worldnormal.xyz;
    normalWorld.xyz = normalize(normalWorld.xyz);
    float3 worldPos = float3(i.worldnormal.w, i.worldtangent.w, i.worldbinormal.w);

    float3 iblColor = float3(0.0, 0.0, 0.0);
    float3 iblDiffuseColor = float3(0.0, 0.0, 0.0);

    SurfaceStandardInput input;
    input.albedo = col.rgb;
    input.metallic = metallic;
    input.smoothness = smoothness;
    input.worldNormal = normalWorld; 
    input.worldPos = worldPos;
    input.emission = AmbientColor() + _EmissionColor.rgb * emissionMapColor;;
    input.specularIbl = iblColor.rgb;
    input.diffuseIbl = iblDiffuseColor.rgb;
    input.alpha = col.a;
    input.atten = 1.0;  // TODO shadow

    outColor.xyz = PBR_LightCalc(input);
	#if ALPHATEST_ON
		outColor.w = 1.0;
		if (col.a - _CutOff <= 0) {discard; }
	#else
		#if ALPHAPREMULTIPLY_ON
			outColor.w = input.alpha *  _Opacity;
		#else
			outColor.w = 1.0;
		#endif
	#endif
}
ENDCG
#END_PASSES



#DEF_PASSES Depth
COLOR_MASK = { COLOR_RGBA }
ALPAH_MODE = { ALPAH_OFF }
DRAW_MODE = { CULL_FACE_OFF, DEPTH_MASK_ON, DEPTH_TEST_ON, DEPTH_FUNCTION_LESS }
STENCIL_MODE = { STENCIL_OFF }
LIGHT_MODE = { DEPTHPASS }

CGPROGRAM
#pragma vertex vert
#pragma fragment frag
#pragma multi_compile_skin

#include "PBRLighting.inc"

struct appdata
{
    float4 vertex : POSITION;
    float3 normal : NORMAL;
    ANI_ATTRIBUTE
};

struct v2f
{
    float4 vertex : SV_POSITION;
};

v2f vert(appdata v)
{
    COMPUTE_BONE_MATERIX(v);

    float4 clipPos = ObjectToClipPos(v.vertex);
    clipPos = ApplyShadowBias(clipPos);

    v2f o;
    o.vertex = UniformNDC(clipPos);
    return o;
}

void frag(in v2f i, out float4 mainColor : SV_Target0)
{
    mainColor = float4(0.0, 0.0, 0.0, 1.0);
}
ENDCG
#END_PASSES
