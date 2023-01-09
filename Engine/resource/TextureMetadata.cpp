#include "TextureMetadata.h"
#include "Core/Interface/IFileSystem.h"
#include "System/Utility.hpp"
#include "Core/Interface/IResourceSystem.h"
#include "Engine/render/texture/TextureStream.h"
#include "Core/Interface/ILoader.h"
#include "Math/Math.h"

#include <sstream>

NS_JYE_BEGIN
IMPLEMENT_RTTI(TextureDescribeData, Object);
BEGIN_ADD_PROPERTY(TextureDescribeData, Object);
REGISTER_PROPERTY(m_Size, m_Size, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_eType, m_eType, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_eUsage, m_eUsage, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_ePixelFormat, m_ePixelFormat, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_eSWarp, m_eSWarp, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_eTWarp, m_eTWarp, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_eMagFilter, m_eMagFilter, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_eMinFilter, m_eMinFilter, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_uAnisotropic, m_uAnisotropic, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_isMipMap, m_isMipMap, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_uMipMapLevel, m_uMipMapLevel, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_sRGB, m_sRGB, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_isKeepSource, m_isKeepSource, Property::F_SAVE_LOAD_CLONE)
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(TextureDescribeData)
IMPLEMENT_INITIAL_END

TextureDescribeData::TextureDescribeData()
	: m_eType(RHIDefine::TT_AUTO)
	, m_eUsage(RHIDefine::TU_STATIC)
	, m_ePixelFormat(RHIDefine::PF_AUTO)
	, m_eSWarp(RHIDefine::TW_CLAMP_TO_EDGE)
	, m_eTWarp(RHIDefine::TW_CLAMP_TO_EDGE)
	, m_eMagFilter(RHIDefine::TF_LINEAR)
	, m_eMinFilter(RHIDefine::TF_LINEAR)
	, m_uAnisotropic(1)
	, m_isMipMap(false)
	, m_uMipMapLevel(0)
	, m_sRGB(true)
	, m_SamplerCompare(RHIDefine::SAMPLER_COMPARE_NONE)
	, m_isMsaa(false)
	, m_isKeepSource(false)
{
}

TextureDescribeData::TextureDescribeData(
	RHIDefine::TextureType tt,
	RHIDefine::TextureUseage mu, RHIDefine::PixelFormat pf,
	uint x, bool mip, uint level,
	RHIDefine::TextureWarp s, RHIDefine::TextureWarp t,
	RHIDefine::TextureFilter mag, RHIDefine::TextureFilter min)
	: m_eType(tt)
	, m_eUsage(mu)
	, m_ePixelFormat(pf)
	, m_eSWarp(s)
	, m_eTWarp(t)
	, m_eMagFilter(mag)
	, m_eMinFilter(min)
	, m_uAnisotropic(x)
	, m_isMipMap(mip)
	, m_uMipMapLevel(level)
	, m_sRGB(true)
	, m_SamplerCompare(RHIDefine::SAMPLER_COMPARE_NONE)
	, m_isMsaa(false)
	, m_isKeepSource(false)
{
}

TextureDescribeData::TextureDescribeData(
	const Math::IntVec2& size,
	RHIDefine::TextureType tt,
	RHIDefine::TextureUseage mu, RHIDefine::PixelFormat pf,
	uint x, bool mip, uint level,
	RHIDefine::TextureWarp s, RHIDefine::TextureWarp t,
	RHIDefine::TextureFilter mag, RHIDefine::TextureFilter min)
	: m_Size(size)
	, m_eType(tt)
	, m_eUsage(mu)
	, m_ePixelFormat(pf)
	, m_eSWarp(s)
	, m_eTWarp(t)
	, m_eMagFilter(mag)
	, m_eMinFilter(min)
	, m_uAnisotropic(x)
	, m_isMipMap(mip)
	, m_uMipMapLevel(level)
	, m_sRGB(true)
	, m_SamplerCompare(RHIDefine::SAMPLER_COMPARE_NONE)
	, m_isMsaa(false)
	, m_isKeepSource(false)
{
}

