#pragma once

#include "Engine/private/Define.h"
#include "Core/Propery/ResourceProperty.h"

NS_JYE_BEGIN

class ENGINE_API IAsyncResource
{
public:
	IAsyncResource(ResourceProperty::ResourceType rt, ResourceProperty::SourceType st)
		: m_eResourceType(rt)
		, m_eSourceType(st)
		, m_isDeprecated(false) {}
	IAsyncResource(const IAsyncResource& rhs)
		:m_isDeprecated(rhs.m_isDeprecated)
		, m_eSourceType(rhs.m_eSourceType)
		, m_eResourceType(rhs.m_eResourceType)
	{
	}
	IAsyncResource& operator = (const IAsyncResource& rhs)
	{
		m_isDeprecated = rhs.m_isDeprecated;
		m_eSourceType = rhs.m_eSourceType;
		m_eResourceType = rhs.m_eResourceType;
		return *this;
	}
	virtual ~IAsyncResource() {}
	virtual void ReleaseResource() = 0;	//释放资源

	FORCEINLINE void SetDeprecated(bool isd);
	FORCEINLINE bool isDeprecated();
	FORCEINLINE ResourceProperty::SourceType GetSourceType() const;
	FORCEINLINE ResourceProperty::ResourceType GetResourceType() const;
private:
	virtual bool _DeprecatedFilter(bool isd) = 0;	//处理资源是否需要废弃和重建

	bool m_isDeprecated;
	ResourceProperty::SourceType m_eSourceType;
	ResourceProperty::ResourceType m_eResourceType;
};

FORCEINLINE void IAsyncResource::SetDeprecated(bool isd)
{
	m_isDeprecated = _DeprecatedFilter(isd);
}

FORCEINLINE bool IAsyncResource::isDeprecated()
{
	return m_isDeprecated;
}

FORCEINLINE ResourceProperty::SourceType IAsyncResource::GetSourceType() const
{
	return m_eSourceType;
}

FORCEINLINE ResourceProperty::ResourceType IAsyncResource::GetResourceType() const
{
	return m_eResourceType;
}

//
// ISharedResource
//
class ENGINE_API ISharedResource : public IAsyncResource
{
public:
	ISharedResource(ResourceProperty::ResourceType rt) : IAsyncResource(rt, ResourceProperty::CST_SHARED) {}
	ISharedResource(const ISharedResource& rhs) : IAsyncResource(rhs) {}
	virtual ~ISharedResource() {}
};

//
// IMonopolyResource
//
class ENGINE_API IMonopolyResource : public IAsyncResource
{
public:
	IMonopolyResource(ResourceProperty::ResourceType rt) : IAsyncResource(rt, ResourceProperty::CST_MONOPOLY) {}
	IMonopolyResource(const IMonopolyResource& rhs) : IAsyncResource(rhs) {}
	virtual ~IMonopolyResource() {}
};

NS_JYE_END