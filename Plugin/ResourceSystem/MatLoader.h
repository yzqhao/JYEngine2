
#pragma once

#include "private/Define.h"
#include "Core/Interface/ILoader.h"
#include "Engine/GraphicDefine.h"
#include "Engine/resource/MaterialStreamData.h"

NS_JYE_BEGIN

class RESOURCE_SYSTEM_API MatLoader : public ILoader
{
public:
	MatLoader(const String& path) : ILoader(ResourceProperty::SLT_MAT, path) {}
	~MatLoader(void) {}

	virtual void Release(void* data) override;
	virtual ILoader* CreateCacheLoader() const override;

private:
	virtual void* _DoLoad(const String& paths);
private:
	MaterialStreamData* m_pSourceData {};
};

NS_JYE_END
