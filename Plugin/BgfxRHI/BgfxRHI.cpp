#include "BgfxRHI.h"
#include "Core/Interface/IApplication.h"
#include "Core/Interface/IWindow.h"
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"
#include "bx/bx.h"

NS_JYE_BEGIN

DEFINE_LOG_TAG(LogBgfxRHI);

bool FBgfxDynamicRHIModule::IsSupported()
{
	return true;
}

FDynamicRHI* FBgfxDynamicRHIModule::CreateRHI()
{
	return new BgfxRHI();
}

BgfxRHI::BgfxRHI()
	: m_ViewUniID(0)
{
}

void BgfxRHI::Init()
{
	IWindow* mainWindow = IApplication::Instance()->GetMainWindow();
	bgfx::PlatformData pd;
	pd.context = NULL;
	pd.backBuffer = NULL;
	pd.backBufferDS = NULL;
	pd.nwh = mainWindow->GetNativeWindow();
	pd.ndt = NULL;

	bgfx::setPlatformData(pd);
	bgfx::renderFrame();

	bgfx::Init bxgfinit;
#ifdef _PLATFORM_WINDOWS
	bxgfinit.profile = true;
	bxgfinit.debug = true;
	bxgfinit.type = bgfx::RendererType::OpenGL;
#endif 

	bxgfinit.vendorId = BGFX_PCI_ID_NONE;
	bxgfinit.resolution.width = 8;
	bxgfinit.resolution.height = 8;
	bxgfinit.resolution.reset = 0;
	bgfx::init(bxgfinit);

	ResetViewID();
}

void BgfxRHI::PostInit()
{

}

void BgfxRHI::Shutdown()
{

}

static RHIDefine::GraphicRenderer _BGFXRendererType2RendererType(
	bgfx::RendererType::Enum renderType)
{
	RHIDefine::GraphicRenderer venusRenderType;
	switch (renderType)
	{
	case bgfx::RendererType::OpenGLES:
		venusRenderType = RHIDefine::GraphicRenderer::RendererGles;
		break;
	case bgfx::RendererType::OpenGL:
		venusRenderType = RHIDefine::GraphicRenderer::RendererOpenGL;
		break;
	case bgfx::RendererType::Metal:
		venusRenderType = RHIDefine::GraphicRenderer::RendererMetal;
		break;
	case bgfx::RendererType::Vulkan:
		venusRenderType = RHIDefine::GraphicRenderer::RendererVulkan;
		break;
	case bgfx::RendererType::Direct3D11:
		venusRenderType = RHIDefine::GraphicRenderer::RendererDirect3D11;
		break;
	case bgfx::RendererType::Direct3D12:
		venusRenderType = RHIDefine::GraphicRenderer::RendererDirect3D12;
		break;
	default:
		venusRenderType = RHIDefine::GraphicRenderer::RendererCount;
		JYLOG("BgfxRenderType: Unknown RendererType!");
	}
	return venusRenderType;
}

RHIDefine::GraphicRenderer BgfxRHI::GetRenderer()
{
	const bgfx::Caps* caps = bgfx::getCaps();
	bgfx::RendererType::Enum renderType = caps->rendererType;
	return _BGFXRendererType2RendererType(renderType);
}

RHIDefine::GraphicFeatureLevel BgfxRHI::GetCurrentFeatureLevel() const
{
	return RHIDefine::SM_4;
}

void BgfxRHI::Frame()
{
	bgfx::frame();
}

void BgfxRHI::SetViewPort(const Math::IntVec4& vp)
{
	bgfx::setViewRect(_GetCurrentViewID(), vp.x, vp.y, vp.z, vp.w);
}

void BgfxRHI::SetRenderTarget(handle rthandle)
{
	bgfx::FrameBufferHandle fbh;
	fbh.idx = rthandle & HANDLE_MASK;
	bgfx::setViewFrameBuffer(_GetCurrentViewID(), fbh);
	m_CacheInfo.cacheRTHandle = rthandle;
}

static uint _Color2Int(const Math::FColor& _color)
{
	uint result = 0;
	result |= (uint)(_color.R) << 24;
	result |= (uint)(_color.G) << 16;
	result |= (uint)(_color.B) << 8;
	result |= (uint)(_color.A) << 0;
	return result;
}

