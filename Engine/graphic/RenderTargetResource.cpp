#include "RenderTargetResource.h"
#include "RHI/DynamicRHI.h"
#include "../IGraphicSystem.h"

NS_JYE_BEGIN

RenderTargetResource::WindowRenderTargetMap RenderTargetResource::s_GlobalWindowRTMap;
std::mutex									RenderTargetResource::s_gWindowMapMutex;


RenderTargetResource::RenderTargetResource(void)
	: ISharedResource(ResourceProperty::SRT_RENDERTARGET)
	, m_eTargetType((RHIDefine::TargetType)-1)
	, m_hGPUResourceHandle(nullhandle)
	, m_RefrenceHandle(nullhandle)
	, m_RenderThreadFunction()
	, m_Scale(1.0f, 1.0f)
	, m_NeedRead(false)
	, m_RTGenerated(false)
	, m_NativeWhd((void*)nullhandle)
	, m_NeedResize(false)
{
	m_BufferStreams.resize(RHIDefine::TA_COUNT, NULL);
	m_TextureVector.resize(RHIDefine::TA_COUNT, Pointer<TextureEntity>(NULL));
}

RenderTargetResource::~RenderTargetResource(void)
{
	for (TextureStream* ts : m_BufferStreams)
	{
		SAFE_DELETE(ts);
	}
}

bool RenderTargetResource::IsSRGB() const
{
	if (m_TextureVector[RHIDefine::TA_COLOR_0] != NULL)
	{
		return m_TextureVector[RHIDefine::TA_COLOR_0]->IsSRGB();
	}
	else
	{
		return false;
	}
}

bool RenderTargetResource::_DeprecatedFilter(bool isd)
{
	//只有不是引用fbo而且fbo已经创建，才需要设置废弃
	if (RHIDefine::RT_RENDER_TARGET_MAIN != m_eTargetType
		&& nullhandle == m_RefrenceHandle
		&& nullhandle != m_hGPUResourceHandle)
	{
		return isd;
	}
	return false;
}

void RenderTargetResource::ReleaseResource()
{
	//只有不是引用fbo而且不是主窗口，才需要设置废弃
	if (m_RenderThreadFunction.GetSize() > 0)
	{
		IGraphicSystem::Instance()->RemoveRenderThreadExitCallback(m_RenderThreadFunctionHandle);
		m_RenderThreadFunction.Clear();
	}

	if (IsWindowTarget())
	{
		_FreeHandleFromWindow();
	}
	else if (IsOffScreenTarget())
	{
		_FreeHandleFromOffScreen();
	}
	else if (IsRefTarget())
	{
		_FreeHandleFromRefHandle();
	}
	m_RTGenerated = false;
}

void RenderTargetResource::OnRenderThreadExit()
{
	if (IsOffScreenTarget())
	{
		_FreeHandleFromOffScreen();
		m_RTGenerated = false;
	}
}

void RenderTargetResource::Resizeview(const ViewResolution& vr)
{
	Math::IntVec2 nres{ (int)m_Scale.x * vr.m_Resolution.x, (int)m_Scale.y * vr.m_Resolution.y };
	nres.x = Math::Max<int>(nres.x, 1);
	nres.y = Math::Max<int>(nres.y, 1);
	if (m_Size != nres)
	{
		m_Size = nres;
		m_ViewPort = Math::IntVec4(1, 1, (int)m_Scale.x, (int)m_Scale.y) * vr.m_ViewSize;
		m_NeedResize = true;
	}
}

void RenderTargetResource::ResizeInRenderThread()
{
	//m_ViewPort = vr.m_ViewSize;//vp暂时不缩放
	if (IsOffScreenTarget())
	{
		_ResizeHandleFromOffScreen();
	}
	else if (IsWindowTarget())
	{
		_ResizeHandleFromWindow();
	}
}

