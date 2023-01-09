#include "RenderObjectMetadata.h"
#include "Core/Interface/IFileSystem.h"
#include "Core/Interface/IFile.h"
#include "System/Utility.hpp"
#include "Core/Interface/IResourceSystem.h"
#include "Core/Interface/ILoader.h"
#include "Core/Interface/Serialize/IDecoder.h"
#include "Core/Interface/Serialize/IEncoder.h"
#include "Core/Interface/Serialize/ISerializer.h"
#include "Core/Interface/Serialize/ISerializeSystem.h"
#include "VertexMetadata.h"
#include "IndicesMetadata.h"

#include <sstream>

NS_JYE_BEGIN

//
// RenderObjectMeshMetadate
//

IMPLEMENT_RTTI(RenderObjectMeshMetadate, IMetadata);
BEGIN_ADD_PROPERTY(RenderObjectMeshMetadate, IMetadata);
REGISTER_PROPERTY(m_Metadata.m_eRenderMode, m_eRenderMode, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_Metadata.m_spVertex, m_spVertex, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_Metadata.m_spIndices, m_spIndices, Property::F_SAVE_LOAD_CLONE)
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(RenderObjectMeshMetadate)
IMPLEMENT_INITIAL_END

RenderObjectMeshMetadate::RenderObjectMeshMetadate()
{
	m_Metadata.m_isCSBuffer = false;
}

RenderObjectMeshMetadate::RenderObjectMeshMetadate(RHIDefine::RenderMode rm, const IMetadata& vertex)
{
	m_Metadata.m_eRenderMode = rm;
	m_Metadata.m_spVertex = Pointer<IMetadata>(static_cast<IMetadata*>(vertex.Duplicate()));
}

RenderObjectMeshMetadate::RenderObjectMeshMetadate(RHIDefine::RenderMode rm, const IMetadata& vertex, const IMetadata& indices)
{
	m_Metadata.m_eRenderMode = rm;
	m_Metadata.m_spVertex = Pointer<IMetadata>(static_cast<IMetadata*>(vertex.Duplicate()));
	m_Metadata.m_spIndices = Pointer<IMetadata>(static_cast<IMetadata*>(indices.Duplicate()));
}

RenderObjectMeshMetadate::~RenderObjectMeshMetadate(void)
{

}

void* RenderObjectMeshMetadate::_ReturnMetadata()
{
	return &m_Metadata;
}

void RenderObjectMeshMetadate::GetIdentifier(std::string& hash) const
{
	m_Metadata.m_spVertex->GetIdentifier(hash);
	if (NULL != m_Metadata.m_spIndices)
	{
		m_Metadata.m_spIndices->GetIdentifier(hash);
	}
	hash.push_back(m_Metadata.m_eRenderMode);
}

void RenderObjectMeshMetadate::ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash)
{
}

void RenderObjectMeshMetadate::ReleaseMetadate()
{
}

//
// RenderObjectMeshFileMetadate
//

IMPLEMENT_RTTI(RenderObjectMeshFileMetadate, PathMetadata);
BEGIN_ADD_PROPERTY(RenderObjectMeshFileMetadate, PathMetadata);
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(RenderObjectMeshFileMetadate)
IMPLEMENT_INITIAL_END

RenderObjectMeshFileMetadate::RenderObjectMeshFileMetadate()
	:m_pILoader(NULL)
	, m_eMemoryUseage(RHIDefine::MemoryUseage::MU_STATIC)
{
	m_Metadata.m_isCSBuffer = false;
}

RenderObjectMeshFileMetadate::RenderObjectMeshFileMetadate(RHIDefine::MemoryUseage mu, const std::string& path, const bool isCSBuffer)
	:PathMetadata(path)
	, m_pILoader(NULL)
	, m_eMemoryUseage(mu)
{
	m_Metadata.m_isCSBuffer = isCSBuffer;
}

RenderObjectMeshFileMetadate::~RenderObjectMeshFileMetadate(void)
{

}

void* RenderObjectMeshFileMetadate::_ReturnMetadata()
{
	return &m_Metadata;
}

void RenderObjectMeshFileMetadate::GetIdentifier(std::string& hash) const
{
	if (RHIDefine::MU_STATIC == m_eMemoryUseage)
	{
		std::string realPath = IFileSystem::Instance()->PathAssembly(m_Path);
		hash.append(realPath);
	}
	else
	{
		hash.append(Utility::RandomString(64));
	}
}

