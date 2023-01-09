#pragma once

#include "Engine/private/Define.h"
#include "Math/IntVec2.h"
#include "Math/Vec2.h"
#include "Math/Vec3.h"
#include "Math/Vec4.h"
#include "Engine/graphic/IAsyncResource.h"
#include "Engine/render/texture/TextureStream.h"
#include "RHI/RHIDefine.h"

NS_JYE_BEGIN

class ENGINE_API ITextureResource : public ISharedResource
{
private:
	TextureStreams m_TextureStreams;
	Math::IntVec2 m_vSize;//尺寸
	RHIDefine::TextureUseage m_MemoryUseage;
	RHIDefine::TextureSamplerCompare m_SamplerCompare;
	RHIDefine::WarpType m_Warp;
	RHIDefine::FilterType m_Filter;
	RHIDefine::PixelFormat m_eFormat;
	RHIDefine::TextureType m_eTextureType;
	uint m_Anisotropic;
	bool m_isKeepSource;
	bool m_isMipMap;//纹理是否使用mipmap
	bool m_isMipMapSource;//纹理的各级mipmap是否从文件中读入
	bool m_isRefrence;
	bool m_isYUV420p;
	handle m_texHandle;
	handle m_refTexHandle; //存储临时的引擎handle引用
	byte m_maxMipLevel;
	uint m_flag;
	bool m_isRTAttachment;
	bool m_isSRGB;
	bool m_isMsaa;
public:
	ITextureResource(void);
	virtual ~ITextureResource(void);

	virtual void ReleaseResource();
private:
	virtual bool _DeprecatedFilter(bool isd);
	void _CopyTextureStreams(const TextureStream& ts);
	void _CopyTextureStreams(const TextureStreams& vec);
	void _GenFlags();
public:

	void GenerateTexture(const TextureStreams& vec, const Math::IntVec2& size, RHIDefine::TextureUseage tu, RHIDefine::TextureType tt, RHIDefine::PixelFormat pf, bool keep, bool mip, uint m_uBaseLevel, uint m_uTopLevel, bool isSrgb, bool isMsaa);

	bool ChangeTextureBuffer(TextureStream& ts);
	bool ChangeSubTextureBuffer(TextureStream& ts, const Math::Vec4& region);
private:
	FORCEINLINE void _ReleaseStreams();
	FORCEINLINE void _SetMipMap(bool m, bool loaded);
	FORCEINLINE RHIDefine::TextureFilter _Refilter(RHIDefine::TextureFilter tf);
	FORCEINLINE void _FlushTextureBuffer(const TextureStreams& tss, const Math::Vec4& dirty = Math::Vec4());
	FORCEINLINE void _SubstituteTextureBuffer(const TextureStreams& tss);
	FORCEINLINE bool _NeedMipAutoGen();
	FORCEINLINE RHIDefine::TextureType _GetTextureType() const;
	FORCEINLINE RHIDefine::PixelFormat _GetPixelFormat() const;
public:
	FORCEINLINE const Math::IntVec2& GetSize() const;
	void Resize(const Math::IntVec2& size);
	FORCEINLINE handle GetHandle() const;
	FORCEINLINE uint GetFlags() const;

	FORCEINLINE void SetWarpType(const RHIDefine::WarpType& wt);
	FORCEINLINE void SetFilterType(const RHIDefine::FilterType& ft);
	FORCEINLINE void SetAnisotropic(uint ani);
	FORCEINLINE uint GetResourceHandle() const;
	FORCEINLINE bool isMipMap();
	bool ReadTexture(TextureStream* ts);
	FORCEINLINE TextureStream* GetSourceStream();
	FORCEINLINE TextureStream* GetSourceStream(uint index);
	FORCEINLINE void SetRTAttachmentFlag(bool f);
	FORCEINLINE bool IsSRGB() const;
	FORCEINLINE bool IsWrittableTexture();
	FORCEINLINE void SetSamplerCompare(RHIDefine::TextureSamplerCompare compare);
};