void BgfxRHI::SetClear(uint clearFlags, const Math::FColor& color, float depth /*= 1.0f*/, byte stencil /*= 0*/)
{
	uint64_t bgfxClear = 0ull;
	if (clearFlags & RHIDefine::CF_COLOR)
	{
		bgfxClear |= BGFX_CLEAR_COLOR;
	}
	if (clearFlags & RHIDefine::CF_DEPTH)
	{
		bgfxClear |= BGFX_CLEAR_DEPTH;
	}
	if (clearFlags & RHIDefine::CF_STENCIL)
	{
		bgfxClear |= BGFX_CLEAR_STENCIL;
	}
	uint bgfxColor = _Color2Int(color);
	bgfx::setViewClear(_GetCurrentViewID(), bgfxClear, bgfxColor, depth, stencil);
}

void BgfxRHI::SetIndiceBuffer(handle ih)
{
	bool isDynamic = ((ih & DYNAMIC_BUFFER_MASK_BIT) != 0);
	if (isDynamic)
	{
		bgfx::DynamicIndexBufferHandle didxBf;
		didxBf.idx = ih & HANDLE_MASK;
		bgfx::setIndexBuffer(didxBf);
	}
	else
	{
		bgfx::IndexBufferHandle idxBf;
		idxBf.idx = ih & HANDLE_MASK;
		bgfx::setIndexBuffer(idxBf);
	}
}

void BgfxRHI::SetRenderTargetSize(const Math::IntVec2& rtSize)
{
	m_CacheInfo.cacheRtSize = rtSize;
}

void BgfxRHI::SetScissor(const Math::IntVec4& scissor)
{
	if (scissor != Math::IntVec4::ZERO)
	{
		int posY = scissor.y;
		if (RHIGetDeviceCoordinateYFlip(m_CacheInfo.cacheRTHandle) < 0)
		{
			// filp Y axis
			posY = Math::Max(0, m_CacheInfo.cacheRtSize.y - scissor.y - scissor.w);
		}
		bgfx::setScissor(scissor.x, posY, scissor.z, scissor.w);
	}
}

void BgfxRHI::SetRenderState(const uint64 state)
{
	m_CacheInfo.cacheState = state;
}

void BgfxRHI::SetStencilState(const uint64 state)
{

}

void BgfxRHI::SetUniform(handle handle, const void* value, uint16 num /*= 1*/)
{
	bgfx::UniformHandle uniformHandle;
	uniformHandle.idx = handle;
	bgfx::setUniform(uniformHandle, value, num);
}

void BgfxRHI::SetTexture(RHIDefine::ShaderType shaderType, byte stage, byte bindPoint, handle uniHandle, handle th, unsigned int flags /*= 0xffffffffU*/)
{
	bgfx::UniformHandle uniformHandle;
	uniformHandle.idx = uniHandle;
	bgfx::TextureHandle texHandle;
	texHandle.idx = th & HANDLE_MASK;

	bgfx::setTexture(stage, uniformHandle, texHandle, flags);
}

void BgfxRHI::SetConstBuffer(RHIDefine::ShaderType shaderType, byte stage, byte bindingPoint, handle cbHandle)
{

}

void BgfxRHI::SubmitIndexedDraw(RHIDefine::RenderMode renderMode, handle _vertexHandle, handle _indexHandle, handle _programHandle, uint _firstIndex, uint _indexCount)
{
	uint64 curState = m_CacheInfo.cacheState;
	if ((m_CacheInfo.cacheState & BGFX_STATE_PT_POINTS) == 0)
	{
		// apply state
		curState = curState & ~BGFX_STATE_PT_MASK;
		switch (renderMode)
		{
		case RHIDefine::RM_POINTS:
			curState |= BGFX_STATE_PT_POINTS;
			break;
		case RHIDefine::RM_LINES:
			curState |= BGFX_STATE_PT_LINES;
			break;
		case RHIDefine::RM_TRIANGLE_STRIP:
			curState |= BGFX_STATE_PT_TRISTRIP;
			break;
		case RHIDefine::RM_LINE_STRIP:
			curState |= BGFX_STATE_PT_LINESTRIP;
			break;
		case RHIDefine::PM_NULL:
		default:
			break;
		}
	}
	bgfx::setState(curState);

	bool isDynamicVertexBuf = ((_vertexHandle & DYNAMIC_BUFFER_MASK_BIT) != 0);
	if (isDynamicVertexBuf)
	{
		bgfx::DynamicVertexBufferHandle vertexHandle;
		vertexHandle.idx = (_vertexHandle & HANDLE_MASK);
		bgfx::setVertexBuffer(0, vertexHandle);
	}
	else
	{
		bgfx::VertexBufferHandle vertexHandle;
		vertexHandle.idx = _vertexHandle & HANDLE_MASK;
		bgfx::setVertexBuffer(0, vertexHandle);
	}

	handle indexHandle = _indexHandle;
	bool isDynamicIndexBuf = ((indexHandle & DYNAMIC_BUFFER_MASK_BIT) != 0);
	if (isDynamicIndexBuf)
	{
		bgfx::DynamicIndexBufferHandle _indexHandle;
		_indexHandle.idx = (indexHandle & HANDLE_MASK);
		bgfx::setIndexBuffer(_indexHandle, _firstIndex, _indexCount);
	}
	else
	{
		bgfx::IndexBufferHandle _indexHandle;
		_indexHandle.idx = indexHandle & HANDLE_MASK;
		bgfx::setIndexBuffer(_indexHandle, _firstIndex, _indexCount);
	}

	bgfx::ProgramHandle programHandle;
	programHandle.idx = _programHandle;
	bgfx::submit(_GetCurrentViewID(), programHandle);
}

