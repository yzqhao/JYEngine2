
#pragma once

#include "private/Define.h"
#include "Core/Interface/ILoader.h"
#include "Engine/GraphicDefine.h"

NS_JYE_BEGIN

class TextureDescribeData;

class RESOURCE_SYSTEM_API TexMetaLoader : public ILoader
{
public:
	TexMetaLoader(const String& path) : ILoader(ResourceProperty::SLT_METADATA, path) {}
	~TexMetaLoader(void) {}

	virtual void Release(void* data) override;
	virtual ILoader* CreateCacheLoader() const override;

private:
	virtual void* _DoLoad(const String& paths);
private:
	TextureDescribeData* m_pSourceData {};
};

NS_JYE_END
