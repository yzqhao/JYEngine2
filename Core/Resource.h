
#pragma once

#include "Object.h"

NS_JYE_BEGIN

class CORE_API Resource
{
public:
	enum	//Resource Type
	{
		RT_TEXTURE,
		RT_SKELECTON_MODEL,
		RT_STATIC_MODEL,
		RT_ACTION,
		RT_MATERIAL,
		RT_POSTEFFECT,
		RT_SHADER,
		RT_ANIMTREE,
		RT_MORPHTREE,
		RT_TERRAIN,
		RT_FSM,
		RT_ACTOR,
		RT_MAP,
		RT_FONTTYPE,
		RT_MAX
	};
	Resource() {}
	virtual ~Resource() {}
	virtual const String& GetResourceName() const { return m_ResourceName; }
	virtual void SetResourceName(const String& ResourceName) { m_ResourceName = ResourceName; }
	virtual unsigned int GetResourceType() const = 0;

	static const String& GetFileSuffix(unsigned int uiFileSuffix);
protected:
	String	m_ResourceName;

	static String ms_FileSuffix[];
};

class CORE_API ResourceProxyBase : public Reference
{
public:
	ResourceProxyBase() {}
	virtual ~ResourceProxyBase() {}
	virtual bool IsEndableASYNLoad() = 0;
	virtual bool IsEndableGC() = 0;
	virtual bool IsLoaded() = 0;
	virtual const String& GetResourceName() = 0;
	virtual unsigned int GetResourceType() = 0;
	void LoadEvent()
	{
		for (auto& it : m_LoadedEventObject)
		{
			it.first->LoadedEvent(this, it.second);
		}
		m_LoadedEventObject.clear();
	}
	void AddLoadEventObject(Object* pObject, void* Data = 0)
	{
		if (!pObject) return;
		
		if (m_bIsLoaded == false)
		{
			m_LoadedEventObject.insert({ pObject, Data });
		}
		else
		{
			pObject->LoadedEvent(this);
		}
	}
protected:
	Map<ObjectPtr, void*> m_LoadedEventObject;
	bool	m_bIsLoaded{false};
	String	m_ResourceName;
	void Loaded()
	{
		m_bIsLoaded = true;
		LoadEvent();
	}
	void SetResourceName(const String& ResourceName)
	{
		m_ResourceName = ResourceName;
	}
};
DECLARE_Ptr(ResourceProxyBase);

template<class T>
class ResourceProxy : public ResourceProxyBase
{
public:
	ResourceProxy();
	virtual ~ResourceProxy();
	T* GetResource();
	virtual const String& GetResourceName();
	virtual bool IsLoaded();
	virtual bool IsEndableASYNLoad();
	virtual bool IsEndableGC();
	virtual unsigned int GetResourceType();
	void SetNewResource(Pointer<T> pResource);
	static ResourceProxy<T>* Create(T* pResource);
protected:
	Pointer<T> m_pResource;
	Pointer<T> m_pPreResource;
};

template<class T>
ResourceProxy<T>* ResourceProxy<T>::Create(T* pResource)
{
	ResourceProxy<T>* pProxy = _NEW ResourceProxy<T>();
	pProxy->SetResourceName(pResource->GetResourceName());
	pProxy->SetNewResource(pResource);
	pProxy->Loaded();
	return pProxy;
}

template<class T>
ResourceProxy<T>::ResourceProxy()
{
	m_pPreResource = (T*)T::GetDefalut();
	m_pResource = NULL;
}

template<class T>
ResourceProxy<T>::~ResourceProxy()
{
	m_pPreResource = NULL;
	m_pResource = NULL;
}
template<class T>
void ResourceProxy<T>::SetNewResource(Pointer<T> pResource)
{
	m_pResource = pResource;
	m_pResource->SetResourceName(m_ResourceName);
}

template<class T>
bool ResourceProxy<T>::IsEndableASYNLoad()
{
	return T::ms_bIsEnableASYNLoader;
}

template<class T>
bool ResourceProxy<T>::IsEndableGC()
{
	return T::ms_bIsEnableGC;
}

template<class T>
bool ResourceProxy<T>::IsLoaded()
{
	return m_bIsLoaded;
}

template<class T>
T* ResourceProxy<T>::GetResource()
{
	if (m_bIsLoaded)
	{
		return m_pResource;
	}
	else
	{
		return m_pPreResource;
	}
}

template<class T>
unsigned int ResourceProxy<T>::GetResourceType()
{
	return GetResource()->GetResourceType();
}

template<class T>
const String& ResourceProxy<T>::GetResourceName()
{
	return m_ResourceName;
}

#define DECLARE_Proxy(ClassName)	\
	typedef ResourceProxy<ClassName> ##ClassName##R;\
	typedef Pointer<ResourceProxy<ClassName>> ##ClassName##RPtr;

NS_JYE_END
