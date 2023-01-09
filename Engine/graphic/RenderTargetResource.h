#pragma once

#include "Engine/private/Define.h"
#include "Math/IntVec2.h"
#include "Math/IntVec4.h"
#include "Math/Vec2.h"
#include "Math/Vec4.h"
#include "Engine/graphic/IAsyncResource.h"
#include "Engine/render/texture/TextureStream.h"
#include "RHI/RHIDefine.h"
#include "Engine/render/texture/TextureEntity.h"
#include "Engine/ViewResolution.h"

#include <mutex>

NS_JYE_BEGIN

class ENGINE_API RenderTargetResource : public ISharedResource
{
	DECLARE_MULTICAST_DELEGATE(RenderThreadExitCallback);
private:
	struct HandleWrapper
	{
		handle m_GPUhd;
		uint m_reference;
	};
	typedef Map<void*, HandleWrapper*> WindowRenderTargetMap;
private:
	static WindowRenderTargetMap s_GlobalWindowRTMap;
	static std::mutex			 s_gWindowMapMutex;
protected:
	typedef Vector<TextureStream*> BufferStreams;
	typedef Vector< Pointer<TextureEntity> > AttachmentVector;
	typedef Map< RHIDefine::RenderTargetAttachment, Pointer<TextureEntity> > AttachmentMap;
private:
	handle m_RefrenceHandle;		//为了满足vpsdk的集成需求。。。
	BufferStreams m_BufferStreams;	//读取出的fbo信息的stream
	AttachmentVector m_TextureVector;
	AttachmentMap m_AttachmentMap;
	Math::IntVec4 m_ViewPort;
	Math::IntVec2 m_Size;
	Math::Vec2 m_Scale;
	uint m_hGPUResourceHandle;
	RHIDefine::TargetType m_eTargetType;
	bool m_NeedRead;
	bool m_RTGenerated;
	void* m_NativeWhd;
	bool m_NeedResize;
	RenderThreadExitCallbackDelegate m_RenderThreadFunction;
	DelegateHandle m_RenderThreadFunctionHandle;
public:
	RenderTargetResource(void);
	virtual ~RenderTargetResource(void);
private:
	virtual bool _DeprecatedFilter(bool isd);
public:
	virtual void ReleaseResource();
private:
	handle _CreateHandleFromWindow();
	handle _CreateHandleFromOffScreen();
	handle _CreateFromRefHandle();
	void _ResizeHandleFromOffScreen();
	void _ResizeHandleFromWindow();
	void _FreeHandleFromOffScreen();
	void _FreeHandleFromWindow();
	void _FreeHandleFromRefHandle();
public:
	void OnRenderThreadExit();
	void Resizeview(const ViewResolution& vr);
	void ResizeInRenderThread();
	void MakeSureCreate();
	void RefranceRenderTarget(handle h, RHIDefine::TargetType tt, const Math::IntVec2& size, const Math::Vec2& scale);
	void GenerateRenderTarget(RHIDefine::TargetType tt, const Math::IntVec2& size, const Math::Vec2& scale, void* whd = (void*)nullhandle);
	void SetAttachment(RHIDefine::RenderTargetAttachment att, Pointer<TextureEntity> tex);
	void ReadBuffer();//目前只读取COLOR0
	bool IsSRGB() const;
public:
	
	FORCEINLINE bool NeedRead()
	{
		return m_NeedRead;
	}
	
	FORCEINLINE RHIDefine::TargetType GetTargetType()
	{
		return m_eTargetType;
	}
	
	FORCEINLINE void SetViewPort(const Math::IntVec4& vp)
	{
		m_ViewPort = vp;
	}
	
	FORCEINLINE const Math::IntVec4& GetViewPort() const
	{
		return m_ViewPort;
	}
	
	FORCEINLINE TextureEntity* GetAttachment(RHIDefine::RenderTargetAttachment att)
	{
		return m_TextureVector[att];
	}
	
	FORCEINLINE bool HasAttachment()
	{
		return m_AttachmentMap.size() > 0 || IsRefTarget();
	}
	
	FORCEINLINE const Math::IntVec2& GetResolution() const
	{
		return m_Size;
	}
	
	FORCEINLINE bool isMainRenderTarget() const
	{
		return m_eTargetType == RHIDefine::RT_RENDER_TARGET_MAIN;
	}
	
	FORCEINLINE TextureStream* GetAttachmentStream(RHIDefine::RenderTargetAttachment rta)
	{
		return m_BufferStreams[rta];
	}
	
	FORCEINLINE bool IsWindowTarget() const
	{
		return (void*)nullhandle != m_NativeWhd && nullhandle == m_RefrenceHandle;
	}
	
	FORCEINLINE bool IsOffScreenTarget() const
	{
		return nullhandle == m_RefrenceHandle && IsWindowTarget() == false;
	}
	
	FORCEINLINE bool IsRefTarget() const
	{
		return nullhandle != m_RefrenceHandle;
	}
	
	FORCEINLINE handle GetGPUHandle() const
	{
		return m_hGPUResourceHandle;
	}
};

NS_JYE_END