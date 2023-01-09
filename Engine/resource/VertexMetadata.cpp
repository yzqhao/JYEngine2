#include "VertexMetadata.h"
#include "Core/Interface/IFileSystem.h"
#include "Core/Interface/IFile.h"
#include "System/Utility.hpp"
#include "Core/Interface/IResourceSystem.h"
#include "Core/Interface/ILoader.h"
#include "Core/Interface/Serialize/IDecoder.h"
#include "Core/Interface/Serialize/IEncoder.h"
#include "Core/Interface/Serialize/ISerializer.h"
#include "Core/Interface/Serialize/ISerializeSystem.h"

#include <sstream>

NS_JYE_BEGIN

IMPLEMENT_RTTI_NoCreateFun(VertexMetadata, IMetadata);
BEGIN_ADD_PROPERTY(VertexMetadata, IMetadata);
END_ADD_PROPERTY

VertexMetadata::VertexMetadata()
{

}

VertexMetadata::VertexMetadata(RHIDefine::MemoryUseage mu)
	: m_eUseage(mu)
{

}

//
//ReferenceVertexMetadata
//
IMPLEMENT_RTTI(ReferenceVertexMetadata, VertexMetadata);
BEGIN_ADD_PROPERTY(ReferenceVertexMetadata, VertexMetadata);
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(ReferenceVertexMetadata)
IMPLEMENT_INITIAL_END

ReferenceVertexMetadata::ReferenceVertexMetadata()
	: m_rpStream(nullptr)
{

}

ReferenceVertexMetadata::ReferenceVertexMetadata(RHIDefine::MemoryUseage mu, VertexStream* stream)
	: VertexMetadata(mu)
	, m_rpStream(stream)
{
}

ReferenceVertexMetadata::~ReferenceVertexMetadata(void)
{
	//SAFE_DELETE(m_rpStream);  // 这里不需要析构
}

void* ReferenceVertexMetadata::_ReturnMetadata()
{
	return m_rpStream;
}

void ReferenceVertexMetadata::ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash)
{
}

void ReferenceVertexMetadata::ReleaseMetadate()
{

}

void ReferenceVertexMetadata::GetIdentifier(std::string& hash) const
{
	hash.append(Utility::RandomString(64));
}

//
// FileVertexMetadata
//

IMPLEMENT_RTTI(FileVertexMetadata, VertexMetadata);
BEGIN_ADD_PROPERTY(FileVertexMetadata, VertexMetadata);
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(FileVertexMetadata)
IMPLEMENT_INITIAL_END

FileVertexMetadata::FileVertexMetadata()
	: m_pILoader(nullptr)
	, m_pSourceData(nullptr)
{

}

FileVertexMetadata::FileVertexMetadata(RHIDefine::MemoryUseage mu, const std::string& path)
	: VertexMetadata(mu)
	, m_Path(path)
	, m_pILoader(NULL)
	, m_pSourceData(NULL)
{

}

FileVertexMetadata::~FileVertexMetadata(void)
{

}

void* FileVertexMetadata::_ReturnMetadata()
{
	return m_pSourceData ? &(m_pSourceData->m_Vertex) : NULL;
}

void FileVertexMetadata::GetIdentifier(String& hash) const
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

void FileVertexMetadata::ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash)
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
		std::vector<GraphicDefine::VertexAttribute> attributes;
		IDecoder* childDecoder = decoder->GetClassFieldByName("VertexAttribute");
		if (childDecoder != NULL)
		{
			int length = childDecoder->GetSerializeLength();
			for (int i = 1; i < length; i++)
			{
				const IDecoder* attribute = childDecoder->AsClass(i);
				attributes.push_back({ RHIDefine::ShaderAttribute(attribute->AsInt(1)), attribute->AsInt(2) });
			}
		}
		m_pSourceData = _NEW GraphicDefine::MeshStreamData();
		m_pSourceData->m_eMode = (RHIDefine::RenderMode)decoder->GetIntFieldByName("RenderMode");
		for (int i = 0; i < attributes.size(); i++)
		{
			m_pSourceData->m_Vertex.SetVertexType(attributes[i].slot, RHIDefine::DT_FLOAT,
				RHIDefine::MU_STATIC == GetMemoryUseage()
				? RHIDefine::DT_HALF_FLOAT
				: RHIDefine::DT_FLOAT,
				attributes[i].size);
		}
		m_pSourceData->m_Vertex.ReserveBuffer(0);
		SAFE_DELETE_ARRAY(buffer);
		ISerializeSystem::Instance()->RecycleBin(ser);
	}
}

void FileVertexMetadata::ReleaseMetadate()
{
	if (m_pILoader)
	{
		m_pILoader->Release(m_pSourceData);
		m_pSourceData = NULL;
		IResourceSystem::Instance()->RecycleBin(m_pILoader);
	}
}

NS_JYE_END