TextureDescribeData::TextureDescribeData(
	const Math::IntVec2& size,
	RHIDefine::TextureType tt,
	RHIDefine::TextureUseage mu, RHIDefine::PixelFormat pf,
	uint x, bool mip, uint level,
	RHIDefine::TextureWarp s, RHIDefine::TextureWarp t,
	RHIDefine::TextureFilter mag, RHIDefine::TextureFilter min, bool sRGB)
	: m_Size(size)
	, m_eType(tt)
	, m_eUsage(mu)
	, m_ePixelFormat(pf)
	, m_eSWarp(s)
	, m_eTWarp(t)
	, m_eMagFilter(mag)
	, m_eMinFilter(min)
	, m_uAnisotropic(x)
	, m_isMipMap(mip)
	, m_uMipMapLevel(level)
	, m_sRGB(sRGB)
	, m_SamplerCompare(RHIDefine::SAMPLER_COMPARE_NONE)
	, m_isMsaa(false)
	, m_isKeepSource(false)
{
}

TextureDescribeData::TextureDescribeData(
	RHIDefine::TextureType tt,
	RHIDefine::TextureUseage mu, RHIDefine::PixelFormat pf,
	uint x, bool mip, uint level,
	RHIDefine::TextureWarp s, RHIDefine::TextureWarp t,
	RHIDefine::TextureFilter mag, RHIDefine::TextureFilter min, bool is_sRGB)
	: m_eType(tt)
	, m_eUsage(mu)
	, m_ePixelFormat(pf)
	, m_eSWarp(s)
	, m_eTWarp(t)
	, m_eMagFilter(mag)
	, m_eMinFilter(min)
	, m_uAnisotropic(x)
	, m_isMipMap(mip)
	, m_uMipMapLevel(level)
	, m_sRGB(is_sRGB)
	, m_SamplerCompare(RHIDefine::SAMPLER_COMPARE_NONE)
	, m_isMsaa(false)
	, m_isKeepSource(false)
{
}

//
// TextureMetadata
//
IMPLEMENT_RTTI(TextureMetadata, PathMetadata);
BEGIN_ADD_PROPERTY(TextureMetadata, PathMetadata);
REGISTER_PROPERTY(m_texData, m_texData, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_Name, m_Name, Property::F_SAVE_LOAD_CLONE)
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(TextureMetadata)
IMPLEMENT_INITIAL_END

TextureMetadata::TextureMetadata()
	: m_texData(_NEW TextureDescribeData())
	, m_pTextureStreams(NULL)
{
}

TextureMetadata::TextureMetadata(
	RHIDefine::TextureType tt,
	RHIDefine::TextureUseage mu, RHIDefine::PixelFormat pf,
	uint x, bool mip, uint level,
	RHIDefine::TextureWarp s, RHIDefine::TextureWarp t,
	RHIDefine::TextureFilter mag, RHIDefine::TextureFilter min)
	: m_texData(_NEW TextureDescribeData(tt, mu, pf, x, mip, level, s, t, mag, min))
	, m_pTextureStreams(NULL)
{
}

TextureMetadata::TextureMetadata(
	const Math::IntVec2& size,
	RHIDefine::TextureType tt,
	RHIDefine::TextureUseage mu, RHIDefine::PixelFormat pf,
	uint x, bool mip, uint level,
	RHIDefine::TextureWarp s, RHIDefine::TextureWarp t,
	RHIDefine::TextureFilter mag, RHIDefine::TextureFilter min)
	: m_texData(_NEW TextureDescribeData(size, tt, mu, pf, x, mip, level, s, t, mag, min))
	, m_pTextureStreams(NULL)
{
}

TextureMetadata::TextureMetadata(
	const Math::IntVec2& size,
	RHIDefine::TextureType tt,
	RHIDefine::TextureUseage mu, RHIDefine::PixelFormat pf,
	uint x, bool mip, uint level,
	RHIDefine::TextureWarp s, RHIDefine::TextureWarp t,
	RHIDefine::TextureFilter mag, RHIDefine::TextureFilter min, bool sRGB)
	: m_texData(_NEW TextureDescribeData(size, tt, mu, pf, x, mip, level, s, t, mag, min, sRGB))
	, m_pTextureStreams(NULL)
{
}