void RenderTargetResource::MakeSureCreate()
{
	if (m_NeedResize)
	{
		ResizeInRenderThread();
		m_NeedResize = false;
	}
	if (!m_RTGenerated)//FBO修改为延迟创建，因为FBO是线程独占的资源，需要在具体使用的线程进行创建
	{
		if (IsWindowTarget())
		{
			m_hGPUResourceHandle = _CreateHandleFromWindow();
			m_RTGenerated = true;
		}
		else if (IsOffScreenTarget())
		{
			m_hGPUResourceHandle = _CreateHandleFromOffScreen();
			m_RTGenerated = true;
		}
		else if (IsRefTarget())
		{
			m_hGPUResourceHandle = _CreateFromRefHandle();
			m_RTGenerated = true;
		}
	}

	if (IsOffScreenTarget())
	{
		bool needResize = false;
		for (auto texpair : m_AttachmentMap)
		{
			if (NULL != texpair.second
				&& texpair.second->GetSize() != m_Size)
			{
				//由于共享纹理的原因会导致这个问题（共享前给相同的size，但是共享纹理被resize，导致的拥有相同hash但是不同的size问题）
				const Math::IntVec2& texsize = texpair.second->GetSize();
				JYERROR("FBO push:different size between fbo and texture. size ofFBO  is %d %d, texture is %d %d"
					, m_Size.x, m_Size.y, texsize.x, texsize.y);
				needResize = true;
				break;
			}
		}

		if (needResize)
		{
			_ResizeHandleFromOffScreen();
		}
	}
}

void RenderTargetResource::RefranceRenderTarget(handle h, RHIDefine::TargetType tt, const Math::IntVec2& size, const Math::Vec2& scale)
{
	m_hGPUResourceHandle = nullhandle;
	m_RefrenceHandle = h;
	m_eTargetType = tt;
	m_Size = size;
	m_Scale = scale;
}

void RenderTargetResource::GenerateRenderTarget(RHIDefine::TargetType tt, const Math::IntVec2& size, const Math::Vec2& scale, void* _whd)
{
	m_hGPUResourceHandle = nullhandle;
	m_eTargetType = tt;
	m_Size = size;
	m_Scale = scale;
	m_NativeWhd = _whd;
}

void RenderTargetResource::SetAttachment(RHIDefine::RenderTargetAttachment att, Pointer<TextureEntity> tex)
{
	if (true == m_RTGenerated)
	{
		JYERROR("Attachment should be set before Push function");
	}

	//当附件为buffer类型的时候tex为NULL
	m_TextureVector[att] = tex;
	m_AttachmentMap.insert(std::make_pair(att, tex));

	if (tex && RHIDefine::TU_READ == tex->GetMemoryUseage())
	{
		JYWARNING("memory usage of texture is TU_READ");
		m_NeedRead = true;
		JY_ASSERT(NULL == m_BufferStreams[att]);
		m_BufferStreams[att] = _NEW TextureStream();
		m_BufferStreams[att]->SetStreamType(
			tex->GetSize(),
			tex->GetPixelFormat());
	}
}

void RenderTargetResource::_FreeHandleFromWindow()
{
	s_gWindowMapMutex.lock();
	if (nullhandle != m_hGPUResourceHandle)
	{
		void* key = m_NativeWhd;
		WindowRenderTargetMap::iterator iter = s_GlobalWindowRTMap.find(key);
		if (iter != s_GlobalWindowRTMap.end())
		{
			HandleWrapper* hdwrp = iter->second;
			hdwrp->m_reference -= 1;
			if (hdwrp->m_reference <= 0)
			{
				RHIDestoryRenderTarget(hdwrp->m_GPUhd);
				s_GlobalWindowRTMap.erase(key);
				SAFE_DELETE(hdwrp);
			}
		}
		m_hGPUResourceHandle = nullhandle;
	}
	s_gWindowMapMutex.unlock();
}

void RenderTargetResource::_FreeHandleFromOffScreen()
{
	if (nullhandle != m_hGPUResourceHandle)
	{
		RHIDestoryRenderTarget(m_hGPUResourceHandle);
		m_hGPUResourceHandle = nullhandle;
	}
}

