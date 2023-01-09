#pragma once

#include "private/Define.h"
#include "Engine/pipeline/IRenderLoop.h"

NS_JYE_BEGIN

class Scene;
class LightComponent;

class ShadowRenderLoop : public IRenderLoop
{
public:
	ShadowRenderLoop(void);
	virtual ~ShadowRenderLoop(void);

	virtual void Init(const Math::IntVec2& viewResolution)
	{

	}

	virtual void ResizeView(const ViewResolution& vr)
	{

	}

	virtual int DoRender(PipelineContex& pipeContex);

};

NS_JYE_END
