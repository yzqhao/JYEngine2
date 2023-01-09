#pragma once

#include "Engine/private/Define.h"
#include "Engine/graphic/IAsyncResource.h"
#include "TextureStream.h"
#include "RHI/RHIDefine.h"
#include "Engine/resource/IAsyncEntity.h"
#include "Engine/graphic/ITextureResource.h"
#include "Engine/resource/ISharedSource.h"
#include "Math/IntVec2.h"
#include "Math/Vec2.h"
#include "Math/Vec3.h"
#include "Math/Vec4.h"

NS_JYE_BEGIN

class TextureSource : public ISharedSource
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	TextureSource();
	virtual ~TextureSource();

	void SetSize(const Math::IntVec2& size);

	FORCEINLINE void SetKeepSource(bool k);
	FORCEINLINE bool isKeepSource();
	FORCEINLINE void SetMipMap(bool mip, uint level);
	FORCEINLINE void SetTextureUsage(RHIDefine::TextureUseage tu);
	FORCEINLINE void SetTextureType(RHIDefine::TextureType tt);
	FORCEINLINE void SetWarp(RHIDefine::TextureWarp s, RHIDefine::TextureWarp t);
	FORCEINLINE void SetFilter(RHIDefine::TextureFilter mag, RHIDefine::TextureFilter min);
	FORCEINLINE void SetPixelFormat(RHIDefine::PixelFormat pf);
	FORCEINLINE void SetAnisotropic(uint x);
	FORCEINLINE RHIDefine::PixelFormat GetPixelFormat() const;
	FORCEINLINE const Math::IntVec2& GetSize() const;
	FORCEINLINE RHIDefine::TextureType GetTextureType() const;
	FORCEINLINE RHIDefine::WarpType GetWrap() const;
	FORCEINLINE RHIDefine::TextureUseage GetMemoryUseage() const;
	FORCEINLINE const std::string& GetName();
	FORCEINLINE void SetRTAttachmentFlag(bool f);
	FORCEINLINE void SetSamplerCompare(RHIDefine::TextureSamplerCompare compare);
	FORCEINLINE void SetSRGB(bool _srgb);
	FORCEINLINE bool IsSRGB() const;
	FORCEINLINE handle GetHandle() const;
private:
	virtual IAsyncResource* _DoCreateResource() override;//����Դ��������Ϊʵ����Դ
	virtual bool _DoSetupResource(IAsyncResource* res) override;//���´�����Դ
	virtual bool _DoLoadResource() override;//������Դ
	virtual void _DoReleaseUnnecessaryDataAfterLoaded() override;

private:
	Math::IntVec2 m_Size;
	TextureStreams m_TextureStreams;
	RHIDefine::TextureUseage m_Usage;
	RHIDefine::WarpType m_Warp;
	RHIDefine::FilterType m_Filter;
	RHIDefine::TextureSamplerCompare m_SamplerCompare;
	RHIDefine::PixelFormat m_eFormat;
	RHIDefine::TextureType m_eTextureType;
	std::string m_Name;
	handle m_hInputHandle;
	uint m_Anisotropic;
	bool m_isMipMap;
	uint m_uBaseLevel;
	uint m_uTopLevel;
	bool m_isLoaded;
	bool m_isKeepSource;
	bool m_allocDefaultTextureStream; // ����ʱ�����һ��Ĭ�ϵ�TextureStream,Ŀǰ�������õ�TextureEntity
	bool m_isRTAttachment;
	bool m_isSRGB;
	bool m_isInternelReference; //�ж��Ƿ�Ϊ�ű����ݵ�resource handle��רΪ����ͨ�������ڲ�������reference��ԭ���������ڴ���ͻ��˴�������reference
	bool m_isMsaa;
};
DECLARE_Ptr(TextureSource);
TYPE_MARCO(TextureSource);

class ENGINE_API TextureEntity : public IAsyncEntity<ITextureResource, TextureSource>
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	TextureEntity();
	virtual ~TextureEntity();
	DECLARE_OBJECT_CTOR_FUNC(TextureEntity);

	void Resize(const Math::IntVec2& rsl);	//RT����

	virtual void BeforeSave(Stream* pStream) override;
	virtual void PostSave(Stream* pStream) override;
	virtual void PostLoad(Stream* pStream) override;

	FORCEINLINE void SetKeepSource(bool k);
	FORCEINLINE bool isKeepSource();
	FORCEINLINE RHIDefine::PixelFormat GetPixelFormat() const;
	FORCEINLINE const Math::IntVec2& GetSize() const;
	FORCEINLINE RHIDefine::TextureType GetTextureType() const;
	FORCEINLINE RHIDefine::WarpType GetWrap() const;
	FORCEINLINE RHIDefine::TextureUseage GetMemoryUseage() const;
	FORCEINLINE const std::string& GetName();
	FORCEINLINE void SetRTAttachmentFlag(bool f);
	FORCEINLINE bool IsSRGB() const;
	FORCEINLINE handle GetHandle() const;
	FORCEINLINE uint GetFlags() const;

private:
};
DECLARE_Ptr(TextureEntity);
TYPE_MARCO(TextureEntity);

#include "TextureEntity.inl"

NS_JYE_END