void RenderObjectMeshFileMetadate::ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash)
{
	std::string fileSuffix = Utility::GetFileSuffixString(m_Path);
	if (fileSuffix == GraphicDefine::NORMAL_MESH)
	{
		m_pILoader = IResourceSystem::Instance()->LoaderFactory(ResourceProperty::SLT_MESH, rt, source_hash, m_Path);
		m_pSourceData = m_pILoader->Load<GraphicDefine::MeshStreamData>(RHIDefine::MU_STATIC == m_eMemoryUseage);
		if (m_pSourceData == NULL)
		{
			JYERROR("load a empty mesh,may be deleted %s ", m_Path.c_str());
			return;
		}
		m_Metadata.m_eRenderMode = m_pSourceData->m_eMode;
		m_Metadata.m_isKeepSource = m_eMemoryUseage == RHIDefine::MU_DYNAMIC;
		m_Metadata.m_spIndices = Pointer<FileIndicesMetadata>(_NEW FileIndicesMetadata(m_eMemoryUseage, m_Path));
		m_Metadata.m_spVertex = Pointer<FileVertexMetadata>(_NEW FileVertexMetadata(m_eMemoryUseage, m_Path));
		m_Metadata.m_pAffectedJointIDs = &m_pSourceData->m_SplitedJointsIDs;
		m_Metadata.m_pVertexStream = &m_pSourceData->m_Vertex;
		m_Metadata.m_BindingBox = m_pSourceData->m_BindingBox;
		int offset = 0;
		if (m_pSourceData->m_TrianglesCnt.size() == 0)
		{
			m_Metadata.m_indicesOffsetAndLength.resize(1);
			m_Metadata.m_indicesOffsetAndLength[0].first = offset;
			m_Metadata.m_indicesOffsetAndLength[0].second = m_pSourceData->m_Indices.GetIndicesCount();
		}
		else
		{
			RHIDefine::IndicesType tp = m_pSourceData->m_Indices.GetIndicesType();
			m_Metadata.m_indicesOffsetAndLength.resize(m_pSourceData->m_TrianglesCnt.size());
			for (size_t i = 0; i < m_pSourceData->m_TrianglesCnt.size(); ++i)
			{
				m_Metadata.m_indicesOffsetAndLength[i].first = offset;
				m_Metadata.m_indicesOffsetAndLength[i].second = m_pSourceData->m_TrianglesCnt[i] * 3;
				offset += m_pSourceData->m_TrianglesCnt[i] * 3;
			}
		}
	}
	else if (fileSuffix == GraphicDefine::SELF_DEFINED_MESH)
	{
		ISerializer* ser = ISerializeSystem::Instance()->SerializerFactory();
		std::string path = m_Path;
		byte* buffer;
		if (!IFileSystem::Instance()->isFileExist(path))
		{
			JYERROR("mesh file %s is not exist", path.c_str());
			return;
		}
		IFile* readfile = IFileSystem::Instance()->FileFactory(path);
		readfile->OpenFile(IFile::AT_READ);
		buffer = _NEW byte[readfile->GetSize()];
		readfile->ReadFile(buffer, readfile->GetSize());
		readfile->CloseFile();
		IFileSystem::Instance()->RecycleBin(readfile);
		IDecoder* originDecoder = ser->GetDecoderFromBuffer(buffer);
		const IDecoder* decoder = originDecoder->AsClass(1);
		m_Metadata.m_eRenderMode = (RHIDefine::RenderMode)decoder->GetIntFieldByName("RenderMode");
		m_Metadata.m_isKeepSource = m_eMemoryUseage == RHIDefine::MU_DYNAMIC;
		m_Metadata.m_spVertex = (_NEW FileVertexMetadata(m_eMemoryUseage, m_Path));
		if (decoder->HasField("UseIndex"))
		{
			if (decoder->GetBoolFieldByName("UseIndex"))
			{
				m_Metadata.m_spIndices = (
					_NEW FileIndicesMetadata(m_eMemoryUseage, m_Path)
					);
			}
		}
		else
		{
			m_Metadata.m_spIndices = (
				_NEW FileIndicesMetadata(m_eMemoryUseage, m_Path)
				);
		}

		SAFE_DELETE_ARRAY(buffer);
		ISerializeSystem::Instance()->RecycleBin(ser);
	}
}

void RenderObjectMeshFileMetadate::ReleaseMetadate()
{
	if (m_pILoader)
	{
		m_pILoader->Release(m_pSourceData);
		m_pSourceData = NULL;
		IResourceSystem::Instance()->RecycleBin(m_pILoader);
	}
}

const RHIDefine::MemoryUseage& RenderObjectMeshFileMetadate::GetMemoryUseage()
{
	return m_eMemoryUseage;
}

void RenderObjectMeshFileMetadate::SetMemoryUseage(const RHIDefine::MemoryUseage& memoryUseage)
{
	m_eMemoryUseage = memoryUseage;
}

NS_JYE_END