TextureMetadata::TextureMetadata(
	RHIDefine::TextureType tt,
	RHIDefine::TextureUseage mu, RHIDefine::PixelFormat pf,
	uint x, bool mip, uint level,
	RHIDefine::TextureWarp s, RHIDefine::TextureWarp t,
	RHIDefine::TextureFilter mag, RHIDefine::TextureFilter min, bool sRGB)
	: m_texData(_NEW TextureDescribeData(tt, mu, pf, x, mip, level, s, t, mag, min, sRGB))
	, m_pTextureStreams(NULL)
{
}

void* TextureMetadata::_ReturnMetadata()
{
	return m_pTextureStreams;
}

void TextureMetadata::GetIdentifier(std::string& hash) const
{
	JY_ASSERT(false);
}

void TextureMetadata::ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash)
{
	m_pTextureStreams = _NEW TextureStreams();
	TextureStream* ts = _NEW TextureStream();
	m_pTextureStreams->push_back(ts);
	ts->SetStreamType(m_texData->m_Size, m_texData->m_ePixelFormat, false);
	m_Name = "Depth renderbuffer Texture";
}

void TextureMetadata::ReleaseMetadate()
{
	if (m_pTextureStreams)//依赖资源，不执行ProcessMetadata，所以这个变量会是空
	{
		for (TextureStream* ts : *m_pTextureStreams)
		{
			SAFE_DELETE(ts);
		}
		SAFE_DELETE(m_pTextureStreams);
	}
}

//
// TextureFileMetadata
//
IMPLEMENT_RTTI(TextureFileMetadata, TextureMetadata);
BEGIN_ADD_PROPERTY(TextureFileMetadata, TextureMetadata);
REGISTER_PROPERTY(m_Path, m_Path, Property::F_SAVE_LOAD_CLONE)
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(TextureFileMetadata)
IMPLEMENT_INITIAL_END
TextureFileMetadata::TextureFileMetadata()
	:TextureMetadata(RHIDefine::TextureType::TEXTURE_2D, RHIDefine::TextureUseage::TU_STATIC,
		RHIDefine::PixelFormat::PF_AUTO, 1, true, 0,
		RHIDefine::TextureWarp::TW_REPEAT, RHIDefine::TextureWarp::TW_REPEAT,
		RHIDefine::TextureFilter::TF_LINEAR, RHIDefine::TextureFilter::TF_LINEAR_MIPMAP_LINEAR)
	, m_pILoader(NULL)
	, m_enbaleCache(true)
{
}

TextureFileMetadata::TextureFileMetadata(
	RHIDefine::TextureType tt,
	RHIDefine::TextureUseage mu, RHIDefine::PixelFormat pf,
	uint x, bool mip, uint level,
	RHIDefine::TextureWarp s, RHIDefine::TextureWarp t,
	RHIDefine::TextureFilter mag, RHIDefine::TextureFilter min,
	const std::string& path)
	:TextureMetadata(tt, mu, pf, x, mip, level, s, t, mag, min)
	, m_pILoader(NULL)
	, m_enbaleCache(true)
{
	m_Path = path;
}

TextureFileMetadata::TextureFileMetadata(
	RHIDefine::TextureType tt,
	RHIDefine::TextureUseage mu, RHIDefine::PixelFormat pf,
	uint x, bool mip, uint level,
	RHIDefine::TextureWarp s, RHIDefine::TextureWarp t,
	RHIDefine::TextureFilter mag, RHIDefine::TextureFilter min,
	const std::string& path, bool cache, bool isSRGB)
	:TextureMetadata(tt, mu, pf, x, mip, level, s, t, mag, min, isSRGB)
	, m_pILoader(NULL)
	, m_enbaleCache(cache)
{
	m_Path = path;
}

TextureFileMetadata::~TextureFileMetadata()
{
}

