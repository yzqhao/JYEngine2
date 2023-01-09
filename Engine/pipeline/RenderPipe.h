#pragma once

#include "PipelineContex.h"
#include "Engine/GraphicDefine.h"
#include "Math/Color.h"
#include "IRenderLoop.h"
#include "Core/Object.h"

NS_JYE_BEGIN

class CameraComponent;
class RenderTargetEntity;
class Scene;
class IWindow;
struct ViewResolution;

class ENGINE_API RenderPipe : public Object
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	RenderPipe();
	virtual ~RenderPipe();

	int DoRender(PipelineContex& renderData);
	int Render(uint64 masku, int seq, bool scull, RHIDefine::ClearFlag cf, const Math::FLinearColor& cc, Scene* sce, RenderContext& con);

	void SetPipelinePass(GraphicDefine::PiplineType pp);
	void SetResolution(const Math::IntVec2& resolution);
	void OnResizeView(const ViewResolution& vr);
	bool AttachRenderTarget(RenderTargetEntity* rt);
	void DetachRenderTarget();
	void UpdateRenderTarget(IWindow* win);

	RenderTargetEntity* GetPingPongRT(uint index);

	RenderObjectEntity* GetCopyRenderObj();
	MaterialEntity* GetCopyMaterial();

	FORCEINLINE RHIDefine::PixelFormat GetPostRTFormat();
	FORCEINLINE RenderTargetEntity* GetMainRenderTarget();
	FORCEINLINE const Math::IntVec2& GetResolution() const;
	FORCEINLINE bool RenderPipe::isOffScreenRender() const;
	FORCEINLINE RenderTargetEntity* RenderPipe::GetAttachedRenderTarget();
	FORCEINLINE GraphicDefine::PiplineType RenderPipe::GetPipelinePass() const;
	FORCEINLINE const IRenderLoop* RenderPipe::GetRenderLoop() const;
	FORCEINLINE void SetHostCamera(CameraComponent* came);

private:
	RenderTargetEntity* _GetCurrentRenderTarget(Scene* scene);

	RenderTargetEntity* GetTempMainRT();
	RenderTargetEntity* SelectRenderTarget(bool hasEffect, Scene* sce);

	FORCEINLINE RenderTargetEntity* _GetAttachedRenderTarget();

	RenderTargetEntity* m_pAttacedRenderTarget;
	RenderTargetEntity* m_pMainRenderTarget;
	RenderTargetEntity* m_pPingPongRT[2];
	RenderObjectEntity* m_pCopyRO;
	RenderTargetEntity* m_pMainTmpRT;
	MaterialEntity* m_pCopyMat;

	IRenderLoop* m_renderLoop;
	GraphicDefine::PiplineType m_ePipelineType;
	Math::IntVec2 m_Resolution;
	CameraComponent* m_rpHostCamera;
	CameraComponent* m_rpCamera;
};

FORCEINLINE RHIDefine::PixelFormat RenderPipe::GetPostRTFormat()
{
	if (m_renderLoop)
	{
		return m_renderLoop->IsHDR() ? RHIDefine::PixelFormat::PF_RG11B10FLOAT : RHIDefine::PixelFormat::PF_R8G8B8A8;
	}
	else
	{
		return RHIDefine::PixelFormat::PF_R8G8B8A8;
	}
}

FORCEINLINE RenderTargetEntity* RenderPipe::GetMainRenderTarget()
{
	return m_pMainRenderTarget;
}

FORCEINLINE RenderTargetEntity* RenderPipe::_GetAttachedRenderTarget()
{
	return m_pAttacedRenderTarget;
}

FORCEINLINE const Math::IntVec2& RenderPipe::GetResolution() const
{
	return m_Resolution;
}

FORCEINLINE bool RenderPipe::isOffScreenRender() const
{
	return NULL != m_pAttacedRenderTarget;
}

FORCEINLINE RenderTargetEntity* RenderPipe::GetAttachedRenderTarget()
{
	return m_pAttacedRenderTarget;
}

FORCEINLINE GraphicDefine::PiplineType RenderPipe::GetPipelinePass() const
{
	return m_ePipelineType;
}

FORCEINLINE const IRenderLoop* RenderPipe::GetRenderLoop() const
{
	return m_renderLoop;
}

FORCEINLINE void RenderPipe::SetHostCamera(CameraComponent* came)
{
	m_rpHostCamera = came;
}

NS_JYE_END