FORCEINLINE void ITextureResource::_SetMipMap(bool mip, bool loaded)
{
	m_isMipMapSource = loaded;
	if ((!Math::IsTwoPower(m_vSize.x)
		|| !Math::IsTwoPower(m_vSize.y)))
	{
		m_isMipMap = false;
	}
	else
	{
		m_isMipMap = mip || loaded;
	}
}

FORCEINLINE RHIDefine::TextureFilter ITextureResource::_Refilter(RHIDefine::TextureFilter tf)
{
	switch (tf)
	{
	case RHIDefine::TF_NEAREST_MIPMAP_LINEAR:
	case RHIDefine::TF_NEAREST_MIPMAP_NEAREST: return RHIDefine::TF_NEAREST;
	case RHIDefine::TF_LINEAR_MIPMAP_LINEAR:
	case RHIDefine::TF_LINEAR_MIPMAP_NEAREST: return RHIDefine::TF_LINEAR;
	}
	return tf;
}

FORCEINLINE void ITextureResource::_ReleaseStreams()
{
	for (auto ts : m_TextureStreams)
	{
		SAFE_DELETE(ts);
	}
	m_TextureStreams.clear();
}

FORCEINLINE RHIDefine::TextureType ITextureResource::_GetTextureType() const
{
	return m_eTextureType;
}

FORCEINLINE RHIDefine::PixelFormat ITextureResource::_GetPixelFormat() const
{
	return m_eFormat;
}

FORCEINLINE const Math::IntVec2& ITextureResource::GetSize() const
{
	return m_vSize;
}

FORCEINLINE handle ITextureResource::GetHandle() const
{
	// 返回临时的、引擎创建的reference handle
	// TODO: 消除掉该接口
	if (m_refTexHandle != -1)
	{
		return m_refTexHandle;
	}
	else
	{
		return m_texHandle;
	}
}

FORCEINLINE bool ITextureResource::_NeedMipAutoGen()
{
	return m_isMipMap && m_maxMipLevel == 0;
}

FORCEINLINE void ITextureResource::SetWarpType(const RHIDefine::WarpType& wt)
{
	m_Warp = wt;
	_GenFlags();
}

FORCEINLINE void ITextureResource::SetFilterType(const RHIDefine::FilterType& ft)
{
	if (m_isMipMap)
	{
		m_Filter.m_MagFilter = _Refilter(ft.m_MagFilter);
		m_Filter.m_MinFilter = ft.m_MinFilter;
	}
	else
	{
		m_Filter.m_MagFilter = _Refilter(ft.m_MagFilter);
		m_Filter.m_MinFilter = _Refilter(ft.m_MinFilter);
	}
		
	_GenFlags();
}

FORCEINLINE void ITextureResource::SetAnisotropic(uint ani)
{
	m_Anisotropic = ani;
}

FORCEINLINE uint ITextureResource::GetResourceHandle() const
{
	return m_texHandle;
}

FORCEINLINE bool ITextureResource::isMipMap()
{
	return m_isMipMap;
}

FORCEINLINE TextureStream* ITextureResource::GetSourceStream(uint index)
{
	return index > m_TextureStreams.size()
		? NULL
		: m_TextureStreams[index];
}

FORCEINLINE TextureStream* ITextureResource::GetSourceStream()
{
	return m_TextureStreams.empty()
		? NULL
		: m_TextureStreams[0];
}

FORCEINLINE uint ITextureResource::GetFlags() const
{
	return m_flag;
}

FORCEINLINE void ITextureResource::SetRTAttachmentFlag(bool f)
{
	m_isRTAttachment = f;
}

FORCEINLINE bool ITextureResource::IsSRGB() const
{
	return m_isSRGB;
}

FORCEINLINE bool ITextureResource::IsWrittableTexture()
{
	return m_MemoryUseage != RHIDefine::TU_RT_WRITE_ONLY;
}

FORCEINLINE void ITextureResource::SetSamplerCompare(RHIDefine::TextureSamplerCompare compare)
{
	m_SamplerCompare = compare;
	_GenFlags();
}

NS_JYE_END