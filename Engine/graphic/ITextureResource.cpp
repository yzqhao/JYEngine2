#include "ITextureResource.h"
#include "RHI/RHI.h"
#include "Engine/render/texture/TextureFormat.h"
#include "Engine/ProjectSetting.h"

NS_JYE_BEGIN

ITextureResource::ITextureResource(void)
	: ISharedResource(ResourceProperty::SRT_TEXTURE)
	, m_Anisotropic(0)
	, m_eFormat((RHIDefine::PixelFormat)-1)
	, m_eTextureType((RHIDefine::TextureType)-1)
	, m_MemoryUseage((RHIDefine::TextureUseage)-1)
	, m_texHandle(-1)
	, m_isMipMap(false)
	, m_isMipMapSource(false)
	, m_isRefrence(false)
	, m_isKeepSource(false)
	, m_maxMipLevel(0)
	, m_flag(0)
	, m_isYUV420p(false)
	, m_isSRGB(false)
	, m_refTexHandle(-1)
	, m_SamplerCompare(RHIDefine::SAMPLER_COMPARE_NONE)
	, m_isMsaa(false)
{
	m_Warp.m_SWarp = (RHIDefine::TextureWarp)-1;
	m_Warp.m_TWarp = (RHIDefine::TextureWarp)-1;
	m_Filter.m_MagFilter = (RHIDefine::TextureFilter)-1;
	m_Filter.m_MinFilter = (RHIDefine::TextureFilter)-1;
}

ITextureResource::~ITextureResource(void)
{
	_ReleaseStreams();
}

bool ITextureResource::_DeprecatedFilter(bool isd)
{
	return false;
}

void ITextureResource::_GenFlags()
{
	m_flag = RHIGenTextureFlags(m_Warp, m_Filter, m_SamplerCompare);
}

void ITextureResource::ReleaseResource()
{
	if (nullhandle != m_texHandle)
	{
		RHIDestoryTexture(m_texHandle);
	}
	m_refTexHandle = -1;
	m_texHandle = -1;
}

void ITextureResource::_CopyTextureStreams(const TextureStreams& vec)
{
	int oricount = m_TextureStreams.size();
	for (int i = 0; i < oricount; ++i)
	{
		TextureStream* lhs = m_TextureStreams[i];
		TextureStream* rhs = vec[i];
		lhs->Merge(*rhs);
	}
	for (int i = oricount; i < vec.size(); ++i)
	{
		TextureStream* ts = _NEW TextureStream(*vec[i]);
		m_TextureStreams.push_back(ts);
	}
}

void ITextureResource::_CopyTextureStreams(const TextureStream& ts)
{
	if (m_TextureStreams.empty())
	{
		m_TextureStreams.push_back(_NEW TextureStream(ts));
	}
	else
	{
		m_TextureStreams.front()->Merge(ts);
	}
}

handle CreateTexture(RHIDefine::PixelFormat format, uint64 _flags, const Math::IntVec2& size, RHIDefine::TextureType textureType, bool hasMipMap,
	bool autogenMipmap, bool isRT, int mipNums, RHIDefine::TextureUseage usage, bool issRGBSpace, bool msaa)
{
	handle textureHandle = -1;
	size_t width = size.x;
	size_t height = size.y;

	if (textureType == RHIDefine::TEXTURE_2D)
	{
		textureHandle = RHICreateTexture2D(width, height, hasMipMap, 1, format, usage, _flags, nullptr, 0, autogenMipmap, isRT, issRGBSpace, autogenMipmap ? 0 : mipNums, msaa);
	}
	else if (textureType == RHIDefine::TEXTURE_CUBE_MAP)
	{
		textureHandle = RHICreateTextureCube(uint16_t(width), hasMipMap, 1, format, _flags, nullptr, 0, autogenMipmap, autogenMipmap ? 0 : mipNums);
	}

	return textureHandle;
}

byte CheckMaxMipLevel(const TextureStreams& tss)
{
	byte maxMipLevel = 0;
	for (int i = 0; i < tss.size(); ++i)
	{
		TextureStream* ts = tss[i];
		byte curMipLevel = ts->GetLevels();
		if (maxMipLevel < curMipLevel)
		{
			maxMipLevel = curMipLevel;
		}
	}

	return maxMipLevel;
}