void RenderTargetResource::_FreeHandleFromRefHandle()
{
	if (nullhandle != m_hGPUResourceHandle)
	{
		RHIDestoryRenderTarget(m_hGPUResourceHandle);
		m_hGPUResourceHandle = nullhandle;
	}
}

handle RenderTargetResource::_CreateHandleFromOffScreen()
{
	handle res = 0;
	RHIDefine::RenderTargetAttachment rtAttachs[
		RHIDefine::RenderTargetAttachment::TA_COUNT];
	handle texHandles[RHIDefine::RenderTargetAttachment::TA_COUNT];

	if (m_AttachmentMap.size() > 0)
	{
		int attachIndex = 0;
		for (auto texpair : m_AttachmentMap)
		{
			rtAttachs[attachIndex] = texpair.first;
			texHandles[attachIndex] = texpair.second->GetHandle();
			++attachIndex;
		}

		res = RHICreateRenderTarget(m_AttachmentMap.size(),
			rtAttachs,
			texHandles);
	}
	else
	{
		res = RHICreateRenderTarget(m_Size.x, m_Size.y);
	}

	if (m_RenderThreadFunction.GetSize() == 0)
	{
		m_RenderThreadFunction = RenderThreadExitCallbackDelegate::CreateRaw(this, &RenderTargetResource::OnRenderThreadExit);
		m_RenderThreadFunctionHandle = IGraphicSystem::Instance()->SetRenderThreadExitCallback(m_RenderThreadFunction);
	}

	m_RTGenerated = true;
	return res;
}

void RenderTargetResource::_ResizeHandleFromOffScreen()
{
	// Resize
	for (auto texpair : m_AttachmentMap)
	{
		if (texpair.second)
		{
			texpair.second->Resize(m_Size);
		}
	}

	for (TextureStream* ts : m_BufferStreams)
	{
		if (NULL != ts)
		{
			ts->Resize(m_Size);
		}
	}

	if (m_hGPUResourceHandle != nullhandle)
	{
		_FreeHandleFromOffScreen();
	}
	m_hGPUResourceHandle = _CreateHandleFromOffScreen();
}

handle RenderTargetResource::_CreateHandleFromWindow()
{
	handle res = 0;
	if ((void*)nullhandle != m_NativeWhd)  // window rendertarget
	{
		s_gWindowMapMutex.lock();
		void* key = m_NativeWhd;
		WindowRenderTargetMap::iterator iter = s_GlobalWindowRTMap.find(key);
		if (iter != s_GlobalWindowRTMap.end())
		{
			res = iter->second->m_GPUhd;
			iter->second->m_reference += 1;
		}
		else
		{
			res = RHICreateRenderTarget(m_NativeWhd,
				m_Size.x,
				m_Size.y);
			HandleWrapper* hdwrp = _NEW HandleWrapper;
			hdwrp->m_GPUhd = res;
			hdwrp->m_reference = 1;
			s_GlobalWindowRTMap.insert({ key, hdwrp });
		}
		m_RTGenerated = true;
		s_gWindowMapMutex.unlock();
	}

	return res;
}

void RenderTargetResource::_ResizeHandleFromWindow()
{
	for (TextureStream* ts : m_BufferStreams)
	{
		if (NULL != ts)
		{
			ts->Resize(m_Size);
		}
	}

	if (m_hGPUResourceHandle != nullhandle)
	{
		_FreeHandleFromWindow();
	}

	m_hGPUResourceHandle = _CreateHandleFromWindow();
}

handle RenderTargetResource::_CreateFromRefHandle()
{
	handle res = 0;
	res = RHICreateRenderTarget(m_RefrenceHandle,
		m_Size.x,
		m_Size.y);
	m_RTGenerated = true;

	return res;
}

void RenderTargetResource::ReadBuffer()
{
	//TextureStream* ts = m_BufferStreams[RHIDefine::TA_COLOR_0];
	//m_TextureVector[RHIDefine::TA_COLOR_0]->ReadTexture(ts);
}

NS_JYE_END