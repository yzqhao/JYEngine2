#include "RenderPipe.h"
#include "IRenderLoop.h"
#include "PipelineSystem.h"
#include "Core/Configure.h"
#include "Engine/render/RenderTargetEntity.h"
#include "Engine/render/RenderObjectEntity.h"
#include "Engine/resource/RenderObjectMetadata.h"
#include "Engine/resource/RenderTargetMetadata.h"
#include "Engine/resource/TextureMetadata.h"
#include "Engine/resource/GeneralMetadata.h"
#include "Engine/resource/ShapeBufferUtility.h"
#include "Core/Interface/IWindow.h"
#include "Engine/object/Scene.h"
#include "Engine/queue/IRenderQueue.h"
#include "Engine/queue/IlightQueue.h"

NS_JYE_BEGIN

IMPLEMENT_RTTI(RenderPipe, Object);
BEGIN_ADD_PROPERTY(RenderPipe, Object);
REGISTER_PROPERTY(m_pAttacedRenderTarget, m_pAttacedRenderTarget, Property::F_SAVE_LOAD_CLONE)
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(RenderPipe)
IMPLEMENT_INITIAL_END

static RenderTargetEntity* CreateRenderTarget(const Math::IntVec2& resolution, RHIDefine::PixelFormat format, bool hasDepthStencil)
{
	RenderTargetEntity* pRenderTarget = _NEW RenderTargetEntity;

	pRenderTarget->PushMetadata(
		RenderTargetMetadata(
			RHIDefine::RT_RENDER_TARGET_2D,
			Math::IntVec4(0, 0, resolution.x, resolution.y),
			resolution,
			Math::Vec2(1.0f, 1.0f)
		));

	TextureEntity* tex = pRenderTarget->MakeTextureAttachment(RHIDefine::TA_COLOR_0);
	tex->PushMetadata(
		TextureRenderMetadata(
			resolution,
			RHIDefine::TextureType::TEXTURE_2D,
			RHIDefine::TextureUseage::TU_STATIC,
			format,
			1,
			RHIDefine::TextureWarp::TW_CLAMP_TO_EDGE,
			RHIDefine::TextureWarp::TW_CLAMP_TO_EDGE,
			RHIDefine::TextureFilter::TF_NEAREST,
			RHIDefine::TextureFilter::TF_NEAREST));

	if (hasDepthStencil)
	{
		TextureEntity* depTexEnt = pRenderTarget->MakeTextureAttachment(RHIDefine::TA_DEPTH_STENCIL);
		depTexEnt->PushMetadata(DepthRenderBufferMetadata(
			resolution,
			RHIDefine::PixelFormat::PF_DEPTH24_STENCIL8
		));
	}

	pRenderTarget->CreateResource();

	return pRenderTarget;
}

RenderPipe::RenderPipe()
	: m_renderLoop(nullptr)
	, m_ePipelineType(GraphicDefine::PP_NONE)
	, m_pAttacedRenderTarget(nullptr)
	, m_pMainRenderTarget(nullptr)
	, m_pMainTmpRT(nullptr)
	, m_pCopyRO(nullptr)
	, m_pCopyMat(nullptr)
	, m_rpCamera(nullptr)
{
	m_pPingPongRT[0] = nullptr;
	m_pPingPongRT[1] = nullptr;
}

RenderPipe::~RenderPipe()
{
	SAFE_DELETE(m_pCopyRO);
	SAFE_DELETE(m_pCopyMat);
	SAFE_DELETE(m_pPingPongRT[0]);
	SAFE_DELETE(m_pPingPongRT[1]);

	PipelineSystem::Instance()->DeleteRenderLoop(m_renderLoop);
}

int RenderPipe::DoRender(PipelineContex& renderData)
{
	int drawCount = 0;
	if (m_renderLoop)
	{
		drawCount = m_renderLoop->DoRender(renderData);
	}
	return drawCount;
}