void ITextureResource::GenerateTexture(const TextureStreams& vec, const Math::IntVec2& size, RHIDefine::TextureUseage tu, RHIDefine::TextureType tt,
	RHIDefine::PixelFormat pf, bool keep, bool mip, uint baseLevel, uint topLevel, bool _isSRGB, bool msaa)
{
	m_MemoryUseage = tu;
	m_eTextureType = tt;

	m_isMsaa = msaa;
	m_eFormat = pf;
	m_vSize = size;
	m_isKeepSource = keep;
	_SetMipMap(mip, topLevel > baseLevel);
	//reflush
	SetWarpType(m_Warp);
	SetFilterType(m_Filter);
	SetSamplerCompare(m_SamplerCompare);

	m_maxMipLevel = CheckMaxMipLevel(vec);
	m_isSRGB = _isSRGB;
	// check srgb setting, whether specified format is supported.
	m_isSRGB = m_isSRGB && ProjectSetting::Instance()->IsCurrentAdvancedShading() && 
		RHIIsSupportsSRGB() && RHIIsTextureFormatSupportSRGB(m_eFormat);

	m_texHandle = CreateTexture(_GetPixelFormat(), m_flag, m_vSize, m_eTextureType, m_isMipMap, _NeedMipAutoGen(), m_isRTAttachment, m_maxMipLevel + 1, m_MemoryUseage, m_isSRGB, m_isMsaa);

	if (m_isKeepSource)
	{
		_CopyTextureStreams(vec);
	}

	_FlushTextureBuffer(vec);
}

bool ITextureResource::ChangeTextureBuffer(TextureStream& ts)
{
	if (RHIDefine::TU_WRITE != m_MemoryUseage)
	{
		JYLOG("texture can't be witre!");
	}
	else
	{
		TextureStreams tss;
		tss.push_back(&ts);
		_SubstituteTextureBuffer(tss);
		if (m_isKeepSource)
		{
			_CopyTextureStreams(ts);
		}
		return true;
	}
	return false;
}

bool ITextureResource::ChangeSubTextureBuffer(TextureStream& ts, const Math::Vec4& region)
{
	if (RHIDefine::TU_WRITE != m_MemoryUseage)
	{
		JYLOG("texture can't be witre!");
	}
	else
	{
		TextureStreams tss;
		tss.push_back(&ts);
		_FlushTextureBuffer(tss, region);
		return true;
	}
	return false;
}

void ReleaseTextureStreamFunc(void* _ptr, void* _userData)
{
	TextureStream* ts = (TextureStream*)_userData;
	SAFE_DELETE(ts);
};

