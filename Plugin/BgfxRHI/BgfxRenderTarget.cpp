#include "BgfxRHI.h"
#include "bgfx/bgfx.h"

NS_JYE_BEGIN

extern bgfx::TextureFormat::Enum _BGFXPixelFormat(RHIDefine::PixelFormat f);

handle BgfxRHI::CreateRenderTarget(void* h, uint width, uint height, RHIDefine::PixelFormat format, RHIDefine::PixelFormat depth_format)
{
	bgfx::FrameBufferHandle hd;
	if (NULL != h)
	{
		hd = bgfx::createFrameBuffer(h, width, height,
			_BGFXPixelFormat(format),
			_BGFXPixelFormat(depth_format));
	}
	else
	{
		bgfx::reset(width, height, BGFX_RESET_NONE, _BGFXPixelFormat(format));
		hd.idx = bgfx::kInvalidHandle;
	}
	handle outHandle = hd.idx;
	outHandle |= MAIN_RT_MASK_BIT;
	return outHandle;
}

handle BgfxRHI::CreateRenderTarget(int32_t refHandle, uint width, uint height, RHIDefine::PixelFormat format, RHIDefine::PixelFormat depth_format)
{
	bgfx::FrameBufferHandle rtHandle;
	rtHandle = bgfx::createFrameBuffer(refHandle, width, height,
		_BGFXPixelFormat(format),
		_BGFXPixelFormat(depth_format));

	return rtHandle.idx;
}

handle BgfxRHI::CreateRenderTarget(uint count, RHIDefine::RenderTargetAttachment* att, handle* texHandles)
{
	if (count <= 0 || count > RHIDefine::RenderTargetAttachment::TA_COUNT)
	{
		JYWARNING("BgfxRenderTarget: Wrong count of attachments");
	}

	bgfx::TextureHandle* attachTex = _NEW bgfx::TextureHandle[count];
	for (int i = 0; i < count; ++i)
	{
		attachTex[i].idx = texHandles[i] & HANDLE_MASK;
	}
	bgfx::FrameBufferHandle hd = bgfx::createFrameBuffer(count, attachTex, false);
	SAFE_DELETE_ARRAY(attachTex);
	return hd.idx;
}

handle BgfxRHI::CreateRenderTarget(uint width, uint height, RHIDefine::PixelFormat format)
{
	bgfx::FrameBufferHandle hd = bgfx::createFrameBuffer(width, height, _BGFXPixelFormat(format));
	return hd.idx;
}

void BgfxRHI::DestoryRenderTarget(handle _rt)
{
	if (nullhandle == _rt)
	{
		JYWARNING("BgfxRenderTarget: Attemp to destroy empty rendertarget.");
		return;
	}

	handle interHd = _rt & HANDLE_MASK;
	if (bgfx::kInvalidHandle == interHd)
	{
		return;	 // do not destroy main window rt
	}
	bgfx::FrameBufferHandle fbhd;
	fbhd.idx = interHd;
	bgfx::destroy(fbhd);
}

void BgfxRHI::_SetCurrentViewID(uint16 id)
{
	bgfx::setCurrentViewID(id);
}

uint16 BgfxRHI::_GetCurrentViewID()
{
	return bgfx::getCurrentViewID();
}

void BgfxRHI::ResetViewID()
{
	m_ViewUniID = 0;
}

void BgfxRHI::SetCurrentViewID(uint16 id)
{
	bgfx::setCurrentViewID(id);
}

uint16 BgfxRHI::GetUniformViewID()
{
	m_ViewUniID++;
	bgfx::resetView(m_ViewUniID);
	return m_ViewUniID;
}

float BgfxRHI::GetDeviceCoordinateYFlip(handle rt)
{
	float flip = 1.0f;
	const bgfx::Caps* caps = bgfx::getCaps();
	if (caps->originBottomLeft)
	{
		// test if belong main rendertarget
		if (rt & MAIN_RT_MASK_BIT)
		{
			flip = 1.0f;
		}
		else
		{
			flip = -1.0;
		}
	}
	return flip;
}

NS_JYE_END