#include "RenderTargetEntity.h"
#include "../resource/RenderTargetMetadata.h"
#include "../resource/TextureMetadata.h"

NS_JYE_BEGIN

IMPLEMENT_RTTI(RenderTargetSource, ISharedSource);
BEGIN_ADD_PROPERTY(RenderTargetSource, ISharedSource);
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(RenderTargetSource)
IMPLEMENT_INITIAL_END

RenderTargetSource::RenderTargetSource(void)
	: ISharedSource(ResourceProperty::SRT_RENDERTARGET, true)
	, m_eTargetType(RHIDefine::RT_RENDER_TARGET_2D)
	, m_RefrenceHandle(nullhandle)
	, m_gWindow((void*)nullhandle)
	, m_Scale(1.0f, 1.0f)
{
}

RenderTargetSource::~RenderTargetSource(void)
{
}

TextureEntity* RenderTargetSource::MakeTextureAttachment(RHIDefine::RenderTargetAttachment rta)
{
	Pointer<TextureEntity> tex = Pointer<TextureEntity>(_NEW TextureEntity);
	tex->DependenceSource(this);
	m_TextureBufferMap.insert({ rta, tex });
	return tex;
}

void RenderTargetSource::Resizeview(const ViewResolution& vr)
{
	RenderTargetMetadata* meta = static_cast<RenderTargetMetadata*>(*GetSourceMetadata().begin());
	if (meta != NULL) {
		meta->Resizeview(vr);
	}
	else {
		JYERROR("RenderTargetSource Resizeview meta null");
	}
}

bool RenderTargetSource::_DoLoadResource()
{
	(*GetSourceMetadata().begin())->ProcessMetadata(GetResourceType(), GetHashCode());
	RenderTargetData* meta
		= (*GetSourceMetadata().begin())->GetMetadata<RenderTargetData>();
	m_Name = meta->m_Name;
	m_RefrenceHandle = meta->m_Resource;
	m_Scale = meta->m_Scale;
	SetViewPort(meta->m_ViewPort);
	SetSize(meta->m_Size);
	SetTargetType(meta->m_eTargetType);
	SetWinHandle(meta->m_gWindow);

	for (auto pair : meta->m_Textures)
	{
		TextureEntity* tex = MakeTextureAttachment(pair.first);
		tex->PushMetadata(
			TextureRenderMetadata(
				m_Name,
				meta->m_Size,
				pair.second.m_eTextureType,
				pair.second.m_eTextureUseage,
				pair.second.m_ePixelFormat,
				1,
				pair.second.m_eVWarp,
				pair.second.m_eHWarp,
				pair.second.m_eMagFilter,
				pair.second.m_eMinFilter,
				false,
				meta->m_isSrgb));
		tex->SetRTAttachmentFlag(true);
		tex->CreateResource();
	}
	return true;
}

void RenderTargetSource::CreateAttachment()
{
	for(auto it : m_TextureBufferMap)
	{
		if (NULL != it.second)
		{
			it.second->SetRTAttachmentFlag(true);
			it.second->CreateResource();
		}
	}
}

IAsyncResource* RenderTargetSource::_DoCreateResource()
{
	return _NEW RenderTargetResource();
}

bool RenderTargetSource::_DoSetupResource(IAsyncResource* res)
{
	RenderTargetResource* rtr = static_cast<RenderTargetResource*>(res);
	rtr->SetViewPort(m_ViewPort);

	if (nullhandle != m_RefrenceHandle)
	{
		rtr->RefranceRenderTarget(m_RefrenceHandle, m_eTargetType, m_Size, m_Scale);
	}
	else
	{
		rtr->GenerateRenderTarget(m_eTargetType, m_Size, m_Scale, (void*)m_gWindow);
	}

	for(auto& it : m_TextureBufferMap)
	{
		if (NULL != it.second)
		{
			JY_ASSERT((it.second->GetTextureType() == (RHIDefine::RT_RENDER_TARGET_2D == m_eTargetType ? RHIDefine::TEXTURE_2D : RHIDefine::TEXTURE_CUBE_MAP)));
			if (it.second->GetSize() != m_Size)
			{
				//由于共享纹理的原因会导致这个问题（共享前给相同的size，但是共享纹理被resize，导致的拥有相同hash但是不同的size问题）
				JYERROR("fbo source: different size between fbo and texture");
			}
		}
		rtr->SetAttachment(it.first, it.second);
	}

	return true;
}

//
// RenderTargetEntity
//
IMPLEMENT_RTTI(RenderTargetEntity, IAsyncEntity);
BEGIN_ADD_PROPERTY(RenderTargetEntity, IAsyncEntity);
REGISTER_PROPERTY(m_spSource, m_spSource, Property::F_SAVE_LOAD_CLONE)
END_ADD_PROPERTY

ENTITY_FACTORY_FUNC(RenderTargetEntity)
IMPLEMENT_INITIAL_NO_FACTORY_BEGIN(RenderTargetEntity)
IMPLEMENT_INITIAL_END

RenderTargetEntity::RenderTargetEntity()
	: IAsyncEntity()
{

}

RenderTargetEntity::~RenderTargetEntity()
{

}

void RenderTargetEntity::Resizeview(const ViewResolution& vr)
{
	if (m_enableResizeview)
	{
		_GetSourcePtr()->Resizeview(vr);
		if (_GetSourcePtr()->isSuccess())
		{
			auto res = _GetResourcePtr();
			if (res != NULL)
			{
				res->Resizeview(vr);
			}
			else
			{
				JYERROR("RenderTargetEntity::Resizeview res is null");
			}

		}
	}
}

void RenderTargetEntity::_OnCreateResource()
{
	_GetSourcePtr()->CreateAttachment();
}

TextureEntity* RenderTargetEntity::MakeTextureAttachment(RHIDefine::RenderTargetAttachment rta)
{
	return _GetSourcePtr()->MakeTextureAttachment(rta);
}

void RenderTargetEntity::SetResizeviewFlag(bool flag)
{
	m_enableResizeview = flag;
}

void RenderTargetEntity::BeforeSave(Stream* pStream)
{

}

void RenderTargetEntity::PostSave(Stream* pStream)
{

}

void RenderTargetEntity::PostLoad(Stream* pStream)
{
	this->CreateResource();
}

NS_JYE_END