void TextureFileMetadata::_Process(byte channel, const Math::IntVec2& size, byte* buff)
{
	if (RHIDefine::TW_CLAMP_TO_BORDER == m_texData->m_eSWarp)
	{
		for (int y = 0; y < size.y; ++y)
		{
			int indexb = y * size.x * channel;
			int indexe = (y * size.x + size.x - 1) * channel;
			memset(buff + indexb, 0, channel);
			memset(buff + indexe, 0, channel);
		}
	}
	if (RHIDefine::TW_CLAMP_TO_BORDER == m_texData->m_eTWarp)
	{
		for (int x = 0; x < size.x; ++x)
		{
			int indexb = x * channel;
			int indexe = ((size.y - 1) * size.x + x) * channel;
			memset(buff + indexb, 0, channel);
			memset(buff + indexe, 0, channel);
		}
	}
}

void TextureFileMetadata::GetIdentifier(std::string& hash) const
{
	if (RHIDefine::TU_STATIC == m_texData->m_eUsage)
	{
		std::string realPath = IFileSystem::Instance()->PathAssembly(m_Path);
		hash.append(realPath);
		hash.push_back(m_texData->m_eType);
		hash.push_back(m_texData->m_eSWarp);
		hash.push_back(m_texData->m_eTWarp);
		hash.push_back(m_texData->m_eMagFilter);
		hash.push_back(m_texData->m_eMinFilter);
		hash.push_back(m_texData->m_ePixelFormat);
		hash.push_back(m_texData->m_uAnisotropic);
		hash.push_back(m_texData->m_uMipMapLevel);
		hash.push_back(m_texData->m_sRGB);
	}
	else
	{
		hash.append(Utility::RandomString(64));
	}
}

void TextureFileMetadata::ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash)
{
	m_pILoader = IResourceSystem::Instance()->LoaderFactory(ResourceProperty::SLT_TEXTURE, rt, source_hash, m_Path);
	m_pTextureStreams = m_pILoader->Load<TextureStreams>(RHIDefine::TU_STATIC == m_texData->m_eUsage && m_enbaleCache);
	m_Name = m_Path;
	if (m_pTextureStreams && !m_pTextureStreams->empty())
	{
		for (TextureStream* ts : *m_pTextureStreams)
		{
			if (0 == ts->GetLevels())
			{
				m_texData->m_Size = ts->GetSize();
			}
			ts->SetTextureType(m_texData->m_eType);
			if (!ts->isCompressed())
			{
				ts->SetLevel(m_texData->m_uMipMapLevel);
				if (RHIDefine::PF_AUTO == m_texData->m_ePixelFormat)
				{
					m_texData->m_ePixelFormat = ts->GetPixelFormat();
				}
				else
				{
					ts->ConvertPixelFormat(m_texData->m_ePixelFormat);
					//序列帧可能导致后处理图片参数的pixelformat与图片实际的类型不符，临时改成这样，防止报错
					m_texData->m_ePixelFormat = ts->GetPixelFormat();
				}
				_Process(ts->GetPixelSize(), m_texData->m_Size, ts->GetBuffer());
			}
		}
	}
}

void TextureFileMetadata::ReleaseMetadate()
{
	if (m_pILoader)
	{
		m_pILoader->Release(m_pTextureStreams);
		IResourceSystem::Instance()->RecycleBin(m_pILoader);
	}
}

//
// TextureDescribeFileMetadata
//
IMPLEMENT_RTTI(TextureDescribeFileMetadata, TextureMetadata);
BEGIN_ADD_PROPERTY(TextureDescribeFileMetadata, TextureMetadata);
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(TextureDescribeFileMetadata)
IMPLEMENT_INITIAL_END

TextureDescribeFileMetadata::TextureDescribeFileMetadata()
	: m_pTextureLoader(NULL)
	, m_pMetadateLoader(NULL)
{
}

TextureDescribeFileMetadata::TextureDescribeFileMetadata(const std::string& describeFilePath)
	: m_pTextureLoader(NULL)
	, m_pMetadateLoader(NULL)
{
	m_Path = describeFilePath;
}

