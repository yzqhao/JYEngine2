#pragma once

#include "PipelineContex.h"
#include "Engine/ViewResolution.h"

NS_JYE_BEGIN

class ENGINE_API IRenderLoop
{
public:

	IRenderLoop(void) { }
	virtual ~IRenderLoop(void) { }

	virtual void Init(const Math::IntVec2& viewResolution) = 0;
	virtual int DoRender(PipelineContex& pipeContex) = 0;
	virtual void ResizeView(const ViewResolution& vr) = 0;

	virtual bool IsHDR() { return false; }
};

NS_JYE_END