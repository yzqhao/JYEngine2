#include "RenderObjectEntity.h"
#include "../resource/RenderObjectMetadata.h"

NS_JYE_BEGIN

IMPLEMENT_RTTI(RenderObjectSource, ISharedSource);
BEGIN_ADD_PROPERTY(RenderObjectSource, ISharedSource);
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(RenderObjectSource)
IMPLEMENT_INITIAL_END

RenderObjectSource::RenderObjectSource(void)
	: IMonopolySource(ResourceProperty::MRT_RENDEROBJECT)
	, m_pVertexBufferEntity(_NEW VertexBufferEntity())
	, m_pIndicesBufferEntity(NULL)
	, m_pInstanceBufferEntity(NULL)
	, m_eRenderMode(RHIDefine::RM_POINTS)
	, m_AffectedJointIDs()
	, m_isKeepSource(false)
	, m_isDynamicBatch(false)
	, m_BindingBox(Math::AABB(Math::Vec3(-1, -1, -1), Math::Vec3(1, 1, 1)))
{
}

RenderObjectSource::~RenderObjectSource(void)
{
	SAFE_DELETE(m_pVertexBufferEntity);
	SAFE_DELETE(m_pIndicesBufferEntity);
	SAFE_DELETE(m_pInstanceBufferEntity);
	for (int i = 0; i < m_pMaterialEntities.size(); i++)
	{
		SAFE_DELETE(m_pMaterialEntities[i]);
	}

}

bool RenderObjectSource::_DoLoadResource()
{
	const ISourceMetadataList& metas = GetSourceMetadata();
	for (auto& it : metas)
	{
		(it)->ProcessMetadata(GetResourceType(), GetHashCode());
		RenderObjectData::MeshData* data = (it)->GetMetadata<RenderObjectData::MeshData>();
		if ((it)->IsDerived(RenderObjectMeshFileMetadate::ms_Type))
		{
			RenderObjectMeshFileMetadate* meshdata = static_cast<RenderObjectMeshFileMetadate*>(it);
			m_MeshPath = meshdata->GetPath();
		}
		else
		{
			m_MeshPath = "buffer stream";
		}

		if (!_SetupMeshData(data))
		{
			return false;
		}
	}
	return true;
}

bool RenderObjectSource::_SetupMeshData(RenderObjectData::MeshData* metadata)
{
	if (nullptr == metadata)
	{
		JYERROR("RenderObjectSource::_SetupMeshData meshData is null ");
		return false;
	}
	if (metadata->m_spVertex == NULL)
	{
		JYERROR("metadata->m_spVertex == NULL ");
		return false;
	}
	m_eRenderMode = metadata->m_eRenderMode;
	m_BindingBox = metadata->m_BindingBox;

	if (!metadata->m_isCSBuffer)
	{
		m_pVertexBufferEntity->SetKeepSource(metadata->m_isKeepSource || m_isKeepSource);
		m_pVertexBufferEntity->PushMetadata(*metadata->m_spVertex);
		m_pVertexBufferEntity->DependenceSource(this);
	}

	if (metadata->m_pAffectedJointIDs != NULL)
	{
		m_AffectedJointIDs = *metadata->m_pAffectedJointIDs;
	}
	if (metadata->m_spIndices)
	{
		m_pIndicesBufferEntity = _NEW IndicesBufferEntity();
		m_pIndicesBufferEntity->SetKeepSource(metadata->m_isKeepSource || m_isKeepSource);
		m_pIndicesBufferEntity->PushMetadata(*metadata->m_spIndices);
		m_pIndicesBufferEntity->DependenceSource(this);
	}

	m_indicesOffsetAndLength = metadata->m_indicesOffsetAndLength;

	m_pVertexBufferEntity->CreateResource();
	if (NULL != m_pIndicesBufferEntity)
	{
		m_pIndicesBufferEntity->CreateResource();
	}
	return true;
}

IAsyncResource* RenderObjectSource::_DoCreateResource()
{
	return _NEW RenderObjectResource();
}

bool RenderObjectSource::_DoSetupResource(IAsyncResource* res)
{
	RenderObjectResource* ro = static_cast<RenderObjectResource*>(res);
	ro->SetupRenderObject(m_pVertexBufferEntity, m_pIndicesBufferEntity, m_indicesOffsetAndLength);
	ro->SetRenderMode(m_eRenderMode);
	return true;
}

int RenderObjectSource::GetMaterialCount()
{
	return m_pMaterialEntities.size();
}

MaterialEntity* RenderObjectSource::GetMaterialEntity(int index)
{
	if (index < 0 || index > m_pMaterialEntities.size())
		return NULL;
	else
		return m_pMaterialEntities[index];
}

const std::vector<MaterialEntity*>& RenderObjectSource::GetMaterialEntities()
{
	return m_pMaterialEntities;
}