void TextureDescribeFileMetadata::_Process(byte channel, const Math::IntVec2& size, byte* buff)
{
	if (RHIDefine::TW_CLAMP_TO_BORDER == m_texData->m_eSWarp)
	{
		for (int y = 0; y < size.y; ++y)
		{
			int indexb = y * size.x * channel;
			int indexe = (y * size.x + size.x - 1) * channel;
			memset(buff + indexb, 0, channel);
			memset(buff + indexe, 0, channel);
		}
	}
	if (RHIDefine::TW_CLAMP_TO_BORDER == m_texData->m_eTWarp)
	{
		for (int x = 0; x < size.x; ++x)
		{
			int indexb = x * channel;
			int indexe = ((size.y - 1) * size.x + x) * channel;
			memset(buff + indexb, 0, channel);
			memset(buff + indexe, 0, channel);
		}
	}
}

void TextureDescribeFileMetadata::GetIdentifier(std::string& hash) const
{
	std::string realPath = IFileSystem::Instance()->PathAssembly(m_Path);
	hash.append(realPath);
}

void TextureDescribeFileMetadata::ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash)
{
	m_pMetadateLoader = IResourceSystem::Instance()->LoaderFactory(ResourceProperty::SLT_METADATA, rt, source_hash, m_Path);
	m_data = m_pMetadateLoader->Load<TextureDescribeData>(false);
	if (m_data)
	{
		*m_texData = *m_data;

		static const String g_metaName = ".meta";
		JY_ASSERT(m_Path.substr(m_Path.size() - g_metaName.size(), g_metaName.size()) == g_metaName);
		String texPath = m_Path.substr(0, m_Path.size() - g_metaName.size());
		m_pTextureLoader = IResourceSystem::Instance()->LoaderFactory(ResourceProperty::SLT_TEXTURE, rt, source_hash, texPath);
		m_pTextureStreams = m_pTextureLoader->Load<TextureStreams>(RHIDefine::TU_STATIC == m_texData->m_eUsage);
		m_Name = texPath;
		AddDependencePath(texPath);
		if (m_pTextureStreams && !m_pTextureStreams->empty())
		{
			for (TextureStream* ts : *m_pTextureStreams)
			{
				if (0 == ts->GetLevels())
				{
					m_texData->m_Size = ts->GetSize();
				}
				ts->SetTextureType(m_texData->m_eType);
				if (!ts->isCompressed())
				{
					ts->SetLevel(m_texData->m_uMipMapLevel);
					if (RHIDefine::PF_AUTO == m_texData->m_ePixelFormat)
					{
						m_texData->m_ePixelFormat = ts->GetPixelFormat();
					}
					else
					{
						ts->ConvertPixelFormat(m_texData->m_ePixelFormat);
						//序列帧可能导致后处理图片参数的pixelformat与图片实际的类型不符，临时改成这样，防止报错
						m_texData->m_ePixelFormat = ts->GetPixelFormat();
					}
					_Process(ts->GetPixelSize(), m_texData->m_Size, ts->GetBuffer());
				}
			}
		}
	}
}

void* TextureDescribeFileMetadata::_ReturnMetadata()
{
	return m_pTextureStreams;
}

void TextureDescribeFileMetadata::ReleaseMetadate()
{
	if (m_pMetadateLoader)
	{
		m_pMetadateLoader->Release(m_data);//这里为什么需要Release？直接Recycle的时候删除不行么？
		IResourceSystem::Instance()->RecycleBin(m_pMetadateLoader);
	}
	if (m_pTextureLoader)
	{
		m_pTextureLoader->Release(m_pTextureStreams);//这里为什么需要Release？直接Recycle的时候删除不行么？
		IResourceSystem::Instance()->RecycleBin(m_pTextureLoader);
	}
	m_pTextureLoader = NULL;
	m_pMetadateLoader = NULL;
	m_pTextureStreams = NULL;
	m_data = NULL;
}

//
// TextureRenderMetadata
//

IMPLEMENT_RTTI(TextureRenderMetadata, TextureMetadata);
BEGIN_ADD_PROPERTY(TextureRenderMetadata, TextureMetadata);
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(TextureRenderMetadata)
IMPLEMENT_INITIAL_END

