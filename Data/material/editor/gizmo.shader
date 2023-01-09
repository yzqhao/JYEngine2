

#DEF_PARAMS
POINT_COLOR = { "Color ", VEC4, "1.0, 1.0, 1.0, 1.0" },
#END_PARAMS

#DEFTAG
ShaderName = "gizmo"
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
};

struct v2f
{
	float4 vertex : SV_POSITION;
};

float4 POINT_COLOR;


v2f vert(appdata v)
{
	v2f o;
	o.vertex = WorldToClipPos(v.vertex);
  o.vertex = UniformNDC(o.vertex);
	return o;
}

void frag(in v2f i, out float4 mainColor : SV_Target0)
{
    mainColor = POINT_COLOR;
}
ENDCG
#END_PASSES
