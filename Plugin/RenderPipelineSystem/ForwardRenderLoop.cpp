#include "ForwardRenderLoop.h"
#include "Engine/render/material/ShaderKeyWords.h"
#include "Engine/render/material/ApplyKeyWord.h"
#include "Engine/render/material/MaterialEntity.h"
#include "Engine/pipeline/RenderUntil.h"
#include "Engine/pipeline/RenderContext.h"

NS_JYE_BEGIN

ForwardRenderLoop::ForwardRenderLoop()
{

}

ForwardRenderLoop::~ForwardRenderLoop()
{

}

int ForwardRenderLoop::DoRender(PipelineContex& pipeContex)
{
	ShaderKeyWords keyWords;
	RenderUntil::ComputeHardwareKeyWords(&keyWords);
	RenderContext& renderContex = *pipeContex.rendercontex;
	ApplyKeyWord apply(renderContex.GetShaderKeyWords(), keyWords);
	RenderUntil::SetCameraParam(pipeContex);
	RenderUntil::SetSystemParam(pipeContex);
	RenderUntil::SetAmbientLightParam(pipeContex);

	RenderCommandData backgroundData;
	RenderCommandData translucentData;
	RenderCommandData overlayerData;
	RenderCommandData forwardOpaqueData;
	RenderCommandData deferredOpaqueData;

	RenderUntil::CollectRenderingData(pipeContex, true, backgroundData, translucentData, overlayerData, forwardOpaqueData, deferredOpaqueData);
	GenRenderPasses(backgroundData);
	GenRenderPasses(forwardOpaqueData);
	GenRenderPasses(translucentData);
	GenRenderPasses(overlayerData);
	// pass sort
	RenderUntil::SortLightingPassDataNonOpaque(backgroundData);
	RenderUntil::SortLightingPassDataOpaque(forwardOpaqueData);
	RenderUntil::SortLightingPassDataNonOpaque(translucentData);
	RenderUntil::SortLightingPassDataNonOpaque(overlayerData);

	RenderCommandData* pCmmand[4] = { &backgroundData, &forwardOpaqueData, &translucentData, &overlayerData };
	int drawCount = RenderUntil::DrawObjects2RT(pipeContex, pipeContex.pDrawRT, pipeContex.clearFlag, pipeContex.clearColor, pCmmand, 3);
	// posteffect
	drawCount += RenderUntil::DrawPostEffects(pipeContex, pipeContex.pDrawRT, pipeContex.pMainRT);

	// overlayer
	drawCount += RenderUntil::DrawObjects2RT(pipeContex, pipeContex.pMainRT, RHIDefine::CF_None, Math::FColor(0, 0, 0, 0), &pCmmand[3], 1);

	return drawCount;
}

void ForwardRenderLoop::GenRenderPasses(RenderCommandData& commandData)
{
	std::vector<RenderPassData>& renderPasses = commandData.renderPassData;
	std::vector<RenderMatData>& matDatas = commandData.matData;
	int commandCount = matDatas.size();

	if (commandCount > 0)
	{
		renderPasses.reserve(commandCount * 3);
		for (int i = 0; i < commandCount; i++)
		{
			RenderMatData& curRenderData = matDatas[i];
			const std::vector<ShaderPass>& passes = curRenderData.mat->GetPasses();

			int beginIndex = renderPasses.size() - 1;
			int validPassCount = 0;
			uint firstPassTag = FirstPass;
			ForwardLight* forwardLight = curRenderData.forwardLight;

			for (int passIndex = 0; passIndex < passes.size(); passIndex++)
			{
				const ShaderPass& curPass = passes[passIndex];
				RHIDefine::PassType passType = curPass.passType;

				if (passType == RHIDefine::PT_UNIVERSAL_POST_EFFECT)
				{
					passType = RHIDefine::PT_ALWAYS;
				}

				bool supportPass = (passType == RHIDefine::PT_ALWAYS || passType == RHIDefine::PT_FORWARDBASE);
				supportPass |= (passType == RHIDefine::PT_FORWARDADD
					&& forwardLight->addLightCount > 0);

				if (supportPass)
				{
					RenderPassData rpassData;
					rpassData.matDataIndex = i;
					rpassData.data = ((passIndex & PassMask) | (passType << PassTypeOffsetBits) |
						firstPassTag);
					validPassCount++;
					firstPassTag = 0;
					renderPasses.push_back(rpassData);
				}
			}

			if (validPassCount > 1)
			{
				for (int mulPassIndex = 0; mulPassIndex < renderPasses.size(); mulPassIndex++)
				{
					renderPasses[mulPassIndex].data |= MulPassTag;
				}
			}
		}
	}
}

NS_JYE_END