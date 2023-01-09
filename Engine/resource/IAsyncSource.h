#pragma once

#include "../private/Define.h"
#include "Core/Object.h"
#include "System/Delegate.h"
#include "Core/Propery/ResourceProperty.h"
#include "IMetadata.h"

#include <atomic>

NS_JYE_BEGIN

class IAsyncResource;

class ENGINE_API IAsyncSource : public Object
{
	DECLARE_RTTI
	DECLARE_INITIAL
private:
	DECLARE_MULTICAST_DELEGATE(SourceLoadedCallBack, IAsyncSource*, IAsyncResource*);
public:
	IAsyncSource(ResourceProperty::ResourceType rt, ResourceProperty::SourceType cst);
	virtual ~IAsyncSource();

	IAsyncSource(const IAsyncSource& rhs);

	FORCEINLINE void PushSourceMetadata(const IMetadata& meta)
	{
		m_ISourceMetadataList.push_back(static_cast<IMetadata*>(meta.Duplicate()));
	}

	FORCEINLINE void ClearSourceMetadata()
	{
		for (IMetadata* meta : m_ISourceMetadataList)
		{
			SAFE_DELETE(meta);
		}
		m_ISourceMetadataList.clear();
	}

	FORCEINLINE const ISourceMetadataList& GetSourceMetadata() const { return m_ISourceMetadataList; }

	FORCEINLINE bool isSuccess() const
	{
		return m_isSuccess;
	}

	FORCEINLINE bool isLoaded() const
	{
		return m_isLoaded;
	}

	FORCEINLINE void SetCloned()
	{
		m_isCloned = true;
		m_nHashCode = 0;
		m_isPreLoaded = false;
	}

	virtual void DoJob(IAsyncSource* self)
	{
		bool res = _DoLoadResource();
		_DependenceReady(res);
	}

	FORCEINLINE IAsyncResource* PreCreateResource()
	{
		return _DoCreateResource();
	}

	FORCEINLINE void ResourceChange(IAsyncResource* res)
	{
		m_rpResourceptr = res;
	}

	FORCEINLINE void SetLoadedCallBacke(SourceLoadedCallBackDelegate& func)
	{
		if (!m_pSourceLoadedCallBack)
		{
			m_pSourceLoadedCallBack = _NEW SourceLoadedCallBack();
		}
		m_pSourceLoadedCallBack->Add(std::move(func));
	}

	FORCEINLINE size_t GetHashCode()
	{
		if (0 == m_nHashCode)
		{
			if (m_isCloned)
			{
				m_nHashCode = std::hash<std::string>{}(GetObjectIDString());
			}
			else
			{
				m_nHashCode = _GetResourceHashCode();
			}
		}
		return m_nHashCode;
	}
	
	FORCEINLINE ResourceProperty::SourceType GetSourceType() const { return m_eSourceType; }
	FORCEINLINE ResourceProperty::ResourceType GetResourceType() const { return m_eResourceType; }
	FORCEINLINE IAsyncResource* GetResource() { return m_rpResourceptr; }
	FORCEINLINE void SetPreLoaded(bool preLoad) { m_isPreLoaded = preLoad; }
	FORCEINLINE bool GetPreLoaded() { return m_isPreLoaded; }

	MulticastDelegate<IAsyncSource*>::DelegateT DependenceLoaded();//增加一个依赖资源，返回接受这个以来资源加载完毕的回掉
	void OnDependenceLoaded(IAsyncSource* dep);//当依赖资源加载完毕后

	IAsyncResource* CloneResource(IAsyncResource* rhs);

protected:
	FORCEINLINE void _SetSuccess(bool is) { m_isSuccess = is; }
	virtual size_t _GetResourceHashCode() const;//返回资源的哈希值
private:
	virtual IAsyncResource* _DoCreateResource() = 0;//将资源数据制作为实体资源
	virtual bool _DoSetupResource(IAsyncResource* res) = 0;//重新创建资源
	virtual bool _isLoadResource() = 0;//是否加载
	virtual bool _DoLoadResource() = 0;//加载资源
	virtual void _DoReleaseUnnecessaryDataAfterLoaded() {};
	virtual IAsyncResource* _OnClone(IAsyncResource* rhs) { return NULL; }

	void _DependenceReady(bool res);	//已经有一个依赖想准备完毕，尝试加入资源制作队列
	void _ReleaseUnnecessaryDataAfterLoaded();//资源被make后释放不需要的数据

	IAsyncResource* m_rpResourceptr;
	ISourceMetadataList m_ISourceMetadataList;
	SourceLoadedCallBack* m_pSourceLoadedCallBack;
	ResourceProperty::SourceType		m_eSourceType;
	ResourceProperty::ResourceType		m_eResourceType;
	size_t m_nHashCode;
	std::atomic_bool m_isSuccess;
	bool m_isLoaded;
	bool m_isPreLoaded;
	bool m_isCloned;
};
DECLARE_Ptr(IAsyncSource);
TYPE_MARCO(IAsyncSource);

//
// IMonopolySource
//
class IMonopolyResource;
class ENGINE_API IMonopolySource : public IAsyncSource 
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	IMonopolySource(ResourceProperty::ResourceType rt) : IAsyncSource(rt, ResourceProperty::CST_MONOPOLY) {}
	virtual ~IMonopolySource() {}
private:
	virtual bool _isLoadResource() { return true; }
};
DECLARE_Ptr(IMonopolySource);
TYPE_MARCO(IMonopolySource);

NS_JYE_END