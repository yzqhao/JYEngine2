#include "VertexBufferEntity.h"
#include "Engine/resource/VertexMetadata.h"

NS_JYE_BEGIN

IMPLEMENT_RTTI(VertexBufferSource, ISharedSource);
BEGIN_ADD_PROPERTY(VertexBufferSource, ISharedSource);
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(VertexBufferSource)
IMPLEMENT_INITIAL_END

VertexBufferSource::VertexBufferSource(void)
	: ISharedSource(ResourceProperty::SRT_VERTEXBUFFER, true)
{
}

VertexBufferSource::~VertexBufferSource(void)
{
}

bool VertexBufferSource::_DoLoadResource()
{
	VertexMetadata* data = static_cast<VertexMetadata*>((*GetSourceMetadata().begin()));
	data->ProcessMetadata(GetResourceType(), GetHashCode());//处理加载原始数据
	m_eMemoryUseage = data->GetMemoryUseage();
	VertexStream* vertex = data->GetMetadata<VertexStream>();
	return NULL != vertex;
}

IAsyncResource* VertexBufferSource::_DoCreateResource()
{
	return _NEW VertexBufferResource();
}

bool VertexBufferSource::_DoSetupResource(IAsyncResource* res)
{
	VertexBufferResource* vbr = static_cast<VertexBufferResource*>(res);
	VertexStream* vertex = (*GetSourceMetadata().begin())->GetMetadata<VertexStream>();
	if (vertex)
	{
		vbr->SetMemoryUseage(m_eMemoryUseage);
		vbr->FlushVertexBuffer(*vertex, m_isKeepSource);
	}
	return true;
}

//
// VertexBufferEntity
//
IMPLEMENT_RTTI(VertexBufferEntity, IAsyncEntity);
BEGIN_ADD_PROPERTY(VertexBufferEntity, IAsyncEntity);
REGISTER_PROPERTY(m_spSource, m_spSource, Property::F_SAVE_LOAD_CLONE)
END_ADD_PROPERTY

ENTITY_FACTORY_FUNC(VertexBufferEntity)
IMPLEMENT_INITIAL_NO_FACTORY_BEGIN(VertexBufferEntity)
IMPLEMENT_INITIAL_END

VertexBufferEntity::VertexBufferEntity()
{

}

VertexBufferEntity::~VertexBufferEntity()
{

}

void VertexBufferEntity::_OnCreateResource()
{

}

void VertexBufferEntity::BeforeSave(Stream* pStream)
{

}

void VertexBufferEntity::PostSave(Stream* pStream)
{

}

void VertexBufferEntity::PostLoad(Stream* pStream)
{
	this->CreateResource();
}

NS_JYE_END
