
#pragma once

#include "private/Define.h"
#include "Engine/pipeline/IRenderLoop.h"

NS_JYE_BEGIN

struct RenderCommandData;

class ForwardRenderLoop : public IRenderLoop
{
public:
	ForwardRenderLoop();
	virtual ~ForwardRenderLoop();

	virtual void Init(const Math::IntVec2& viewResolution) {}

	virtual void ResizeView(const ViewResolution& vr) {}

	virtual int DoRender(PipelineContex& pipeContex) override;

	void GenRenderPasses(RenderCommandData& commandData);
};

NS_JYE_END
