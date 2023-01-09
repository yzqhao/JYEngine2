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

	MulticastDelegate<IAsyncSource*>::DelegateT DependenceLoaded();//����һ��������Դ�����ؽ������������Դ������ϵĻص�
	void OnDependenceLoaded(IAsyncSource* dep);//��������Դ������Ϻ�

	IAsyncResource* CloneResource(IAsyncResource* rhs);

protected:
	FORCEINLINE void _SetSuccess(bool is) { m_isSuccess = is; }
	virtual size_t _GetResourceHashCode() const;//������Դ�Ĺ�ϣֵ
private:
	virtual IAsyncResource* _DoCreateResource() = 0;//����Դ��������Ϊʵ����Դ
	virtual bool _DoSetupResource(IAsyncResource* res) = 0;//���´�����Դ
	virtual bool _isLoadResource() = 0;//�Ƿ����
	virtual bool _DoLoadResource() = 0;//������Դ
	virtual void _DoReleaseUnnecessaryDataAfterLoaded() {};
	virtual IAsyncResource* _OnClone(IAsyncResource* rhs) { return NULL; }

	void _DependenceReady(bool res);	//�Ѿ���һ��������׼����ϣ����Լ�����Դ��������
	void _ReleaseUnnecessaryDataAfterLoaded();//��Դ��make���ͷŲ���Ҫ������

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