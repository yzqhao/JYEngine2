#include "ShadowRenderLoop.h"
#include "Engine/render/material/ShaderKeyWords.h"
#include "Engine/render/material/ApplyKeyWord.h"
#include "Engine/render/material/MaterialEntity.h"
#include "Engine/pipeline/RenderUntil.h"
#include "Engine/pipeline/RenderContext.h"

NS_JYE_BEGIN

ShadowRenderLoop::ShadowRenderLoop(void)
	: IRenderLoop()
{

}

ShadowRenderLoop::~ShadowRenderLoop(void)
{

}

int ShadowRenderLoop::DoRender(PipelineContex& pipeContex)
{
	ShaderKeyWords keyWords;
	RenderUntil::ComputeHardwareKeyWords(&keyWords);
	RenderContext& renderContex = *pipeContex.rendercontex;
	ApplyKeyWord apply(renderContex.GetShaderKeyWords(), keyWords);
	RenderUntil::SetCameraParam(pipeContex);
	RenderUntil::SetSystemParam(pipeContex);
	
	RenderCommandData shadowData;
	RenderUntil::CollectShadowPassData(pipeContex, shadowData);
	int depthPasses[] = { RHIDefine::PT_DEPTHPASS };
	RenderUntil::GenRenderPasses(shadowData, depthPasses, 1);
	RenderUntil::SortShadowPassData(shadowData);

	RenderCommandData * pShadowCommand = &shadowData;
	int drawCount = RenderUntil::DrawObjects2RT(pipeContex, pipeContex.pMainRT, pipeContex.clearFlag, pipeContex.clearColor, &pShadowCommand);
	
	return drawCount;
}
	
NS_JYE_END