

#DEF_PARAMS
TEXTURE_DIFFUSE = { "main Color", TEXTURE2D, "white" },
#END_PARAMS

#DEFTAG
ShaderName = "quadtexture"
RenderQueue = "Transparent"
#END

#DEF_PASSES ForwardBase
COLOR_MASK = { COLOR_RGBA }
ALPAH_MODE = { ALPAH_BLEND, SRC_ALPHA, ONE_MINUS_SRC_ALPHA , SRC_ALPHA, ONE }
DRAW_MODE = { CULL_FACE_OFF, DEPTH_MASK_OFF, DEPTH_TEST_ON, DEPTH_FUNCTION_LESS } 
STENCIL_MODE = {STENCIL_OFF}
LIGHT_MODE = { FORWARDBASE }

CGPROGRAM
#pragma vertex vert
#pragma fragment frag
#include "Common.inc"

struct appdata   
{
	float4 vertex : POSITION;
	float2 uv1 : TEXCOORD0;
	float2 uv2 : TEXCOORD1;
};

struct v2f
{
	float4 vertex : SV_POSITION;
	float2 uv : TEXCOORD0;
};

Texture2D TEXTURE_DIFFUSE;
SamplerState TEXTURE_DIFFUSE_Sampler;

v2f vert(appdata v)
{
	v2f o;
	float4 pos = mul(v.vertex, mul(LOCALWORLD_TRANSFORM, CAMERA_VIEW));
	o.vertex.w = pos.w;
	float3 vert;
	vert.z = 0;
	vert.xy = v.uv2;
	o.vertex.xyz = pos.xyz + vert;
	o.vertex = mul(o.vertex, CAMERA_PROJECTION);
	o.vertex = UniformNDC(o.vertex);  
	o.uv = v.uv1.xy;
	return o;
}

void frag(in v2f i, out float4 mainColor : SV_Target0)
{
	mainColor = TEXTURE_DIFFUSE.Sample(TEXTURE_DIFFUSE_Sampler, i.uv);
}
ENDCG
#END_PASSES
