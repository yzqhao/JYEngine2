#include "IResourceEntity.h"
#include "Core/Interface/ILogSystem.h"

NS_JYE_BEGIN

IMPLEMENT_RTTI_NoCreateFun(IResourceEntity, Object);
BEGIN_ADD_PROPERTY(IResourceEntity, Object);
END_ADD_PROPERTY

IResourceEntity::IResourceEntity()
	: m_HashCode(0)
	, m_isReady(false)
	, m_isPushIntoQueue(false)
{

}

IResourceEntity::~IResourceEntity(void)
{
	
}

void IResourceEntity::_DoCopy(const IResourceEntity& rhs)
{
	// _OnHostGone();
	m_isReady = rhs.m_isReady;
	m_HashCode = rhs.m_HashCode;
	m_isPushIntoQueue = rhs.m_isPushIntoQueue;
}

const IResourceEntity& IResourceEntity::operator=(const IResourceEntity& rhs)
{
	_DoCopy(rhs);
	return *this;
}

NS_JYE_END