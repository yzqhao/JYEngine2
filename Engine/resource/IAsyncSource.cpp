
#include "IAsyncSource.h"

NS_JYE_BEGIN

IMPLEMENT_RTTI_NoCreateFun(IAsyncSource, Object);
BEGIN_ADD_PROPERTY(IAsyncSource, Object);
REGISTER_PROPERTY(m_ISourceMetadataList, m_ISourceMetadataList, Property::F_SAVE_LOAD_CLONE)
END_ADD_PROPERTY

IAsyncSource::IAsyncSource(ResourceProperty::ResourceType rt, ResourceProperty::SourceType cst)
	: m_eSourceType(cst)
	, m_eResourceType(rt)
	, m_nHashCode(0)
	, m_pSourceLoadedCallBack(NULL)
	, m_rpResourceptr(NULL)
	, m_isSuccess(true)
	, m_isLoaded(false)
	, m_isPreLoaded(false)
	, m_isCloned(false)
{

}

IAsyncSource::~IAsyncSource(void)
{
	ClearSourceMetadata();
	SAFE_DELETE(m_pSourceLoadedCallBack);
}

IAsyncSource::IAsyncSource(const IAsyncSource& rhs)
	: m_eSourceType(rhs.m_eSourceType)
	, m_eResourceType(rhs.m_eResourceType)
	, m_nHashCode(rhs.m_nHashCode)
	, m_rpResourceptr(NULL)
	, m_isSuccess(rhs.isSuccess())
	, m_isPreLoaded(rhs.m_isPreLoaded)
	, m_isLoaded(rhs.m_isLoaded)
	, m_isCloned(rhs.m_isCloned)
	, m_pSourceLoadedCallBack(NULL)
{
	if (rhs.m_pSourceLoadedCallBack)
	{
		m_pSourceLoadedCallBack = _NEW SourceLoadedCallBack(*rhs.m_pSourceLoadedCallBack);
	}
	for (auto it : rhs.m_ISourceMetadataList)
	{
		m_ISourceMetadataList.push_back(static_cast<IMetadata*>(it->Duplicate()));
	}
}

MulticastDelegate<IAsyncSource*>::DelegateT IAsyncSource::DependenceLoaded()
{
	return MulticastDelegate<IAsyncSource*>::DelegateT::CreateRaw(this, &IAsyncSource::OnDependenceLoaded);
}

void IAsyncSource::OnDependenceLoaded(IAsyncSource* dep)
{
	//这个地方传进来的是其他资源的原始数据，而不是自己的，所以这里不能直接向下传递
	_DependenceReady(dep->isSuccess());
}

void IAsyncSource::_DependenceReady(bool res)
{
	m_isSuccess = m_isSuccess && res;
	if (m_pSourceLoadedCallBack)
	{
		if (_isLoadResource())
		{
			if (m_rpResourceptr == nullptr)
			{
				m_rpResourceptr = _DoCreateResource();
			}

			if (!m_ISourceMetadataList.empty())
			{
				bool res = _DoSetupResource(m_rpResourceptr);
				m_isSuccess = m_isSuccess && res;
			}
			_ReleaseUnnecessaryDataAfterLoaded();
		}
		m_isLoaded = true;
		m_pSourceLoadedCallBack->Broadcast(this, m_rpResourceptr);
		SAFE_DELETE(m_pSourceLoadedCallBack);//节省内存，直接删除
	}
}

void IAsyncSource::_ReleaseUnnecessaryDataAfterLoaded()
{
	_DoReleaseUnnecessaryDataAfterLoaded();
	for(auto& it : m_ISourceMetadataList)
	{
		(it)->ReleaseMetadate();
	}
}

size_t IAsyncSource::_GetResourceHashCode() const
{
	size_t res = 0;
	std::string hash;
	for(auto& it : m_ISourceMetadataList)
	{
		(it)->GetIdentifier(hash);
	}
	res = std::hash<std::string>{}(hash);
	return res;
}

IAsyncResource* IAsyncSource::CloneResource(IAsyncResource* rhs)
{
	m_rpResourceptr = _OnClone(rhs);
	return m_rpResourceptr;
}

IMPLEMENT_RTTI_NoCreateFun(IMonopolySource, IAsyncSource);
BEGIN_ADD_PROPERTY(IMonopolySource, IAsyncSource);
END_ADD_PROPERTY

NS_JYE_END