void BgfxRHI::SubmitIndexedDrawInstanced(RHIDefine::RenderMode renderMode, handle _vertexHandle, handle _indexHandle, handle _instanceHandle, handle _programHandle, uint _firstIndex, uint _indexCount, uint _instanceCount)
{

}
bool BgfxRHI::SupportDepthTexture()
{
	const bgfx::Caps* caps = bgfx::getCaps();
	return 0 != (caps->supported & BGFX_CAPS_TEXTURE_COMPARE_LEQUAL);
}

bool BgfxRHI::SupportShadowSampler()
{
	const bgfx::Caps* caps = bgfx::getCaps();
	return 0 != (caps->supported2 & BGFX_CAPS_SHADOW_SAMPLER);
}

bool BgfxRHI::SupportDeferredShading()
{
	BgfxRHI* temp = const_cast<BgfxRHI*>(this);
	bgfx::DeviceFeatureLevel::Enum flevel = bgfx::getDeviceFeatureLevel();
	return
		(BX_ENABLED(BX_PLATFORM_WINDOWS || BX_PLATFORM_OSX || BX_PLATFORM_LINUX))
		|| (temp->SupportHalfFrameBuffer() || temp->SupportFloatFrameBuffer())
		&& (flevel == bgfx::DeviceFeatureLevel::ES3_0 || flevel == bgfx::DeviceFeatureLevel::ES3_1
			|| flevel == bgfx::DeviceFeatureLevel::ES3_2)
		&& (temp->SupportDepthTexture());
}

extern bgfx::TextureFormat::Enum _BGFXPixelFormat(RHIDefine::PixelFormat f);
bool BgfxRHI::IsTextureFormatSupportSRGB(RHIDefine::PixelFormat pf)
{
	const bgfx::Caps* caps = bgfx::getCaps();
	bgfx::TextureFormat::Enum btf = _BGFXPixelFormat(pf);
	return caps->formats[btf] & BGFX_CAPS_FORMAT_TEXTURE_2D_SRGB;
}

bool BgfxRHI::IsSupportsSRGB()
{
	const bgfx::Caps* caps = bgfx::getCaps();
	return caps->supported & BGFX_CAPS_SRGB;
}

uint BgfxRHI::GetFrameBufferAttachments()
{
	const bgfx::Caps* caps = bgfx::getCaps();
	return caps->limits.maxFBAttachments;
}

uint BgfxRHI::GetMaxRTAttachments()
{
	const bgfx::Caps* caps = bgfx::getCaps();
	return caps->limits.maxFBAttachments;
}

bool BgfxRHI::SupportHalfFrameBuffer()
{
	const bgfx::Caps* caps = bgfx::getCaps();
	return 0 != (caps->supported2 & BGFX_CAPS_FRAMEBUFFER_HALF);
}

bool BgfxRHI::SupportFloatFrameBuffer()
{
	const bgfx::Caps* caps = bgfx::getCaps();
	return 0 != (caps->supported2 & BGFX_CAPS_FRAMEBUFFER_FLOAT);
}

bool BgfxRHI::IsSupportCompute()
{
	const bgfx::Caps* pCaps = bgfx::getCaps();
	return (pCaps->supported & BGFX_CAPS_COMPUTE) != 0;
}

bool BgfxRHI::IsSupportGPUInstancing()
{
	const bgfx::Caps* caps = bgfx::getCaps();
	return 0 != (caps->supported & BGFX_CAPS_INSTANCING);
}

NS_JYE_END
