#pragma once

#include "../Core.h"
#include "../Propery/ResourceProperty.h"

NS_JYE_BEGIN

class CORE_API ILoader
{
public:
	ILoader(ResourceProperty::SourceLoaderType slt, const String& path) : m_eSourceLoaderType(slt), m_Path(path) {};
	virtual ~ILoader(void) {};

	virtual void Release(void* data) = 0;
	virtual ILoader* CreateCacheLoader() const = 0;

	template< typename T >
	T* Load(bool needcache)
	{
		T* res = static_cast<T*>(_DoLoad(m_Path));
		if (needcache)
		{
			IResourceSystem::Instance()->TryCacheLoader(this);
		}
		return res;
	}

	inline const String& GetPath() const { return m_Path; }
	inline ResourceProperty::SourceLoaderType GetSourceLoaderType() const { return m_eSourceLoaderType; }

private:
	virtual void* _DoLoad(const String& path) = 0;

	ResourceProperty::SourceLoaderType m_eSourceLoaderType;
	String	m_Path;
};

NS_JYE_END