void RenderObjectSource::SetMaterialEntities(const std::vector<MaterialEntity*>& entities)
{
	for (uint i = 0; i < m_pMaterialEntities.size(); ++i)
	{
		SAFE_DELETE(m_pMaterialEntities[i]);
	}
	m_pMaterialEntities.clear();
	m_pMaterialEntities.resize(entities.size());
	for (uint i = 0; i < entities.size(); ++i)
	{
		m_pMaterialEntities[i] = entities[i];
	}
}

MaterialEntity* RenderObjectSource::MakeMaterialAttachment()
{
	MaterialEntity* matEnt = _NEW MaterialEntity();
	m_pMaterialEntities.push_back(matEnt);
	return matEnt;
}

void RenderObjectSource::ChangeMaterialEntity(MaterialEntity* ma, int index)
{
	JY_ASSERT(index < m_pMaterialEntities.size());
	SAFE_DELETE(m_pMaterialEntities[index]);
	m_pMaterialEntities[index] = ma;
}

String& RenderObjectSource::GetMeshName()
{
	return m_MeshPath;
}

Math::AABB RenderObjectSource::GetBindBox()
{
	return m_BindingBox;
}

bool RenderObjectSource::hasParameter(RHIDefine::ParameterSlot att)
{
	for (int i = 0; i < m_pMaterialEntities.size(); i++)
	{
		if (m_pMaterialEntities[i]->hasParameter(att))
		{
			return true;
		}
	}
	return false;
}

void RenderObjectSource::SetKeepSource(bool isKeepSource)
{
	m_isKeepSource = isKeepSource;
	if (isLoaded())
	{
		m_pVertexBufferEntity->SetKeepSource(isKeepSource);
		if (m_pIndicesBufferEntity)
		{
			m_pIndicesBufferEntity->SetKeepSource(isKeepSource);
		}
	}
}

void RenderObjectSource::SetDynamicBatch(bool isDynamicBatch)
{
	if (isDynamicBatch && !m_isKeepSource && isLoaded())
	{
		JYERROR("Please enable batching before loading source");
	}
	m_isDynamicBatch = isDynamicBatch;
	m_isKeepSource = isDynamicBatch || m_isKeepSource;
}

bool RenderObjectSource::isKeepSource() const
{
	return m_isKeepSource;
}

const std::vector<std::vector<int>>& RenderObjectSource::GetAffectedJointIDs()
{
	return m_AffectedJointIDs;
}

void RenderObjectSource::SetParameter(RHIDefine::ParameterSlot att, MaterialParameter* obj)
{
	//»´≤ø…Ë÷√
	for (int i = 0; i < m_pMaterialEntities.size(); i++)
	{
		m_pMaterialEntities[i]->SetParameter(att, obj);
	}
}

Math::IntVec2 RenderObjectSource::GetDrawRange(int ind) const
{
	Math::IntVec2 range;

	if (ind < m_indicesOffsetAndLength.size())
	{
		range.x = m_indicesOffsetAndLength[ind].first;
		range.y = m_indicesOffsetAndLength[ind].second;
	}

	return range;
}

//
// RenderObjectEntity
//
IMPLEMENT_RTTI(RenderObjectEntity, IAsyncEntity);
BEGIN_ADD_PROPERTY(RenderObjectEntity, IAsyncEntity);
REGISTER_PROPERTY(m_spSource, m_spSource, Property::F_SAVE_LOAD_CLONE)
END_ADD_PROPERTY

ENTITY_FACTORY_FUNC(RenderObjectEntity)
IMPLEMENT_INITIAL_NO_FACTORY_BEGIN(RenderObjectEntity)
IMPLEMENT_INITIAL_END

RenderObjectEntity::RenderObjectEntity()
{

}

RenderObjectEntity::~RenderObjectEntity()
{

}

void RenderObjectEntity::_OnCreateResource()
{

}

VertexStream* RenderObjectEntity::GetVertexStream()
{
	if (_GetResourcePtr())
		return _GetResourcePtr()->GetVertexStream();
	return NULL;
}

IndicesStream* RenderObjectEntity::GetIndexStream()
{
	if (_GetResourcePtr())
		return _GetResourcePtr()->GetIndexStream();
	return NULL;
}

const std::vector<std::vector <int>>& RenderObjectEntity::GetAffectedIDs()
{
	if (_GetSourcePtr())
		return _GetSourcePtr()->GetAffectedJointIDs();
	return std::vector<std::vector<int>>();
}

void RenderObjectEntity::BeforeSave(Stream* pStream)
{

}

void RenderObjectEntity::PostSave(Stream* pStream)
{

}

void RenderObjectEntity::PostLoad(Stream* pStream)
{
	this->CreateResource();
}

NS_JYE_END
