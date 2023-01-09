#pragma once

#include "../private/Define.h"
#include "Core/Object.h"
#include "Core/Propery/ResourceProperty.h"
#include "IMetadata.h"

#include <atomic>

NS_JYE_BEGIN

class IAsyncResource;

class ENGINE_API IResourceEntity : public Object
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	IResourceEntity();
	virtual ~IResourceEntity(void);

	virtual void ResourceChange(IAsyncResource* res) = 0;//重定向变化，导致的资源变化
	virtual void ResourceCallBack(IAsyncResource* res, size_t hash) = 0;//资源准备好后的回掉

	FORCEINLINE bool isReady() const { return m_isReady; }
	FORCEINLINE bool isPushIntoQueue() const { return m_isPushIntoQueue; }
	FORCEINLINE size_t GetHashCode() const;
	FORCEINLINE size_t GetHashCode();

	const IResourceEntity& operator = (const IResourceEntity& rhs);

protected:
	FORCEINLINE void _Ready();
	FORCEINLINE void _PushIntoQueue();
	FORCEINLINE bool _isPushIntoQueue() const;
	FORCEINLINE void _SetHashCode(size_t hash);
	void _DoCopy(const IResourceEntity& rhs);
private:
	virtual void _OnCreateResource() {}	//准备加载资源的时候调用
	virtual void _OnResourceCreated() {}//资源加载完毕的回掉

	size_t m_HashCode;
	bool m_isReady;
	bool m_isPushIntoQueue;
};

FORCEINLINE void IResourceEntity::_Ready()
{
	JY_ASSERT(false == m_isReady);
	m_isReady = true;
}
FORCEINLINE void IResourceEntity::_PushIntoQueue()
{
	m_isPushIntoQueue = true;
}
FORCEINLINE bool IResourceEntity::_isPushIntoQueue() const
{
	return m_isPushIntoQueue;
}
FORCEINLINE void IResourceEntity::_SetHashCode(size_t hash)
{
	m_HashCode = hash;
}
FORCEINLINE size_t IResourceEntity::GetHashCode() const
{
	return m_HashCode;
}
FORCEINLINE size_t IResourceEntity::GetHashCode()
{
	return m_HashCode;
}

NS_JYE_END