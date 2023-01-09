#pragma once

#include "../private/Define.h"
#include "Core/Propery/ResourceProperty.h"
#include "Core/Interface/ILogSystem.h"
#include "IMetadata.h"
#include "IResourceEntity.h"
#include "Core/Interface/IResourceSystem.h"
#include "System/Delegate.h"

NS_JYE_BEGIN

namespace EntityProperty
{
	enum EntityStatus
	{
		ES_NEWBORN = 0,
		ES_LOADING,
		ES_READY,
		ES_DISCARD,
	};
}

#define DECLARE_OBJECT_CTOR_FUNC(type)	\
	type(Object* obj) : IAsyncEntity(obj) \
	{\
	}

#define ENTITY_FACTORY_FUNC(type)	\
	Object* type::FactoryFunc(Object* host)\
	{\
		return _NEW type(host);\
	}

template<typename RES, typename SOU>
class IAsyncEntity : public IResourceEntity
{
	DECLARE_RTTI
	DECLARE_INITIAL
private:
	DECLARE_MULTICAST_DELEGATE(LoadSourceEvent, IAsyncSource*);
public:
	IAsyncEntity()
		: m_eEntityStatus(EntityProperty::ES_NEWBORN)
		, m_pResource(NULL)
		, m_spSource(_NEW SOU())
		, m_pOnLoadedEvent(NULL)
	{
	}

	// ���л��Ż���ã����������ֹ����
	IAsyncEntity(Object* obj)
		: m_eEntityStatus(EntityProperty::ES_NEWBORN)
		, m_pResource(NULL)
		, m_spSource(NULL)	// �����ʹ�����л���ֵ
		, m_pOnLoadedEvent(NULL)
	{
	}

	IAsyncEntity(const IAsyncEntity& rhs)
		:IResourceEntity(rhs)
		, m_pResource(NULL)
		, m_pOnLoadedEvent(NULL)
		, m_spSource(NULL)
		, m_eEntityStatus(EntityProperty::ES_NEWBORN)
	{
		_DoCopyEntity(rhs);
	}

	virtual ~IAsyncEntity() 
	{
		IResourceSystem::Instance()->UnloadSource(m_spSource);
		SAFE_DELETE(m_pOnLoadedEvent); 
	}

	virtual void ResourceChange(IAsyncResource* res) override {}

	virtual void ResourceCallBack(IAsyncResource* res, size_t hash) override
	{
		m_pResource = static_cast<RES*>(res);
		m_pResource->SetDeprecated(false);
		_Ready();//��Դ׼������
		m_eEntityStatus = EntityProperty::ES_READY;
		_OnResourceCreated();
	}

	void CreateResource()//��ʼ������Դ
	{
		if (_isPushIntoQueue())
		{
			JYERROR("resource is in queue");
		}
		else
		{
			_PushIntoQueue();
			_SetHashCode(m_spSource->GetHashCode());
			m_eEntityStatus = EntityProperty::ES_LOADING;
			_OnCreateResource();
			IResourceSystem::Instance()->LoadResource(this, m_spSource);
		}
	}

	virtual SOU* GetSourcePtr() { return m_spSource; }
	virtual SOU* GetSourcePtr() const { return m_spSource; }

	FORCEINLINE void PushMetadata(const IMetadata& meta)
	{
		m_spSource->PushSourceMetadata(meta);
	}

	FORCEINLINE void PushMetadata(const ISourceMetadataList& metalist)
	{
		for(auto& it : metalist)
		{
			IMetadata* data = static_cast<IMetadata*>(it);
			m_spSource->PushSourceMetadata(*data);
		}
	}

	FORCEINLINE void ClearMetadata()
	{
		m_spSource->ClearSourceMetadata();
	}

	FORCEINLINE const ISourceMetadataList& GetSourceMetadata()
	{
		return m_spSource->GetSourceMetadata();
	}

	FORCEINLINE void ConnectLoadedEvent(MulticastDelegate<IAsyncSource*>::DelegateT& ptr)	//ֻ��һ����Ч��
	{
		if (!m_pOnLoadedEvent)
		{
			m_pOnLoadedEvent = _NEW LoadSourceEvent();
		}
		m_pOnLoadedEvent->Add(std::move(ptr));
	}

	FORCEINLINE void DependenceSource(IAsyncSource* source)	//����߼��е���A����B��Ҫ����B->DependenceSource(A)
	{
		JY_ASSERT(!source->isLoaded());
		ConnectLoadedEvent(source->DependenceLoaded());//����һ��������Ŀ
	}

	FORCEINLINE bool isSuccess() const
	{
		return m_spSource->isSuccess() && m_pResource;
	}

