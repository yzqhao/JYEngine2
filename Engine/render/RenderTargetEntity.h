#pragma once

#include "Engine/private/Define.h"
#include "Engine/graphic/IAsyncResource.h"
#include "RHI/RHIDefine.h"
#include "Engine/resource/IAsyncEntity.h"
#include "Engine/graphic/RenderTargetResource.h"
#include "Engine/resource/ISharedSource.h"
#include "Math/IntVec2.h"
#include "Math/Vec2.h"
#include "Math/Vec3.h"
#include "Math/Vec4.h"

NS_JYE_BEGIN

class RenderTargetSource : public ISharedSource
{
	DECLARE_RTTI
	DECLARE_INITIAL
private:
	typedef Map< RHIDefine::RenderTargetAttachment, Pointer<TextureEntity> > AttachmentMap;
public:
	RenderTargetSource();
	virtual ~RenderTargetSource();

	TextureEntity* MakeTextureAttachment(RHIDefine::RenderTargetAttachment rta);//设置纹理
	void Resizeview(const ViewResolution& vr);
	void CreateAttachment();

	FORCEINLINE const String& GetName() const;
	FORCEINLINE bool isRefrence();
	FORCEINLINE void SetWinHandle(void* whd);
	FORCEINLINE void SetSize(const Math::IntVec2& size);
	FORCEINLINE const Math::IntVec2& GetSize();
	FORCEINLINE void SetViewPort(const Math::IntVec4& rect);
	FORCEINLINE const Math::IntVec4& GetViewPort();
	FORCEINLINE void SetTargetType(RHIDefine::TargetType tt);
private:
	virtual IAsyncResource* _DoCreateResource() override;//将资源数据制作为实体资源
	virtual bool _DoSetupResource(IAsyncResource* res) override;//重新创建资源
	virtual bool _DoLoadResource() override;//加载资源

private:
	AttachmentMap m_TextureBufferMap;
	String m_Name;
	Math::IntVec4 m_ViewPort;
	Math::IntVec2 m_Size;
	Math::Vec2 m_Scale;
	RHIDefine::TargetType m_eTargetType;
	handle m_RefrenceHandle;
	void* m_gWindow;
};
DECLARE_Ptr(RenderTargetSource);
TYPE_MARCO(RenderTargetSource);

class ENGINE_API RenderTargetEntity : public IAsyncEntity<RenderTargetResource, RenderTargetSource>
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	RenderTargetEntity();
	virtual ~RenderTargetEntity();
	DECLARE_OBJECT_CTOR_FUNC(RenderTargetEntity);

	TextureEntity* MakeTextureAttachment(RHIDefine::RenderTargetAttachment rta);//设置纹理
	void Resizeview(const ViewResolution& vr);
	void SetResizeviewFlag(bool flag);

	virtual void BeforeSave(Stream* pStream) override;
	virtual void PostSave(Stream* pStream) override;
	virtual void PostLoad(Stream* pStream) override;

	FORCEINLINE const std::string& GetName() const;
	FORCEINLINE bool IsSRGB() const;
	FORCEINLINE void MakeSureCreate();
	FORCEINLINE TextureEntity* GetAttachment(RHIDefine::RenderTargetAttachment att);
	FORCEINLINE bool isMainRenderTarget() const;
	FORCEINLINE const Math::IntVec2& GetResolution() const;
	FORCEINLINE const Math::IntVec4& GetViewPort() const;
	FORCEINLINE TextureStream* GetAttachmentStream(RHIDefine::RenderTargetAttachment rta);
	FORCEINLINE bool NeedRead();
	FORCEINLINE void ReadBuffer();//目前只读取COLOR0
	FORCEINLINE bool HasAttachment();
	FORCEINLINE handle GetGPUHandle() const;

private:
	virtual void _OnCreateResource() override;

	bool m_enableResizeview{true};//是否需要随视口缩放，默认需要
};
DECLARE_Ptr(RenderTargetEntity);
TYPE_MARCO(RenderTargetEntity);

#include "RenderTargetEntity.inl"

NS_JYE_END