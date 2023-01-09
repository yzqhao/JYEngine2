#include "IndicesMetadata.h"
#include "Core/Interface/IFileSystem.h"
#include "System/Utility.hpp"
#include "Core/Interface/IResourceSystem.h"
#include "Core/Interface/ILoader.h"
#include "Engine/GraphicDefine.h"

#include <sstream>

NS_JYE_BEGIN

IMPLEMENT_RTTI_NoCreateFun(IndicesMetadata, IMetadata);
BEGIN_ADD_PROPERTY(IndicesMetadata, IMetadata);
END_ADD_PROPERTY

IndicesMetadata::IndicesMetadata()
{

}

IndicesMetadata::IndicesMetadata(RHIDefine::MemoryUseage mu)
	: m_eUseage(mu)
{

}

//
//ReferenceIndicesMetadata
//
IMPLEMENT_RTTI(ReferenceIndicesMetadata, IndicesMetadata);
BEGIN_ADD_PROPERTY(ReferenceIndicesMetadata, IndicesMetadata);
REGISTER_PROPERTY(m_rpStream, m_rpStream, Property::F_SAVE_LOAD_CLONE)
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(ReferenceIndicesMetadata)
IMPLEMENT_INITIAL_END

ReferenceIndicesMetadata::ReferenceIndicesMetadata()
	: m_rpStream(nullptr)
{

}

ReferenceIndicesMetadata::ReferenceIndicesMetadata(RHIDefine::MemoryUseage mu, IndicesStream* stream)
	: IndicesMetadata(mu)
	, m_rpStream(stream)
{
}

ReferenceIndicesMetadata::~ReferenceIndicesMetadata(void)
{
	// SAFE_DELETE(m_rpStream);	 // 这里不需要析构
}

void* ReferenceIndicesMetadata::_ReturnMetadata()
{
	return m_rpStream;
}

void ReferenceIndicesMetadata::ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash)
{
}

void ReferenceIndicesMetadata::ReleaseMetadate()
{

}

void ReferenceIndicesMetadata::GetIdentifier(std::string& hash) const
{
	hash.append(Utility::RandomString(64));
}

//
// FileIndicesMetadata
//

IMPLEMENT_RTTI(FileIndicesMetadata, IndicesMetadata);
BEGIN_ADD_PROPERTY(FileIndicesMetadata, IndicesMetadata);
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(FileIndicesMetadata)
IMPLEMENT_INITIAL_END

FileIndicesMetadata::FileIndicesMetadata()
	: m_pILoader(nullptr)
	, m_pSourceData(nullptr)
{

}

FileIndicesMetadata::FileIndicesMetadata(RHIDefine::MemoryUseage mu, const std::string& path)
	: IndicesMetadata(mu)
	, m_Path(path)
	, m_pILoader(NULL)
	, m_pSourceData(NULL)
{

}

FileIndicesMetadata::~FileIndicesMetadata(void)
{

}

void* FileIndicesMetadata::_ReturnMetadata()
{
	return m_pSourceData ? &(m_pSourceData->m_Indices) : NULL;
}

void FileIndicesMetadata::GetIdentifier(String& hash) const
{
	if (RHIDefine::MU_STATIC == GetMemoryUseage())
	{
		std::string realPath = IFileSystem::Instance()->PathAssembly(m_Path);
		hash.append(realPath);
	}
	else
	{
		hash.append(Utility::RandomString(64));
	}
}

void FileIndicesMetadata::ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash)
{
	std::string fileSuffix = Utility::GetFileSuffixString(m_Path);
	if (fileSuffix == GraphicDefine::NORMAL_MESH)
	{
		m_pILoader = IResourceSystem::Instance()->LoaderFactory(ResourceProperty::SLT_MESH, rt, source_hash, m_Path);
		m_pSourceData = m_pILoader->Load<GraphicDefine::MeshStreamData>(RHIDefine::MU_STATIC == GetMemoryUseage());
		if (!m_pSourceData)
		{
			JYERROR("fail to load vertex file %s", m_Path.c_str());
		}
	}
	else if (fileSuffix == GraphicDefine::SELF_DEFINED_MESH)
	{
		m_pSourceData = _NEW GraphicDefine::MeshStreamData();
		m_pSourceData->m_Indices.SetIndicesType(RHIDefine::IT_UINT16);
		m_pSourceData->m_Indices.ReserveBuffer(0);
	}
}

void FileIndicesMetadata::ReleaseMetadate()
{
	if (m_pILoader)
	{
		m_pILoader->Release(m_pSourceData);
		m_pSourceData = NULL;
		IResourceSystem::Instance()->RecycleBin(m_pILoader);
	}
}

NS_JYE_END
