

#DEF_PARAMS
TEXTURE_DIFFUSE = { "main Color", TEXTURE2D, "white" },
_SCISSORX = {"scissor x",FLOAT,"0.0"},
_SCISSORY = {"scissor y",FLOAT,"0.0"},
_SCISSORZ = {"scissor z",FLOAT,"0.0"},
_SCISSORW = {"scissor w",FLOAT,"0.0"},
#END_PARAMS

#DEFTAG
ShaderName = "editorui"
RenderQueue = "Background"
#END

#DEF_PASSES Always
COLOR_MASK = { COLOR_RGBA }
ALPAH_MODE = { ALPAH_BLEND, SRC_ALPHA, ONE_MINUS_SRC_ALPHA, ONE, ONE }
DRAW_MODE = { CULL_FACE_OFF, DEPTH_MASK_OFF, DEPTH_TEST_OFF, DEPTH_FUNCTION_LESS }
STENCIL_MODE = {STENCIL_OFF}
LIGHT_MODE = { FORWARDBASE }
SCISSOR_MODE = { "%_SCISSORX", "%_SCISSORY", "%_SCISSORZ", "%_SCISSORW" }

CGPROGRAM
#pragma vertex vert
#pragma fragment frag
#include "Common.inc"

struct appdata   
{
	float2 vertex : POSITION;
	float2 uv : TEXCOORD0;
	float4 color : COLOR0;
};

struct v2f
{
	float4 vertex : SV_POSITION;
	float2 uv : TEXCOORD0;
	float4 color : COLOR0;
};

Texture2D TEXTURE_DIFFUSE;
SamplerState TEXTURE_DIFFUSE_Sampler;

v2f vert(appdata v)
{
	v2f o;
	float4 pos;
	pos.z = 0.0;
	pos.w = 1.0;
	pos.xy = v.vertex;
	o.vertex = mul(CAMERA_PROJECTION, pos);
	o.vertex = UniformNDC(o.vertex);
	o.uv = v.uv;
	
	o.color = v.color;
	return o;
}

void frag(in v2f i, out float4 mainColor : SV_Target0)
{
	mainColor = i.color*TEXTURE_DIFFUSE.Sample(TEXTURE_DIFFUSE_Sampler, i.uv);
}
ENDCG
#END_PASSES