void ITextureResource::_FlushTextureBuffer(const TextureStreams& tss, const Math::Vec4& dirty)
{
	if (tss.empty())
	{
		JYLOG("fail to flush texture");
		return;
	}

	if (false == IsWrittableTexture())
	{
		if (!m_isKeepSource)
		{
			_ReleaseStreams();
		}
		JYLOG("Attempt to flush RT_WRITE_ONLY texture");
		return;
	}

	// <Mark texture stream kept or not>
	bool keepMemSource = (m_isKeepSource || RHIDefine::TU_WRITE == m_MemoryUseage);

	RHIDefine::TextureType texType = _GetTextureType();
	size_t width = m_vSize.x;
	size_t height = m_vSize.y;

	if (texType == RHIDefine::TEXTURE_2D)
	{
		for (int i = 0; i < tss.size(); ++i)
		{
			TextureStream* ts = tss[i];
			const Math::IntVec2& size = ts->GetSize();

			int bufferSize = TextureFormat::GetMemSize(size.x, size.y, 1, m_eFormat);
			int streamBufSize = ts->GetBufferSize();
			if (streamBufSize < bufferSize)
			{
				JYERROR("ITextureResource: Too small buffer size of TextureStream during flush!");
				continue;
			}
			byte* buffer = ts->GetBuffer();
			size_t memSize = TextureFormat::GetMemSize(size.x, size.y
				, 1, _GetPixelFormat());
			TextureStream* memts = NULL;
			RHIReleaseFn memRelease = RHIReleaseFn(0);
			if (!keepMemSource)
			{
				memts = _NEW TextureStream(std::move(*ts));
				memRelease = ReleaseTextureStreamFunc;
				buffer = memts->GetBuffer();
			}

			if (dirty == Math::Vec4(0, 0, 0, 0))
			{
				if (size.x != 0 && size.y != 0)
				{
					RHIUpdateTexture2D(m_texHandle, 0, ts->GetLevels(), 0, 0,
						uint16_t(size.x), uint16_t(size.y),
						buffer, memSize, memRelease, memts);
				}
				else
				{
					JYERROR("_FlushTextureBuffer: texture data is null");
					if (memRelease != RHIReleaseFn(0))
					{
						memRelease(buffer, memts);
					}
					continue;
				}
			}
			else
			{
				RHIUpdateTexture2D(m_texHandle, 0, ts->GetLevels(), uint16_t(dirty.x), uint16_t(dirty.y),
					uint16_t(dirty.z), uint16_t(dirty.w), buffer, memSize);
			}
		}
	}
	else if (texType == RHIDefine::TEXTURE_CUBE_MAP)
	{
		for (int i = 0; i < tss.size(); ++i)
		{
			TextureStream* ts = tss[i];
			const Math::IntVec2& size = ts->GetSize();

			int bufferSize = TextureFormat::GetMemSize(size.x, size.y, 1, m_eFormat);
			int streamBufSize = ts->GetBufferSize();
			if (streamBufSize < bufferSize)
			{
				JYERROR("ITextureResource: Too small buffer size of TextureStream during flush!");
				continue;
			}
			byte* buffer = ts->GetBuffer();
			size_t memSize = TextureFormat::GetMemSize(size.x, size.y
				, 1, _GetPixelFormat());
			TextureStream* memts = NULL;
			RHIReleaseFn memRelease = RHIReleaseFn(0);
			if (!keepMemSource)
			{
				memts = _NEW TextureStream(std::move(*ts));
				memRelease = ReleaseTextureStreamFunc;
				buffer = memts->GetBuffer();
			}

			if (dirty == Math::Vec4(0, 0, 0, 0))
			{
				if (size.x != 0 && size.y != 0)
				{
					RHIUpdateTextureCube(m_texHandle, 0, ts->GetTextureType(), ts->GetLevels(),
						0, 0, size.x, size.y, buffer, memSize, memRelease, memts);
				}
				else
				{
					JYERROR("_FlushTextureBuffer: texture data is null");
					if (memRelease != RHIReleaseFn(0))
					{
						memRelease(buffer, memts);
					}
					continue;
				}
			}
			else
			{
				RHIUpdateTextureCube(m_texHandle, 0, ts->GetTextureType(), ts->GetLevels(),
					dirty.x, dirty.y, dirty.z, dirty.w, buffer, memSize, memRelease, memts);
			}
		}
	}
	else
	{
		JYLOG("Unkown texture type!");
	}

	if (_NeedMipAutoGen())
	{
		//暂时去掉纹理的自动创建,bgfx不支持
		//_DoGenerateMipMap();
	}

	if (!keepMemSource)
	{
		_ReleaseStreams();
	}
}

void ITextureResource::_SubstituteTextureBuffer(const TextureStreams& tss)
{
	if (tss.empty())
	{
		JYLOG("fail to substitute texture");
		return;
	}

	if (m_isRefrence)
	{
		JYLOG("fail to substitute texture for texture refrence type");
		return;
	}

	TextureStream* first = tss.front();
	Resize(first->GetSize());
	_FlushTextureBuffer(tss);
}

void ITextureResource::Resize(const Math::IntVec2& size)
{
	if (!m_isRefrence
		&& m_vSize != size)
	{
		m_vSize = size;

		// 由于vulkan不支持，把之前的resize方法暂时去掉，改用删除再次创建的方式
		if (m_texHandle != nullhandle)
		{
			RHIDestoryTexture(m_texHandle);
			m_texHandle = nullhandle;
		}

		m_texHandle = CreateTexture(_GetPixelFormat(), m_flag, m_vSize, m_eTextureType, m_isMipMap, _NeedMipAutoGen(), m_isRTAttachment, m_maxMipLevel + 1, m_MemoryUseage, m_isSRGB, m_isMsaa);

		if (_NeedMipAutoGen())
		{
			// bgfx 暂时不支持mipmap autogen
			JYWARNING("performance is low, when generate mipmap during runtime");
		}
	}
}

bool ITextureResource::ReadTexture(TextureStream* ts)
{
	return false;
}

NS_JYE_END