	void PrepareResource()
	{
		IResourceSystem::Instance()->PreLoadResource(this, m_spSource);
	}

protected:
	FORCEINLINE RES* _GetResourcePtr() { return m_pResource; }
	FORCEINLINE const RES* _GetResourcePtr() const { return m_pResource; }
	FORCEINLINE SOU* _GetSourcePtr() { return m_spSource; }
	FORCEINLINE const SOU* _GetSourcePtr() const { return m_spSource; }

	void _DoCopyEntity(const IAsyncEntity& rhs)
	{
		//source�滻ʵ���ϵ���entityԭ�м�������ֹͣ
		m_eEntityStatus = rhs.m_eEntityStatus;
		if (rhs.m_spSource->IsDerived(ISharedSource::RTTI()))
		{
			m_pResource = rhs.m_pResource;//���ﲻ��Ҫ�������ü���ӦΪ���õ���sourceָ�룬�������ǹ���һ�����ü������ɣ���source�ͷ�ʱ���Զ��������ü���
			m_spSource = rhs.m_spSource;
		}
		else
		{
			//clone�ز��Ƿ�ֱ�ӿ���ԭ�����CreataResource�Ƚ�ͳһ��
			m_spSource = _NEW SOU(*(rhs.m_spSource));
			IAsyncResource* res = m_spSource->CloneResource(rhs.m_spSource->GetResource());
			m_pResource = static_cast<RES*>(res);
		}
		SAFE_DELETE(m_pOnLoadedEvent);
		if (rhs.m_pOnLoadedEvent)
		{
			m_pOnLoadedEvent = _NEW LoadSourceEvent();
			m_pOnLoadedEvent = rhs.m_pOnLoadedEvent;
		}
		//IResourceSystem::Instance()->OnCopyEntity(this);
	}
	void Copy(const IAsyncEntity& rhs, bool depthCopy)
	{
		bool isSrcReady = (rhs.m_eEntityStatus == EntityProperty::ES_READY);
		bool bDstReady = (m_eEntityStatus == EntityProperty::ES_READY);
		bool bShared = rhs.m_spSource->IsDerived(ISharedSource::RTTI());
		IResourceEntity::operator=(rhs);

		if (bShared)
		{
			if (depthCopy)
			{
				m_spSource = _NEW SOU(*(rhs.m_spSource));

				if (isSrcReady)
				{
					m_spSource->SetCloned();
					PrepareResource();

					RES* srcResource = static_cast<RES*>(rhs.m_spSource->GetResource());
					RES* dstResource = static_cast<RES*>(m_spSource->GetResource());
					*dstResource = *srcResource;
					m_pResource = dstResource;
				}
				else
				{
					JYWARNING("resource is not ready, not support depth copy");
				}
			}
			else
			{
				m_pResource = rhs.m_pResource;
				m_spSource = rhs.m_spSource;
			}
		}
		else
		{
			//clone�ز��Ƿ�ֱ�ӿ���ԭ�����CreataResource�Ƚ�ͳһ��
			m_spSource = _NEW SOU(*rhs.m_spSource);
			IAsyncResource* res = m_spSource->CloneResource(rhs.m_spSource->GetResource());
			m_pResource = static_cast<RES*>(res);
		}

		SAFE_DELETE(m_pOnLoadedEvent);
		if (rhs.m_pOnLoadedEvent)
		{
			m_pOnLoadedEvent = _NEW LoadSourceEvent();
			m_pOnLoadedEvent = rhs.m_pOnLoadedEvent;
		}
		//IResourceSystem::Instance()->OnCopyEntity(this);
	}
protected:
	virtual void _OnCreateResource() {};//׼��������Դ��ʱ�����
	virtual void _OnResourceCreated() {};//��Դ������ϵĻص�

	LoadSourceEvent* m_pOnLoadedEvent;
	EntityProperty::EntityStatus m_eEntityStatus;
	RES* m_pResource;//GPU��Դ
	Pointer<SOU> m_spSource;//CPU��Դ
};

template<typename RES, typename SOU>
Object* IAsyncEntity<RES, SOU>::Duplicate() const { JY_ASSERT(false); return NULL; }

template<typename RES, typename SOU>
Rtti IAsyncEntity<RES, SOU>::ms_Type("IAsyncEntity", &IResourceEntity::ms_Type, NULL); 

template<typename RES, typename SOU>
Priority IAsyncEntity<RES, SOU>::ms_Priority;

template<typename RES, typename SOU>
bool IAsyncEntity<RES, SOU>::TerminalProperty()
{
	ms_Type.ClearProperty();
	return true;
}

template<typename RES, typename SOU>
bool IAsyncEntity<RES, SOU>::InitialProperty(Rtti* pRtti)
{
	IAsyncEntity<RES, SOU>* dummyPtr = NULL;
	Property* activeProperty = NULL;
	Rtti* pRttiTemp = pRtti;
	if (!pRtti)
	{
		pRtti = &ms_Type;
	}

	IResourceEntity::InitialProperty(pRtti);
	// ADD Property
	return true;
}

NS_JYE_END