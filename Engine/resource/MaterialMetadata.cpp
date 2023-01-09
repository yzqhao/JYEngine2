#include "MaterialMetadata.h"
#include "MaterialStreamData.h"
#include "Core/Interface/IFileSystem.h"
#include "Core/Interface/IResourceSystem.h"
#include "Core/Interface/ILoader.h"
#include "Core/Interface/ILogSystem.h"

NS_JYE_BEGIN

IMPLEMENT_RTTI(MaterialMetadata, PathMetadata);
BEGIN_ADD_PROPERTY(MaterialMetadata, PathMetadata);
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(MaterialMetadata)
IMPLEMENT_INITIAL_END

MaterialMetadata::MaterialMetadata()
	: PathMetadata()
	, m_pSourceData(nullptr)
{

}

MaterialMetadata::MaterialMetadata(const String& path)
	: PathMetadata(path)
	, m_pSourceData(nullptr)
{

}

MaterialMetadata::~MaterialMetadata()
{

}

void MaterialMetadata::GetIdentifier(std::string& hash) const
{
	std::string realPath = IFileSystem::Instance()->PathAssembly(m_Path);
	hash.append(realPath);
	hash.append(m_SceneIDString);
}

void MaterialMetadata::ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash)
{
	m_pILoader = IResourceSystem::Instance()->LoaderFactory(ResourceProperty::SLT_MAT, rt, source_hash, m_Path);
	m_pSourceData = m_pILoader->Load<MaterialStreamData>(true);
	if (m_pSourceData==NULL)
	{
		JYERROR("load a empty mat, may be deleted %s " , m_Path.c_str() );
		return;
	}
}

void* MaterialMetadata::_ReturnMetadata()
{
	return m_pSourceData ? m_pSourceData : NULL;
}

void MaterialMetadata::ReleaseMetadate()
{
	if (m_pILoader)
	{
		m_pILoader->Release(m_pSourceData);
		m_pSourceData = NULL;
		IResourceSystem::Instance()->RecycleBin(m_pILoader);
	}
}

NS_JYE_END