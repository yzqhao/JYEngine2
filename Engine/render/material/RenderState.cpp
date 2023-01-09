#include "RenderState.h"
#include "RHI/DynamicRHI.h"

NS_JYE_BEGIN

void RenderState::ApplyStateSetting()
{
	m_RenderStateFlag = RHIGetRenderState(this);
	//m_StencilStateFlag = RHIGetStencilState(this);
}

NS_JYE_END
