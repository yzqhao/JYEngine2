#include "IndicesBufferEntity.h"
#include "Engine/resource/IndicesMetadata.h"

NS_JYE_BEGIN

IMPLEMENT_RTTI(IndicesBufferSource, ISharedSource);
BEGIN_ADD_PROPERTY(IndicesBufferSource, ISharedSource);
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(IndicesBufferSource)
IMPLEMENT_INITIAL_END
IndicesBufferSource::IndicesBufferSource(void)
	: ISharedSource(ResourceProperty::SRT_INDICESBUFFER, true)
	, m_eMemoryUseage(RHIDefine::MemoryUseage(-1))
{
}

IndicesBufferSource::~IndicesBufferSource(void)
{
}

bool IndicesBufferSource::_DoLoadResource()
{
	IndicesMetadata* data = static_cast<IndicesMetadata*>((*GetSourceMetadata().begin()));
	data->ProcessMetadata(GetResourceType(), GetHashCode());//处理加载原始数据
	m_eMemoryUseage = data->GetMemoryUseage();
	IndicesStream* indices = data->GetMetadata<IndicesStream>();
	return NULL != indices;
}

IAsyncResource* IndicesBufferSource::_DoCreateResource()
{
	return _NEW IndicesBufferResource();
}

bool IndicesBufferSource::_DoSetupResource(IAsyncResource* res)
{
	IndicesBufferResource* ivbr = static_cast<IndicesBufferResource*>(res);
	IndicesStream* indices = (*GetSourceMetadata().begin())->GetMetadata<IndicesStream>();
	if (indices)
	{
		ivbr->SetMemoryUseage(m_eMemoryUseage);
		ivbr->FlushIndicesBuffer(*indices, m_isKeepSource);
	}
	return true;
}

//
// IndicesBufferEntity
//
IMPLEMENT_RTTI(IndicesBufferEntity, IAsyncEntity);
BEGIN_ADD_PROPERTY(IndicesBufferEntity, IAsyncEntity);
REGISTER_PROPERTY(m_spSource, m_spSource, Property::F_SAVE_LOAD_CLONE)
END_ADD_PROPERTY

ENTITY_FACTORY_FUNC(IndicesBufferEntity)
IMPLEMENT_INITIAL_NO_FACTORY_BEGIN(IndicesBufferEntity)
IMPLEMENT_INITIAL_END

IndicesBufferEntity::IndicesBufferEntity()
{

}

IndicesBufferEntity::~IndicesBufferEntity()
{

}

void IndicesBufferEntity::_OnCreateResource()
{

}

void IndicesBufferEntity::BeforeSave(Stream* pStream)
{

}

void IndicesBufferEntity::PostSave(Stream* pStream)
{

}

void IndicesBufferEntity::PostLoad(Stream* pStream)
{
	this->CreateResource();
}

NS_JYE_END