TextureRenderMetadata::TextureRenderMetadata(
	const Math::IntVec2& size,
	RHIDefine::TextureType type,
	RHIDefine::TextureUseage mu, RHIDefine::PixelFormat pf,
	uint anisotropic,
	RHIDefine::TextureWarp s, RHIDefine::TextureWarp t,
	RHIDefine::TextureFilter mag, RHIDefine::TextureFilter min,
	bool isMsaa, bool isSRGB)
	: TextureMetadata(size, type, mu, pf, anisotropic, false, 0, s, t, mag, min, isSRGB)
{
	m_texData->m_isMsaa = isMsaa;
}

TextureRenderMetadata::TextureRenderMetadata(
	const Math::IntVec2& size,
	RHIDefine::TextureSamplerCompare compare,
	RHIDefine::TextureType type,
	RHIDefine::TextureUseage mu, RHIDefine::PixelFormat pf,
	uint anisotropic,
	RHIDefine::TextureWarp s, RHIDefine::TextureWarp t,
	RHIDefine::TextureFilter mag, RHIDefine::TextureFilter min,
	bool isMsaa, bool isSRGB)
	: TextureMetadata(size, type, mu, pf, anisotropic, false, 0, s, t, mag, min, isSRGB)
{
	m_texData->m_SamplerCompare = compare;
	m_texData->m_isMsaa = isMsaa;
}

TextureRenderMetadata::TextureRenderMetadata(
	const std::string& name,
	const Math::IntVec2& size,
	RHIDefine::TextureType type,
	RHIDefine::TextureUseage mu, RHIDefine::PixelFormat pf,
	uint anisotropic,
	RHIDefine::TextureWarp s, RHIDefine::TextureWarp t,
	RHIDefine::TextureFilter mag, RHIDefine::TextureFilter min,
	bool isMsaa, bool isSRGB)
	: TextureMetadata(size, type, mu, pf, anisotropic, false, 0, s, t, mag, min, isSRGB)
{
	m_Name = name;
	m_texData->m_isMsaa = isMsaa;
}

TextureRenderMetadata::~TextureRenderMetadata()
{

}

void TextureRenderMetadata::GetIdentifier(std::string& hash) const
{
	
}

void TextureRenderMetadata::ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash)
{
	m_pTextureStreams = _NEW TextureStreams();
	TextureStream* ts = _NEW TextureStream();
	m_pTextureStreams->push_back(ts);
	ts->SetStreamType(m_texData->m_Size, m_texData->m_ePixelFormat, false);
}

const Math::IntVec2& TextureRenderMetadata::GetSize()
{
	return m_texData->m_Size;
}

void TextureRenderMetadata::Resize(const Math::IntVec2& size)
{
	m_texData->m_Size = size;
}

//
// DepthRenderBufferMetadata
//
IMPLEMENT_RTTI(DepthRenderBufferMetadata, TextureMetadata);
BEGIN_ADD_PROPERTY(DepthRenderBufferMetadata, TextureMetadata);
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(DepthRenderBufferMetadata)
IMPLEMENT_INITIAL_END
DepthRenderBufferMetadata::DepthRenderBufferMetadata()
	:TextureMetadata(RHIDefine::TextureType::TEXTURE_2D, RHIDefine::TextureUseage::TU_RT_WRITE_ONLY,
		RHIDefine::PixelFormat::PF_DEPTH24_STENCIL8, 1, false, 0,
		RHIDefine::TextureWarp::TW_REPEAT, RHIDefine::TextureWarp::TW_REPEAT,
		RHIDefine::TextureFilter::TF_LINEAR, RHIDefine::TextureFilter::TF_LINEAR_MIPMAP_LINEAR)
{
}

static bool IsDepthTexture(RHIDefine::PixelFormat pf)
{
	if (pf == RHIDefine::PF_DEPTH24_STENCIL8 ||
		pf == RHIDefine::PF_DEPTH16 ||
		pf == RHIDefine::PF_DEPTH32)
	{
		return true;
	}
	else
	{
		return false;
	}
}

