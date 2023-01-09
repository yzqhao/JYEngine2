#include "TextureEntity.h"
#include "Core/Interface/ILogSystem.h"
#include "Engine/resource/TextureMetadata.h"

NS_JYE_BEGIN

//
// TextureSource
//
IMPLEMENT_RTTI(TextureSource, ISharedSource);
BEGIN_ADD_PROPERTY(TextureSource, ISharedSource);
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(TextureSource)
IMPLEMENT_INITIAL_END

TextureSource::TextureSource() 
	: ISharedSource(ResourceProperty::SRT_TEXTURE, true)
	, m_Anisotropic(0)
	, m_Usage(RHIDefine::TU_STATIC)
	, m_eTextureType(RHIDefine::TT_AUTO)
	, m_eFormat(RHIDefine::PF_AUTO)
	, m_isLoaded(false)
	, m_isMipMap(false)
	, m_uBaseLevel(0)
	, m_uTopLevel(0)
	, m_isKeepSource(false)
	, m_hInputHandle(nullhandle)
	, m_allocDefaultTextureStream(false)
	, m_isRTAttachment(false)
	, m_isSRGB(false)
	, m_isInternelReference(false)
	, m_SamplerCompare(RHIDefine::SAMPLER_COMPARE_NONE)
	, m_isMsaa(false)
{

}

TextureSource::~TextureSource()
{

}

void TextureSource::SetSize(const Math::IntVec2& size)
{
	m_Size = size;
	const ISourceMetadataList& metas = GetSourceMetadata();
	for (auto& it : metas)
	{
		if ((it)->IsDerived(TextureRenderMetadata::RTTI()))
		{
			TextureRenderMetadata* trm = static_cast<TextureRenderMetadata*>(it);
			trm->Resize(size);
		}
	}
}

static RHIDefine::TextureType _TextureType(RHIDefine::TextureType tt)
{
	switch (tt)
	{
	case RHIDefine::TEXTURE_1D:
	case RHIDefine::TEXTURE_2D:
	case RHIDefine::TEXTURE_3D:
	case RHIDefine::TEXTURE_CUBE_MAP: return tt;
	case RHIDefine::TT_TEXTURECUBE_FRONT:
	case RHIDefine::TT_TEXTURECUBE_BACK:
	case RHIDefine::TT_TEXTURECUBE_TOP:
	case RHIDefine::TT_TEXTURECUBE_BOTTOM:
	case RHIDefine::TT_TEXTURECUBE_LEFT:
	case RHIDefine::TT_TEXTURECUBE_RIGHT: return RHIDefine::TEXTURE_CUBE_MAP;
	}
	JYERROR("unkown texture type");
	return RHIDefine::TEXTURE_2D;
}

bool TextureSource::_DoLoadResource()
{
	bool res = true;
	int curAreaSize = -1;

	const ISourceMetadataList& metas = GetSourceMetadata();
	for (IMetadata* m : metas)
	{
		res &= m->IsDerived(TextureMetadata::ms_Type);
		if (res)
		{
			TextureMetadata* texmeta = static_cast<TextureMetadata*>(m);
			texmeta->ProcessMetadata(GetResourceType(), GetHashCode());
			Math::IntVec2 texMetaSize = texmeta->m_texData->m_Size;
			int metaAreaSize = texMetaSize.x * texMetaSize.y;
			if (curAreaSize < metaAreaSize)
			{
				curAreaSize = metaAreaSize;
				m_Size = texmeta->m_texData->m_Size;
				m_Name = texmeta->m_Name;
			}

			if (RHIDefine::TT_AUTO == m_eTextureType)
			{
				m_eTextureType = _TextureType(texmeta->m_texData->m_eType);
			}

			if (m_Size.x <= 0)
			{
				m_Size.x = 1;
			}
			if (m_Size.y <= 0)
			{
				m_Size.y = 1;
			}
			SetMipMap(texmeta->m_texData->m_isMipMap, texmeta->m_texData->m_uMipMapLevel);
			SetFilter(texmeta->m_texData->m_eMagFilter, texmeta->m_texData->m_eMinFilter);
			SetTextureUsage(texmeta->m_texData->m_eUsage);
			SetWarp(texmeta->m_texData->m_eSWarp, texmeta->m_texData->m_eTWarp);
			SetPixelFormat(texmeta->m_texData->m_ePixelFormat);
			SetAnisotropic(texmeta->m_texData->m_uAnisotropic);
			SetSamplerCompare(texmeta->m_texData->m_SamplerCompare);
			m_isMsaa = texmeta->m_texData->m_isMsaa;
			m_isSRGB = texmeta->m_texData->m_sRGB;

			if (texmeta->m_texData->m_isKeepSource)
			{
				SetKeepSource(texmeta->m_texData->m_isKeepSource);
			}

			TextureStreams* tss = texmeta->GetMetadata<TextureStreams>();
			if (tss && !tss->empty())
			{
				m_TextureStreams.insert(m_TextureStreams.end(), tss->begin(), tss->end());
			}
			else
			{
				res = false;
			}
		}
		else
		{
			JYERROR("type of metadata %s is not supported", m->GetType().GetName());
		}
	}
	return res;
}

void TextureSource::_DoReleaseUnnecessaryDataAfterLoaded()
{
	m_TextureStreams.clear();
}

IAsyncResource* TextureSource::_DoCreateResource()
{
	JY_ASSERT(false == m_isLoaded);
	return _NEW ITextureResource();
}

bool TextureSource::_DoSetupResource(IAsyncResource* res)
{
	ITextureResource* texres = static_cast<ITextureResource*>(res);
	
	{
		texres->SetRTAttachmentFlag(m_isRTAttachment);
		texres->GenerateTexture(m_TextureStreams, m_Size, m_Usage, m_eTextureType, m_eFormat, m_isKeepSource, m_isMipMap, m_uBaseLevel, m_uTopLevel, IsSRGB(), m_isMsaa);
		texres->SetAnisotropic(m_Anisotropic);
		texres->SetFilterType(m_Filter);
		texres->SetWarpType(m_Warp);
		texres->SetSamplerCompare(m_SamplerCompare);
	}

	m_isLoaded = true;
	return true;
}

//
// TextureEntity
//
IMPLEMENT_RTTI(TextureEntity, IAsyncEntity);
BEGIN_ADD_PROPERTY(TextureEntity, IAsyncEntity);
REGISTER_PROPERTY(m_spSource, m_spSource, Property::F_SAVE_LOAD_CLONE)
END_ADD_PROPERTY

ENTITY_FACTORY_FUNC(TextureEntity)
IMPLEMENT_INITIAL_NO_FACTORY_BEGIN(TextureEntity)
IMPLEMENT_INITIAL_END

TextureEntity::TextureEntity()
{

}

TextureEntity::~TextureEntity()
{

}

void TextureEntity::Resize(const Math::IntVec2& rsl)
{
	_GetSourcePtr()->SetSize(rsl);
	if (_GetResourcePtr())
		_GetResourcePtr()->Resize(rsl);
}

void TextureEntity::BeforeSave(Stream* pStream)
{

}

void TextureEntity::PostSave(Stream* pStream)
{

}

void TextureEntity::PostLoad(Stream* pStream)
{
	this->CreateResource();
}

NS_JYE_END