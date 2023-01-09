#pragma once

#include "Engine/private/Define.h"
#include "Math/IntVec2.h"
#include "Math/Vec2.h"
#include "Math/Vec3.h"
#include "Math/Vec4.h"
#include "RHI/RHIDefine.h"
#include "Math/IntVec4.h"
#include "ParserStateType.h"

NS_JYE_BEGIN

class ENGINE_API RenderState
{
public:
	ParserBoolean m_isDepthTest;
	ParserBoolean m_isDepthMask;
	ParserFunction m_DepthFunction;
	ParserCullFaceMode m_isCullFace;

	ParserBoolean m_isAlphaEnable;
	ParserBoolean m_isAlphaTest;
	ParserBlend m_Src;
	ParserBlend m_Des;
	ParserBlend m_SrcA;
	ParserBlend m_DesA;

	ParserColorMask m_ColorMask;

	ParserBoolean m_isStencil;
	ParserBitMask m_StencilMask;

	ParserPolygonMode m_PolygonMode;
	ParserUint m_PointSize;
	ParserUint m_LineWidth;

	ParserUint m_ScissorX;
	ParserUint m_ScissorY;
	ParserUint m_ScissorZ;
	ParserUint m_ScissorW;
	Math::IntVec4 m_ScissorRect;

	uint m_RenderQueue;
	uint m_uHashCode;

	uint64 m_RenderStateFlag;
	uint64 m_StencilStateFlag;

	std::vector<std::pair<BaseStateType*, RHIDefine::ParameterSlot>> m_comEffectByVar;

	FORCEINLINE uint64 GetRenderStateFlag() const
	{
		return m_RenderStateFlag;
	}
	FORCEINLINE uint64 GetStencilStateFlag() const
	{
		return m_StencilStateFlag;
	}
	FORCEINLINE const Math::IntVec4& GetScissor()
	{
		m_ScissorRect.x = m_ScissorX.value;
		m_ScissorRect.y = m_ScissorY.value;
		m_ScissorRect.z = m_ScissorZ.value;
		m_ScissorRect.w = m_ScissorW.value;
		return m_ScissorRect;
	}

	void ApplyStateSetting();
};

NS_JYE_END