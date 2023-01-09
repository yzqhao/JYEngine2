#pragma once

#include "GeneralMetadata.h"
#include "RHI/RHI.h"
#include "Engine/render/VertexStream.h"
#include "Engine/GraphicDefine.h"

NS_JYE_BEGIN

class VertexStream;

class ENGINE_API VertexMetadata : public IMetadata
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	VertexMetadata();
	VertexMetadata(RHIDefine::MemoryUseage mu);

	FORCEINLINE RHIDefine::MemoryUseage GetMemoryUseage() const { return m_eUseage; }
private:
	RHIDefine::MemoryUseage m_eUseage;
};

class ENGINE_API ReferenceVertexMetadata : public VertexMetadata
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	ReferenceVertexMetadata();
	ReferenceVertexMetadata(RHIDefine::MemoryUseage mu, VertexStream* stream);
	~ReferenceVertexMetadata(void);
private:
	virtual void* _ReturnMetadata();
public:
	virtual void GetIdentifier(std::string& hash) const;
	virtual void ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash);
	virtual void ReleaseMetadate();
private:
	VertexStream* m_rpStream;
};

class ILoader;

class ENGINE_API FileVertexMetadata : public VertexMetadata
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	FileVertexMetadata();
	FileVertexMetadata(RHIDefine::MemoryUseage mu, const String& path);
	~FileVertexMetadata(void);
private:
	virtual void* _ReturnMetadata();
public:
	virtual void GetIdentifier(String& hash) const;
	virtual void ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash);//处理原始资源
	virtual void ReleaseMetadate();
private:
	String m_Path;
	ILoader* m_pILoader;
	GraphicDefine::MeshStreamData* m_pSourceData;
};

NS_JYE_END