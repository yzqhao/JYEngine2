
#pragma once

#include "private/Define.h"
#include "Engine/pipeline/PipelineSystem.h"

NS_JYE_BEGIN

class RenderPipelineSystem : public PipelineSystem
{
public:
	RenderPipelineSystem();
    ~RenderPipelineSystem();

	virtual IRenderLoop* CreateRenderLoop(GraphicDefine::PiplineType passType);
	virtual void DeleteRenderLoop(IRenderLoop* pRenderLoop);

	
};

NS_JYE_END
