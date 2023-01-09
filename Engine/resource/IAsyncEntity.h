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

	// 序列化才会调用，其他情况禁止调用
	IAsyncEntity(Object* obj)
		: m_eEntityStatus(EntityProperty::ES_NEWBORN)
		, m_pResource(NULL)
		, m_spSource(NULL)	// 这里会使用序列化的值
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
		_Ready();//资源准备就绪
		m_eEntityStatus = EntityProperty::ES_READY;
		_OnResourceCreated();
	}

	void CreateResource()//开始加载资源
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

	FORCEINLINE void ConnectLoadedEvent(MulticastDelegate<IAsyncSource*>::DelegateT& ptr)	//只有一次有效性
	{
		if (!m_pOnLoadedEvent)
		{
			m_pOnLoadedEvent = _NEW LoadSourceEvent();
		}
		m_pOnLoadedEvent->Add(std::move(ptr));
	}

	FORCEINLINE void DependenceSource(IAsyncSource* source)	//这个逻辑有点绕A依赖B需要调用B->DependenceSource(A)
	{
		JY_ASSERT(!source->isLoaded());
		ConnectLoadedEvent(source->DependenceLoaded());//增加一个依赖项目
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
		//source替换实际上等于entity原有加载流程停止
		m_eEntityStatus = rhs.m_eEntityStatus;
		if (rhs.m_spSource->IsDerived(ISharedSource::RTTI()))
		{
			m_pResource = rhs.m_pResource;//这里不需要增加引用计数应为我拿到了source指针，所以我们共享一个引用计数即可，当source释放时，自动减少引用计数
			m_spSource = rhs.m_spSource;
		}
		else
		{
			//clone素材是否直接拷贝原表调用CreataResource比较统一？
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
			//clone素材是否直接拷贝原表调用CreataResource比较统一？
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
	virtual void _OnCreateResource() {};//准备加载资源的时候调用
	virtual void _OnResourceCreated() {};//资源加载完毕的回掉

	LoadSourceEvent* m_pOnLoadedEvent;
	EntityProperty::EntityStatus m_eEntityStatus;
	RES* m_pResource;//GPU资源
	Pointer<SOU> m_spSource;//CPU资源
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