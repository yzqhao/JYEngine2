
#pragma once

#include "private/Define.h"
#include "Core/Interface/ILoader.h"
#include "Engine/GraphicDefine.h"

NS_JYE_BEGIN

enum CHUNK_IDENTIFIER
{
	APOLLO_MESH = 100,
	CHUNK_END
};

struct Header
{
	char name[4];
	int  version;
	int  binaryDataOffset;
};

struct BindingBox
{
	float bindingbox[6];
};

struct VertexChunk
{
	int vertexLayout;
	unsigned int bufferOffset;
	unsigned int bufferLength;
	int skeletonIndex; //没用
};

struct VertexChunk2
{
	int vertexLayout;
	unsigned int bufferOffset;
	unsigned int bufferLength;
	int skeletonIndex; //没用
	unsigned int jointsIDOffset;
	unsigned int jointsIDLength;
};

struct VertexChunk3_vertex
{
	int vertexLayout;
	unsigned int bufferOffset;
	unsigned int bufferLength;
	int skeletonIndex; //没用
	int subMeshCnt;
};

struct VertexChunk4_vertex
{
	int vertexLayout;
	unsigned int bufferOffset;
	unsigned int bufferLength;
	int subMeshCnt;
};

struct VertexChunk3_indices
{
	int triangleCntOffset;
	int triangleCntLength;
	std::vector<int> jointsIDOffsetAndLength;
};


enum DRACO_GENERIC_UNIQUE_ID
{
	OLD_VERSION_COMPATIBLE = 0,
	BOND_INDEX = 1,
	BOND_WEIGHTS = 2,
	MORPH_TARGETS_VERTEX = 3,
	MORPH_TARGETS_NORMAL = 4,
};

class IFile;

class RESOURCE_SYSTEM_API MeshLoader : public ILoader
{
public:
	MeshLoader(const String& path) : ILoader(ResourceProperty::SLT_MESH, path), m_pMeshStreamData(nullptr){}
	~MeshLoader(void) {}

	virtual void Release(void* data) override;
	virtual ILoader* CreateCacheLoader() const override;

private:
	virtual void* _DoLoad(const String& paths);

	void _LoadMesh(const VertexChunk& chunkDesc, IFile* file, Header& header, std::vector<std::vector<int>>& jointsIndex, GraphicDefine::MeshStreamData* meshStreamData);
	void _LoadMesh2(const VertexChunk2& chunkDesc, IFile* file, Header& header, GraphicDefine::MeshStreamData* meshStreamData);
	void _LoadMesh3(VertexChunk3_indices& chunkDesc3_indices, const VertexChunk3_vertex& chunkDesc3_vertex, IFile* file, Header& header, GraphicDefine::MeshStreamData* meshStreamData);
	void _LoadMesh4(VertexChunk3_indices& chunkDesc3_indices, const VertexChunk4_vertex& chunkDesc4_vertex, IFile* file, Header& header, GraphicDefine::MeshStreamData* meshStreamData);


	GraphicDefine::MeshStreamData* m_pMeshStreamData;
};

NS_JYE_END