DepthRenderBufferMetadata::DepthRenderBufferMetadata(
	const Math::IntVec2& size,
	RHIDefine::PixelFormat pf,
	RHIDefine::TextureWarp s, RHIDefine::TextureWarp t,
	RHIDefine::TextureFilter mag, RHIDefine::TextureFilter min)
	:TextureMetadata(size, RHIDefine::TextureType::TEXTURE_2D, RHIDefine::TextureUseage::TU_RT_WRITE_ONLY, pf, 1, false, 0, s, t, mag, min)
{
	if (false == IsDepthTexture(pf))
	{
		JYERROR("DepthRenderBuffer: Attempt to create DepthRenderBuffer with format %d.", (int)pf);
		JY_ASSERT(false);
	}
}

DepthRenderBufferMetadata::~DepthRenderBufferMetadata()
{
}

void DepthRenderBufferMetadata::GetIdentifier(std::string& hash) const
{
	hash.append("DepthRenderBuffer");
	
	{
		std::stringstream ss;
		ss << m_texData->m_Size.x << m_texData->m_Size.y;
		hash.append(ss.str());
		hash.push_back(m_texData->m_eSWarp);
		hash.push_back(m_texData->m_eTWarp);
		hash.push_back(m_texData->m_eMagFilter);
		hash.push_back(m_texData->m_eMinFilter);
		hash.push_back(m_texData->m_ePixelFormat);
		hash.push_back(m_texData->m_uAnisotropic);
	}
}

void DepthRenderBufferMetadata::ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash)
{
	m_pTextureStreams = _NEW TextureStreams();
	TextureStream* ts = _NEW TextureStream();
	m_pTextureStreams->push_back(ts);
	ts->SetStreamType(m_texData->m_Size, m_texData->m_ePixelFormat, false);
	m_Name = "Depth renderbuffer Texture";
}

//
// TextureBufferMetadata
//
IMPLEMENT_RTTI(TextureBufferMetadata, TextureMetadata);
BEGIN_ADD_PROPERTY(TextureBufferMetadata, TextureMetadata);
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(TextureBufferMetadata)
IMPLEMENT_INITIAL_END
static byte _ChannelMapping(RHIDefine::PixelFormat pf)
{
	switch (pf)
	{
	case RHIDefine::PF_A8: return 1;
	case RHIDefine::PF_L8: return 1;
	case RHIDefine::PF_L8A8: return 2;
	case RHIDefine::PF_R8G8B8: return 3;
	case RHIDefine::PF_R5G6B5: return 3;
	case RHIDefine::PF_R8G8B8A8: return 4;
	case RHIDefine::PF_R4G4B4A4: return 4;
	case RHIDefine::PF_DEPTH16: return 2;
	case RHIDefine::PF_DEPTH32: return 4;
	case RHIDefine::PF_DEPTH24_STENCIL8: return 4;
	case RHIDefine::PF_RGBAFLOAT: return 16;
	case RHIDefine::PF_RGBAHALF: return 8;
	case RHIDefine::PF_RG11B10FLOAT: return 4;
	default: JYERROR("ChannelMapping Error : unknown pixel format");
	}
	return 0;
}

TextureBufferMetadata::TextureBufferMetadata()
	:TextureMetadata(RHIDefine::TextureType::TEXTURE_2D, RHIDefine::TextureUseage::TU_RT_WRITE_ONLY,
		RHIDefine::PixelFormat::PF_DEPTH24_STENCIL8, 1, false, 0,
		RHIDefine::TextureWarp::TW_REPEAT, RHIDefine::TextureWarp::TW_REPEAT,
		RHIDefine::TextureFilter::TF_LINEAR, RHIDefine::TextureFilter::TF_LINEAR_MIPMAP_LINEAR)
	, m_rpBuffer(nullptr)
{
	JYLOG("never be call");
}

TextureBufferMetadata::TextureBufferMetadata(const Math::IntVec2& size)
	:TextureMetadata(size, RHIDefine::TextureType::TEXTURE_2D, RHIDefine::TextureUseage::TU_STATIC,
		RHIDefine::PixelFormat::PF_R8G8B8A8, 1, false, 0,
		RHIDefine::TextureWarp::TW_REPEAT, RHIDefine::TextureWarp::TW_REPEAT,
		RHIDefine::TextureFilter::TF_NEAREST, RHIDefine::TextureFilter::TF_NEAREST)
	, m_rpBuffer(nullptr)
{
	JYLOG("never be call");
}

