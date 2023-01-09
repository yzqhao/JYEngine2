
#DEF_PARAMS
_MainTex = { "Main Color", TEXTURE2D, "white" },
#END_PARAMS

#DEFTAG
ShaderName = "unlit"
RenderQueue = "Opaque"
#END

#DEF_PASSES Always
COLOR_MASK = { COLOR_RGBA }
ALPAH_MODE = { ALPAH_OFF }
DRAW_MODE = { CULL_FACE_OFF, DEPTH_MASK_ON, DEPTH_TEST_ON, DEPTH_FUNCTION_LEQUAL }
STENCIL_MODE = { STENCIL_OFF }
LIGHT_MODE = { ALWAYS }

CGPROGRAM
#pragma vertex vert
#pragma fragment frag
#include "Common.inc"

struct appdata
{
	float4 vertex : POSITION;
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
	float4 pos = ObjectToClipPos(v.vertex);
	o.vertex = UniformNDC(pos);
	o.uv = v.uv.xy;

	return o;
}

void frag(in v2f v, out float4 mainColor : SV_Target0)
{
	// sample the texture
	mainColor = _MainTex.Sample(_MainSampler, v.uv);
}
ENDCG
#END_PASSES
