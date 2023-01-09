#pragma once

#include "GeneralMetadata.h"
#include "RHI/RHI.h"
#include "Math/AABB.h"
#include "Engine/render/VertexStream.h"
#include "Engine/GraphicDefine.h"

NS_JYE_BEGIN

namespace RenderObjectData
{
	struct MeshData 
	{
		RHIDefine::RenderMode	m_eRenderMode;
		bool m_isKeepSource;
		bool m_isCSBuffer;
		Pointer<IMetadata> m_spVertex;
		Pointer<IMetadata> m_spIndices;
		Vector<Vector<int>>* m_pAffectedJointIDs;
		Vector<std::pair<int, int>> m_indicesOffsetAndLength;
		Math::AABB m_BindingBox;
		VertexStream* m_pVertexStream;

		MeshData(void) : m_pAffectedJointIDs(NULL) , m_isCSBuffer(false)
		{
			m_BindingBox = Math::AABB(Math::Vec3(-1, -1, -1), Math::Vec3(1, 1, 1));
		};
		~MeshData(void) {};
	};
}

class ENGINE_API RenderObjectMeshMetadate : public IMetadata
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	RenderObjectMeshMetadate();
	RenderObjectMeshMetadate(RHIDefine::RenderMode rm, const IMetadata& vertex);
	RenderObjectMeshMetadate(RHIDefine::RenderMode rm, const IMetadata& vertex, const IMetadata& indices);
	~RenderObjectMeshMetadate(void);
	virtual void GetIdentifier(std::string& hash) const;//在str中推入表示符号
	virtual void ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash);//处理原始资源
	virtual void ReleaseMetadate();

	IMetadata* GetVectexMeta() { return m_Metadata.m_spVertex; }
	IMetadata* GetIndicesMeta() { return m_Metadata.m_spIndices; }
	void SetVectexMeta(IMetadata* meta) { m_Metadata.m_spVertex = meta; }
	void SetIndicesMeta(IMetadata* meta) { m_Metadata.m_spIndices = meta; }
private:
	virtual void* _ReturnMetadata();

	RenderObjectData::MeshData		m_Metadata;
};
DECLARE_Ptr(RenderObjectMeshMetadate);
TYPE_MARCO(RenderObjectMeshMetadate);

class ILoader;

class ENGINE_API RenderObjectMeshFileMetadate : public PathMetadata
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	RenderObjectMeshFileMetadate();
	RenderObjectMeshFileMetadate(RHIDefine::MemoryUseage mu, const String& path, const bool isCSBuffer = false);
	virtual ~RenderObjectMeshFileMetadate(void);

	virtual void GetIdentifier(std::string& hash) const override;//在str中推入表示符号
	virtual void ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash) override;//处理原始资源
	virtual void ReleaseMetadate() override;

	const RHIDefine::MemoryUseage& GetMemoryUseage();
	void SetMemoryUseage(const RHIDefine::MemoryUseage& m_eMemoryUseage);
private:
	virtual void* _ReturnMetadata();

	RHIDefine::MemoryUseage m_eMemoryUseage;
	RenderObjectData::MeshData m_Metadata;
	GraphicDefine::MeshStreamData* m_pSourceData;
	ILoader* m_pILoader;
};
DECLARE_Ptr(RenderObjectMeshFileMetadate);
TYPE_MARCO(RenderObjectMeshFileMetadate);

NS_JYE_END