TextureBufferMetadata::TextureBufferMetadata(
	const Math::IntVec2& size,
	RHIDefine::TextureType type,
	RHIDefine::TextureUseage mu, RHIDefine::PixelFormat pf,
	uint anisotropic, bool mip, uint level,
	RHIDefine::TextureWarp s, RHIDefine::TextureWarp t,
	RHIDefine::TextureFilter mag, RHIDefine::TextureFilter min,
	byte* buf)
	: TextureMetadata(size, type, mu, pf, anisotropic, mip, level, s, t, mag, min)
	, m_rpBuffer(buf)
{

}

TextureBufferMetadata::TextureBufferMetadata(
	const Math::IntVec2& size,
	RHIDefine::TextureType type,
	RHIDefine::TextureUseage mu, RHIDefine::PixelFormat pf,
	uint anisotropic, bool mip, uint level,
	RHIDefine::TextureWarp s, RHIDefine::TextureWarp t,
	RHIDefine::TextureFilter mag, RHIDefine::TextureFilter min,
	byte* buf, bool is_sRGB)
	: TextureMetadata(size, type, mu, pf, anisotropic, mip, level, s, t, mag, min, is_sRGB)
	, m_rpBuffer(buf)
{
}

TextureBufferMetadata::TextureBufferMetadata(
	const Math::IntVec2& size,
	RHIDefine::TextureType type,
	RHIDefine::TextureUseage mu, RHIDefine::PixelFormat pf,
	uint anisotropic, bool mip, uint level,
	RHIDefine::TextureWarp s, RHIDefine::TextureWarp t,
	RHIDefine::TextureFilter mag, RHIDefine::TextureFilter min,
	TextureStream* ts)
	: TextureMetadata(size, type, mu, pf, anisotropic, mip, level, s, t, mag, min)
	, m_rpBuffer(ts->GetBuffer())
{
}

TextureBufferMetadata::TextureBufferMetadata(
	const Math::IntVec2& size,
	RHIDefine::TextureType type,
	RHIDefine::TextureUseage mu, RHIDefine::PixelFormat pf,
	uint anisotropic, bool mip, uint level,
	RHIDefine::TextureWarp s, RHIDefine::TextureWarp t,
	RHIDefine::TextureFilter mag, RHIDefine::TextureFilter min,
	TextureStream* ts, bool is_sRGB)
	: TextureMetadata(size, type, mu, pf, anisotropic, mip, level, s, t, mag, min, is_sRGB)
	, m_rpBuffer(ts->GetBuffer())
{
}

TextureBufferMetadata::~TextureBufferMetadata()
{

}

void TextureBufferMetadata::GetIdentifier(std::string& hash) const
{
	uint size = m_texData->m_Size.x * m_texData->m_Size.y * _ChannelMapping(m_texData->m_ePixelFormat);
	if (RHIDefine::TU_STATIC == m_texData->m_eUsage
		&& NULL != m_rpBuffer)
	{
		uint res = Math::MathInstance::GetMathInstance().CRC32Compute(m_rpBuffer, size);
		std::stringstream ss;
		ss << res;
		hash.append(ss.str());
		hash.push_back(m_texData->m_eSWarp);
		hash.push_back(m_texData->m_eTWarp);
		hash.push_back(m_texData->m_eMagFilter);
		hash.push_back(m_texData->m_eMinFilter);
		hash.push_back(m_texData->m_ePixelFormat);
		hash.push_back(m_texData->m_uAnisotropic);
		hash.push_back(m_texData->m_sRGB);
	}
	else
	{
		hash.append(Utility::RandomString(64));
	}
}

void TextureBufferMetadata::ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash)
{
	m_pTextureStreams = _NEW TextureStreams();
	TextureStream* ts = _NEW TextureStream();
	m_pTextureStreams->push_back(ts);
	ts->SetStreamType(m_texData->m_Size, m_texData->m_ePixelFormat, NULL != m_rpBuffer);
	if (m_rpBuffer)
	{
		ts->SetBuffer(m_rpBuffer);
	}
	m_Name = "Buffer Texture";
}

NS_JYE_END
