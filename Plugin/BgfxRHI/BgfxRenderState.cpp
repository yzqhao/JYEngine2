#include "BgfxRHI.h"
#include "bgfx/bgfx.h"
#include "Engine/render/material/RenderState.h"

NS_JYE_BEGIN

static uint64 _AlphaBlendFunction(RHIDefine::Blend input)
{
	switch (input)
	{
	case RHIDefine::BL_ONE:					return BGFX_STATE_BLEND_ONE;
	case RHIDefine::BL_ZERO:				return BGFX_STATE_BLEND_ZERO;
	case RHIDefine::BL_SRC_COLOR:			return BGFX_STATE_BLEND_SRC_COLOR;
	case RHIDefine::BL_ONE_MINUS_SRC_COLOR:	return BGFX_STATE_BLEND_INV_SRC_COLOR;
	case RHIDefine::BL_SRC_ALPHA:			return BGFX_STATE_BLEND_SRC_ALPHA;
	case RHIDefine::BL_ONE_MINUS_SRC_ALPHA:	return BGFX_STATE_BLEND_INV_SRC_ALPHA;
	case RHIDefine::BL_DST_ALPHA:			return BGFX_STATE_BLEND_DST_ALPHA;
	case RHIDefine::BL_ONE_MINUS_DST_ALPHA:	return BGFX_STATE_BLEND_INV_DST_ALPHA;
	case RHIDefine::BL_DST_COLOR:			return BGFX_STATE_BLEND_DST_COLOR;
	case RHIDefine::BL_ONE_MINUS_DST_COLOR:	return BGFX_STATE_BLEND_INV_DST_COLOR;
	}
	return BGFX_STATE_BLEND_ONE;
}

uint64 BgfxRHI::GetRenderState(const RenderState* rs)
{
	uint64 stateFlag = 0ULL;

	if (RHIDefine::Boolean::MB_TRUE == rs->m_isDepthMask.value)
	{
		stateFlag |= BGFX_STATE_WRITE_Z;
	}
	// depth function
	if (RHIDefine::Boolean::MB_TRUE == rs->m_isDepthTest.value)
	{
		RHIDefine::Function depthFunc = rs->m_DepthFunction.value;
		switch (depthFunc)
		{
		case RHIDefine::Function::FN_LESS:
			stateFlag |= BGFX_STATE_DEPTH_TEST_LESS;
			break;
		case RHIDefine::Function::FN_EQUAL:
			stateFlag |= BGFX_STATE_DEPTH_TEST_EQUAL;
			break;
		case RHIDefine::Function::FN_LEQUAL:
			stateFlag |= BGFX_STATE_DEPTH_TEST_LEQUAL;
			break;
		case RHIDefine::Function::FN_GREATER:
			stateFlag |= BGFX_STATE_DEPTH_TEST_GREATER;
			break;
		case RHIDefine::Function::FN_NOTEQUAL:
			stateFlag |= BGFX_STATE_DEPTH_TEST_NOTEQUAL;
			break;
		case RHIDefine::Function::FN_GEQUAL:
			stateFlag |= BGFX_STATE_DEPTH_TEST_GEQUAL;
			break;
		case RHIDefine::Function::FN_ALWAYS:
			stateFlag |= BGFX_STATE_DEPTH_TEST_ALWAYS;
			break;
		case RHIDefine::Function::FN_NEVER:
			stateFlag |= BGFX_STATE_DEPTH_TEST_NEVER;
			break;
		case  RHIDefine::Function::FN_NULL:
			break;
		default:
			JYERROR("Unknown Depth test function");
		}
	}
	// cullface flag
	{
		RHIDefine::RasterizerCullMode cullface_state = RHIDefine::CM_CCW;
		RHIDefine::CullFaceMode cull_mode = rs->m_isCullFace.value;
		switch (cull_mode)
		{
		case RHIDefine::CullFaceMode::CFM_FRONT:
			stateFlag |= cullface_state == RHIDefine::CM_CW ? BGFX_STATE_CULL_CW : BGFX_STATE_CULL_CCW;
			break;
		case RHIDefine::CullFaceMode::CFM_BACK:
			stateFlag |= cullface_state == RHIDefine::CM_CCW ? BGFX_STATE_CULL_CCW : BGFX_STATE_CULL_CW;
			break;
		case RHIDefine::CullFaceMode::CFM_OFF:
		case RHIDefine::CullFaceMode::CFM_NULL:
			break;
		default:
			JYERROR("Unknown CullFace mode");
		}
	}
	if (GetDeviceCoordinateYFlip(m_CacheInfo.cacheRTHandle) < 0)
	{
		uint64 cullface_state = 0ULL;
		if (stateFlag & BGFX_STATE_CULL_CW)
		{
			cullface_state = BGFX_STATE_CULL_CCW;
		}
		else if (stateFlag & BGFX_STATE_CULL_CCW)
		{
			cullface_state = BGFX_STATE_CULL_CW;
		}
		stateFlag = stateFlag & (~BGFX_STATE_CULL_MASK);
		stateFlag = stateFlag | cullface_state;
	}
	// AlphaTest
	{
		if (RHIDefine::Boolean::MB_TRUE == rs->m_isAlphaTest.value)
		{
			//stateFlag |= BGFX_STATE_ALPHA_REF();
			JYERROR("AlphaTest currently not supported in bgfx");
		}
	}
	//alpha blend function
	{
		if (RHIDefine::Boolean::MB_TRUE == rs->m_isAlphaEnable.value)
		{
			RHIDefine::Blend srcRGB = rs->m_Src.value;
			RHIDefine::Blend desRGB = rs->m_Des.value;
			RHIDefine::Blend srcA = rs->m_SrcA.value;
			RHIDefine::Blend desA = rs->m_DesA.value;

			stateFlag |= BGFX_STATE_BLEND_FUNC_SEPARATE(_AlphaBlendFunction(srcRGB),
				_AlphaBlendFunction(desRGB),
				_AlphaBlendFunction(srcA),
				_AlphaBlendFunction(desA));
		}
	}
	//color mask
	{
		bool mask_R = (RHIDefine::ColorMask::CM_COLOR_R & rs->m_ColorMask.value);
		bool mask_G = (RHIDefine::ColorMask::CM_COLOR_G & rs->m_ColorMask.value);
		bool mask_B = (RHIDefine::ColorMask::CM_COLOR_B & rs->m_ColorMask.value);
		bool mask_A = (RHIDefine::ColorMask::CM_COLOR_A & rs->m_ColorMask.value);
		stateFlag |= (mask_R ? BGFX_STATE_WRITE_R : 0);
		stateFlag |= (mask_G ? BGFX_STATE_WRITE_G : 0);
		stateFlag |= (mask_B ? BGFX_STATE_WRITE_B : 0);
		stateFlag |= (mask_A ? BGFX_STATE_WRITE_A : 0);
	}
	//polygon mode
	{
		RHIDefine::PolygonMode pm = rs->m_PolygonMode.value;
		switch (pm)
		{
		case RHIDefine::PM_POINT:
			stateFlag |= BGFX_STATE_PT_POINTS;
			break;
		case RHIDefine::PM_LINE:
			stateFlag |= BGFX_STATE_PT_LINES;
			break;
		case RHIDefine::PM_TRIANGLE:
			//	stateFlag |= BGFX_STATE_PT_TRISTRIP;
			break;
		case RHIDefine::PM_NULL:
		default:
			break;
		}
	}
	//pointSize
	{
		uint ptsz = rs->m_PointSize.value;
		stateFlag |= BGFX_STATE_POINT_SIZE(ptsz);
	}

	return stateFlag;
}

NS_JYE_END