#include "MaterialEntity.h"
#include "ShaderState.h"
#include "Engine/resource/MaterialMetadata.h"
#include "Engine/resource/MaterialStreamData.h"
#include "IMaterialSystem.h"

NS_JYE_BEGIN

//
// MaterialSource
//
IMPLEMENT_RTTI(MaterialSource, ISharedSource);
BEGIN_ADD_PROPERTY(MaterialSource, ISharedSource);
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(MaterialSource)
IMPLEMENT_INITIAL_END

MaterialSource::MaterialSource()
	: ISharedSource(ResourceProperty::SRT_MATERIALENTITY, true)
	, m_shader(NULL)
	, m_pSourceData(NULL)
{

}

MaterialSource::~MaterialSource()
{

}

IAsyncResource* MaterialSource::_DoCreateResource()
{
	return _NEW MaterialResource();
}

bool MaterialSource::_DoSetupResource(IAsyncResource* res)
{
	MaterialResource* mres = static_cast<MaterialResource*>(res);
	mres->SetStateEntity(m_shader);

	HashMap<String, MaterialParameter*> allparams;
	if (m_pSourceData)
	{
		auto slots = m_pSourceData->GetAllSlots();
		for (auto& slot : slots)
		{
			allparams.insert({ IMaterialSystem::Instance()->GetParameterInfomation(slot).Name , m_pSourceData->GetParameter(slot) });
		}
	}
	mres->SyncDefProperty(&allparams);
	m_pSourceData = nullptr;
	return true;
}

bool MaterialSource::_DoLoadResource()
{
	PathMetadata* data = static_cast<PathMetadata*>((*GetSourceMetadata().begin()));
	data->ProcessMetadata(GetResourceType(), GetHashCode());//处理加载原始数据
	String path;
	if (data->IsDerived(MaterialMetadata::RTTI()))
	{
		MaterialMetadata* matmetadata = static_cast<MaterialMetadata*>(data);
		m_pSourceData = matmetadata->GetMetadata<MaterialStreamData>();
		path = m_pSourceData->GetShaderPath();
	}
	else if (data->IsDerived(PathMetadata::RTTI()))
	{
		path = *(data->GetMetadata<String>());
	}
	else
	{
		JYERROR("MaterialSource load resource failed! Unknow meta type.");
	}
	m_shader = _NEW ShaderState();
	m_shader->CreateResource(path);
	return true;
}


//
// MaterialEntity
//
IMPLEMENT_RTTI(MaterialEntity, IAsyncEntity);
BEGIN_ADD_PROPERTY(MaterialEntity, IAsyncEntity);
REGISTER_PROPERTY(m_spSource, m_spSource, Property::F_SAVE_LOAD_CLONE)
END_ADD_PROPERTY

ENTITY_FACTORY_FUNC(MaterialEntity)
IMPLEMENT_INITIAL_NO_FACTORY_BEGIN(MaterialEntity)
IMPLEMENT_INITIAL_END

MaterialEntity::MaterialEntity()
{

}

MaterialEntity::~MaterialEntity()
{

}

void MaterialEntity::_OnCreateResource()
{

}

MaterialEntity* MaterialEntity::Clone()
{
	MaterialEntity* pEntity = _NEW MaterialEntity();
	pEntity->Copy(*this, true);
	return pEntity;
}

void MaterialEntity::BeforeSave(Stream* pStream)
{

}

void MaterialEntity::PostSave(Stream* pStream)
{

}

void MaterialEntity::PostLoad(Stream* pStream)
{
	this->CreateResource();
}

const String& MaterialEntity::GetMatPath() const
{
	IMetadata* metadata = *(GetSourcePtr()->GetSourceMetadata()).begin();
	if (metadata->IsSameType(MaterialMetadata::RTTI()))
	{
		MaterialMetadata* matmetadata = static_cast<MaterialMetadata*>(metadata);
		return matmetadata->GetPath();
	}
	static String failRes("");
	return failRes;
}

Map<String, MaterialParameter*> MaterialEntity::GetParameters()
{
	return std::move(_GetResourcePtr()->GetParameters());
}

NS_JYE_END