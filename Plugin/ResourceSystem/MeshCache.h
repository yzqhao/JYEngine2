
#pragma once

#include "private/Define.h"
#include "Core/Interface/ILoader.h"
#include "Engine/GraphicDefine.h"

NS_JYE_BEGIN

class RESOURCE_SYSTEM_API MeshCache : public ILoader
{
public:
	MeshCache(GraphicDefine::MeshStreamData* input, const String& path);
	MeshCache(const String& path);
	~MeshCache();

	virtual void Release(void* data) override;
	virtual ILoader* CreateCacheLoader() const override;

private:
	virtual void* _DoLoad(const String& paths);

	FORCEINLINE void _Reference(GraphicDefine::MeshStreamData* input);

	GraphicDefine::MeshStreamData* m_pMeshStreamData;
	bool m_isReference;
};

FORCEINLINE void MeshCache::_Reference(GraphicDefine::MeshStreamData* input)
{
	m_pMeshStreamData = input;
	m_isReference = true;
}

NS_JYE_END
