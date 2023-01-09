
#DEF_PARAMS
_MainTex = { "Main Color", TEXTURE2D, "white" },
#END_PARAMS

#DEFTAG
ShaderName = "FullScreenQuad"
RenderQueue = "Transparent"
#END

#DEF_PASSES Always
COLOR_MASK = { COLOR_RGBA }
ALPAH_MODE = { ALPAH_OFF }
DRAW_MODE = { CULL_FACE_BACK, DEPTH_MASK_ON, DEPTH_TEST_ON, DEPTH_FUNCTION_LESS }
STENCIL_MODE = { STENCIL_OFF }
LIGHT_MODE = { ALWAYS }

CGPROGRAM
#pragma vertex vert
#pragma fragment frag
#include "Common.inc"

struct appdata
{
	float2 vertex : POSITION;
	float2 uv : TEXCOORD0;
};

struct v2f
{
	float2 uv : TEXCOORD0;
	float4 vertex : SV_POSITION;
};

Texture2D _MainTex;
SamplerState _MainSampler;

v2f vert(appdata v)
{
	v2f o;
	float4 pos = float4(v.vertex, 0.0, 1.0);
	pos.w = 1.0;
	o.vertex = pos;
    //o.vertex = UniformNDC(o.vertex);
	o.uv = v.uv.xy;


	return o;
}

void frag(in v2f i, out float4 mainColor : SV_Target0)
{
	mainColor = _MainTex.Sample(_MainSampler,i.uv);
}
ENDCG
#END_PASSES
