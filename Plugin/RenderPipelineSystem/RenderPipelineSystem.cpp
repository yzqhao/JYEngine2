
#include "RenderPipelineSystem.h"
#include "ForwardRenderLoop.h"
#include "ShadowRenderLoop.h"


NS_JYE_BEGIN

RenderPipelineSystem::RenderPipelineSystem(void)
	: PipelineSystem()
{
}

RenderPipelineSystem::~RenderPipelineSystem(void)
{
}

IRenderLoop* RenderPipelineSystem::CreateRenderLoop(GraphicDefine::PiplineType passType)
{
	if (passType == GraphicDefine::PP_FORWARD_LIGHTING)
	{
		return _NEW ForwardRenderLoop();
	}
	else if (passType == GraphicDefine::PP_DEFERRED_LIGHTING)
	{
		//return _NEW DeferredRenderLoop();
	}
	else if (passType == GraphicDefine::PP_DEPTH)
	{
		return _NEW ShadowRenderLoop();
	}
	else if (passType == GraphicDefine::PP_UI)
	{
	}

	return nullptr;
}

void RenderPipelineSystem::DeleteRenderLoop(IRenderLoop* pRenderLoop)
{
	SAFE_DELETE(pRenderLoop);
}

NS_JYE_END
