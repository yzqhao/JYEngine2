#pragma once

#include "GeneralMetadata.h"

NS_JYE_BEGIN

class MaterialStreamData;
class ILoader;

class ENGINE_API MaterialMetadata : public PathMetadata
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	MaterialMetadata();
	MaterialMetadata(const String& path);
	virtual ~MaterialMetadata(void);

	virtual void GetIdentifier(std::string& hash) const;
	virtual void ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash);
	virtual void ReleaseMetadate();

private:
	virtual void* _ReturnMetadata();
private:
	MaterialStreamData*	m_pSourceData;
	ILoader* m_pILoader;
};
DECLARE_Ptr(MaterialMetadata);
TYPE_MARCO(MaterialMetadata);

NS_JYE_END