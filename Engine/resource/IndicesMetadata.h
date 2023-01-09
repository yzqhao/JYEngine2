#pragma once

#include "GeneralMetadata.h"
#include "RHI/RHI.h"
#include "Engine/render/IndicesStream.h"
#include "Engine/GraphicDefine.h"

NS_JYE_BEGIN

class ENGINE_API IndicesMetadata : public IMetadata
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	IndicesMetadata();
	IndicesMetadata(RHIDefine::MemoryUseage mu);

	FORCEINLINE RHIDefine::MemoryUseage GetMemoryUseage() const { return m_eUseage; }
private:
	RHIDefine::MemoryUseage m_eUseage;
};
DECLARE_Ptr(IndicesMetadata);
TYPE_MARCO(IndicesMetadata);

class ENGINE_API ReferenceIndicesMetadata : public IndicesMetadata
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	ReferenceIndicesMetadata();
	ReferenceIndicesMetadata(RHIDefine::MemoryUseage mu, IndicesStream* stream);
	~ReferenceIndicesMetadata(void);
private:
	virtual void* _ReturnMetadata();
public:
	virtual void GetIdentifier(std::string& hash) const;
	virtual void ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash);
	virtual void ReleaseMetadate();
private:
	IndicesStream* m_rpStream;
};
DECLARE_Ptr(ReferenceIndicesMetadata);
TYPE_MARCO(ReferenceIndicesMetadata);

class ILoader;

class ENGINE_API FileIndicesMetadata : public IndicesMetadata
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	FileIndicesMetadata();
	FileIndicesMetadata(RHIDefine::MemoryUseage mu, const String& path);
	~FileIndicesMetadata(void);
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
DECLARE_Ptr(FileIndicesMetadata);
TYPE_MARCO(FileIndicesMetadata);

NS_JYE_END