
#include "ISharedSource.h"

NS_JYE_BEGIN

IMPLEMENT_RTTI_NoCreateFun(ISharedSource, IAsyncSource);
BEGIN_ADD_PROPERTY(ISharedSource, IAsyncSource);
END_ADD_PROPERTY

ISharedSource::ISharedSource(ResourceProperty::ResourceType rt, bool isfinalizer)
	: IAsyncSource(rt, ResourceProperty::CST_SHARED)
	, m_isFinalizer(isfinalizer)
	, m_isJustDepends(false)
{

}

ISharedSource::~ISharedSource()
{

}

bool ISharedSource::_isLoadResource()
{
	return !isJustDepends();
}

NS_JYE_END