int RenderPipe::Render(uint64 masku, int seq, bool scull, RHIDefine::ClearFlag cf, const Math::FLinearColor& cc, Scene* sce, RenderContext& con)
{
	sce->GetRenderQueue()->OnCulling(masku, seq, scull, m_ePipelineType, m_rpHostCamera);//裁剪可见物体
	sce->GeLightQueue()->OnCulling(masku, seq, scull, m_ePipelineType, m_rpHostCamera);//裁剪可见灯光

	bool hasPostEffect = false;
	RenderTargetEntity* selctRT = SelectRenderTarget(hasPostEffect, sce);

	// keep the size of rendertarget the same as render pipeline
	if (m_Resolution != selctRT->GetResolution())
	{
		selctRT->Resizeview(m_Resolution);
	}

	m_rpCamera = m_rpHostCamera;
	PipelineContex pipeContex;
	pipeContex.clearFlag = cf;
	pipeContex.clearColor = cc;
	pipeContex.scene = sce;
	pipeContex.rendercontex = &con;
	pipeContex.resolution = m_Resolution;
	pipeContex.camera = m_rpCamera;
	pipeContex.hostCamera = m_rpHostCamera;
	pipeContex.visableLight = &(sce->GeLightQueue()->GetVisableLight());
	pipeContex.visableRender = &(sce->GetRenderQueue()->GetVisableRender());
	pipeContex.pCopyMat = GetCopyMaterial();
	pipeContex.pCopyRO = GetCopyRenderObj();
	pipeContex.pDrawRT = selctRT;
	pipeContex.pMainRT = _GetCurrentRenderTarget(sce);
	pipeContex.pPingPongRT[0] = hasPostEffect ? GetPingPongRT(0) : nullptr;
	pipeContex.pPingPongRT[1] = hasPostEffect ? GetPingPongRT(1) : nullptr;

	uint count = DoRender(pipeContex);

	return count;
}

void RenderPipe::SetPipelinePass(GraphicDefine::PiplineType pp)
{
	if (m_ePipelineType != pp)
	{
		if (m_renderLoop)
		{
			PipelineSystem::Instance()->DeleteRenderLoop(m_renderLoop);
			m_renderLoop = NULL;
		}

		if (pp != GraphicDefine::PP_NONE)
		{
			m_renderLoop = PipelineSystem::Instance()->CreateRenderLoop(pp);
			m_renderLoop->Init(m_Resolution);
		}

		m_ePipelineType = pp;
	}
}

void RenderPipe::SetResolution(const Math::IntVec2& resolution)
{
	m_Resolution = resolution;
	OnResizeView(resolution);
}

void RenderPipe::OnResizeView(const ViewResolution& vr)
{
	m_Resolution = vr.m_Resolution;

	if (m_pMainRenderTarget)
	{
		m_pMainRenderTarget->Resizeview(vr);
	}
	else
	{
		JYERROR("RenderPipe::OnResizeView main target null");
	}

	if (m_pAttacedRenderTarget)
	{
		m_pAttacedRenderTarget->Resizeview(vr);
	}

	if (m_pMainTmpRT)
	{
		m_pMainTmpRT->Resizeview(vr);
	}

	if (m_renderLoop)
	{
		m_renderLoop->ResizeView(vr);
	}

	if (m_pPingPongRT[0])
	{
		m_pPingPongRT[0]->Resizeview(vr);
	}

	if (m_pPingPongRT[1])
	{
		m_pPingPongRT[1]->Resizeview(vr);
	}
}

bool RenderPipe::AttachRenderTarget(RenderTargetEntity* rt)
{
	if (rt && rt->isReady())
	{
		m_pAttacedRenderTarget = rt;
		const auto& rt_res = rt->GetResolution();
		if (m_Resolution != rt_res)
		{
			JYWARNING(
				"resolution miss match between render pipe line and render target. "
				"size of pipeline is %d %d and fbo is %d %d",
				m_Resolution.x,
				m_Resolution.y,
				rt_res.x,
				rt_res.y
			);

			m_pAttacedRenderTarget->Resizeview(m_Resolution);
		}
		return true;
	}
	JYERROR("accept only loaded render target");
	return false;
}

void RenderPipe::DetachRenderTarget()
{
	SAFE_DELETE(m_pAttacedRenderTarget);
}

