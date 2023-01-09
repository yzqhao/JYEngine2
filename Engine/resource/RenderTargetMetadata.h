#pragma once

#include "GeneralMetadata.h"
#include "Math/IntVec2.h"
#include "RHI/RHI.h"
#include "Engine/render/texture/TextureStream.h"
#include "../ViewResolution.h"

NS_JYE_BEGIN

struct RenderTextureProperty
{
	RHIDefine::PixelFormat m_ePixelFormat;
	RHIDefine::TextureType m_eTextureType;
	RHIDefine::TextureUseage m_eTextureUseage;
	RHIDefine::TextureWarp m_eVWarp;
	RHIDefine::TextureWarp m_eHWarp;
	RHIDefine::TextureFilter m_eMagFilter;
	RHIDefine::TextureFilter m_eMinFilter;
};

typedef Map<RHIDefine::RenderTargetAttachment, RenderTextureProperty>	RenderTexturePropertys;

struct RenderTargetData
{
	handle m_Resource;	//实现外部传入一个gpu资源handle，ugly
	void* m_gWindow;	// Add support to create rt from window
	std::string m_Name;
	Math::FColor m_Color;
	Math::IntVec4 m_ViewPort;
	Math::IntVec2 m_Size;
	Math::Vec2 m_Scale;
	RHIDefine::TargetType m_eTargetType;
	RenderTexturePropertys m_Textures;
	int m_version;
	bool m_isSrgb;
	RenderTargetData()
		: m_Resource(nullhandle)
		, m_gWindow((void*)nullhandle)
		, m_eTargetType(RHIDefine::RT_RENDER_TARGET_2D)
		, m_Scale(1.0f, 1.0f)
		, m_Color(Math::FColor::Purple)
		, m_Size()
		, m_ViewPort()
		, m_version(0)
		, m_isSrgb(false)
	{}
};

class ENGINE_API RenderTargetMetadata : public PathMetadata
{
	DECLARE_RTTI
	DECLARE_INITIAL
private:
	RenderTargetData m_Metadata;
public:
	RenderTargetMetadata() {}
	RenderTargetMetadata(
		RHIDefine::TargetType tt,
		const Math::IntVec4& vp,
		const Math::IntVec2& size,
		const Math::Vec2& scale,
		const bool issRGB = false,
		void* winhandle = (void*)nullhandle
	);
	RenderTargetMetadata(
		RHIDefine::TargetType tt,
		const Math::IntVec4& vp,
		const Math::IntVec2& size,
		const bool issRGB = false,
		void* winhandle = (void*)nullhandle
	);
	virtual ~RenderTargetMetadata(void);
private:
	virtual void* _ReturnMetadata();
public:
	virtual void GetIdentifier(std::string& hash) const override;//在str中推入表示符号
	virtual void ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash) override;//处理原始资源
	virtual void ReleaseMetadate() override;
	virtual void Resizeview(const ViewResolution& vr);
};
DECLARE_Ptr(RenderTargetMetadata);
TYPE_MARCO(RenderTargetMetadata);

NS_JYE_END