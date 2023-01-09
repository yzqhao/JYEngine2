#include "MeshCache.h"

NS_JYE_BEGIN

MeshCache::MeshCache(GraphicDefine::MeshStreamData* input, const std::string& path)
	: ILoader(ResourceProperty::SLT_MESH, path)
	, m_pMeshStreamData(_NEW GraphicDefine::MeshStreamData(*input))
	, m_isReference(false)
{

}

MeshCache::MeshCache(const std::string& path)
	: ILoader(ResourceProperty::SLT_MESH, path)
	, m_pMeshStreamData(NULL)
	, m_isReference(false)
{
}

MeshCache::~MeshCache()
{
	Release(NULL);
}

void MeshCache::Release(void* data)
{
	if (!m_isReference)
	{
		SAFE_DELETE(m_pMeshStreamData);
	}
}

ILoader* MeshCache::CreateCacheLoader() const
{
	MeshCache* mc = _NEW MeshCache(GetPath());
	mc->_Reference(m_pMeshStreamData);
	return mc;
}

void* MeshCache::_DoLoad(const String& path)
{
	return m_pMeshStreamData;
}

NS_JYE_END