void RenderPipe::UpdateRenderTarget(IWindow* win)
{
	SAFE_DELETE(m_pMainRenderTarget);

	int wWidth, wHeight;
	win->GetWindowSize(&wWidth, &wHeight);
	RenderTargetEntity* _newRT = _NEW RenderTargetEntity;

	// Create render target for current render pipe
	void* nativeWin = NULL;
	RHIDefine::TargetType rtType;
	if (NULL != win && win->IsMainWindow())
	{
		// case 1: render to main window, the bgfx will use null to mark main window.
		nativeWin = (void*)NULL;
		rtType = RHIDefine::RT_RENDER_TARGET_MAIN;
	}
	else if (NULL != win)
	{
		// case 2: render to sub window
		nativeWin = win->GetNativeWindow();
		rtType = RHIDefine::RT_RENDER_TARGET_MAIN;
	}
	else
	{
		// case 3: render to off screen rt
		nativeWin = (void*)nullhandle;
		rtType = RHIDefine::RT_RENDER_TARGET_2D;
	}
	_newRT->PushMetadata(
		RenderTargetMetadata(
			rtType,
			Math::IntVec4(0, 0, m_Resolution.x, m_Resolution.y),
			m_Resolution,
			Math::Vec2(1.0f, 1.0f),
			false,
			nativeWin));
	_newRT->CreateResource();
	m_pMainRenderTarget = _newRT;
}

RenderTargetEntity* RenderPipe::GetPingPongRT(uint index)
{
	if (m_pPingPongRT[0] == NULL)
	{
		m_pPingPongRT[0] = CreateRenderTarget(m_Resolution, GetPostRTFormat(), false);
		m_pPingPongRT[1] = CreateRenderTarget(m_Resolution, GetPostRTFormat(), false);
	}

	if (index < 2)
	{
		return m_pPingPongRT[index];
	}
	else
	{
		return nullptr;
	}
}

RenderObjectEntity* RenderPipe::GetCopyRenderObj()
{
	if (m_pCopyRO == NULL)
	{
		m_pCopyRO = _NEW RenderObjectEntity();
		m_pCopyRO->PushMetadata(
			RenderObjectMeshMetadate(
				RHIDefine::RM_TRIANGLES,
				QuadVertexMetadata(RHIDefine::MU_STATIC, true, false),
				QuadIndicesMetadata(RHIDefine::MU_STATIC))
		);
		m_pCopyRO->CreateResource();
	}

	return m_pCopyRO;
}

MaterialEntity* RenderPipe::GetCopyMaterial()
{
	if (m_pCopyMat == NULL)
	{
		m_pCopyMat = _NEW MaterialEntity();
		m_pCopyMat->PushMetadata(PathMetadata(Configure::ImageCopyMaterial));
		m_pCopyMat->CreateResource();
	}

	return m_pCopyMat;
}

RenderTargetEntity* RenderPipe::_GetCurrentRenderTarget(Scene* scene)
{
#ifdef _EDITOR
	RenderTargetEntity* defaultTarget = scene->GetDefaultRenderTarget();
	return m_pAttacedRenderTarget ? m_pAttacedRenderTarget : (defaultTarget ? defaultTarget : m_pMainRenderTarget);
#else
	return m_pAttacedRenderTarget ? m_pAttacedRenderTarget : m_pMainRenderTarget;
#endif
}

RenderTargetEntity* RenderPipe::GetTempMainRT()
{
	if (m_pMainTmpRT == NULL)
	{
		m_pMainTmpRT = CreateRenderTarget(m_Resolution, GetPostRTFormat(), true);
	}

	return m_pMainTmpRT;
}

RenderTargetEntity* RenderPipe::SelectRenderTarget(bool hasEffect, Scene* sce)
{
	RenderTargetEntity* selRT = nullptr;
	RenderTargetEntity* mainrt = _GetCurrentRenderTarget(sce);

	if (!hasEffect)
	{
		if (!mainrt->HasAttachment())
		{
			selRT = GetTempMainRT();
		}
		else
		{
			selRT = mainrt;
		}
	}
	else
	{
		RenderTargetEntity* attachedRT = _GetAttachedRenderTarget();

		if (attachedRT)
		{
			selRT = attachedRT;
		}
		else
		{
			selRT = GetTempMainRT();
		}
	}

	return selRT;
}

NS_JYE_END