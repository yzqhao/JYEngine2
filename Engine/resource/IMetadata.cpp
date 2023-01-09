#include "IMetadata.h"

NS_JYE_BEGIN

IMPLEMENT_RTTI_NoCreateFun(IMetadata, Object);
NO_PROPERTY(IMetadata);

IMPLEMENT_INITIAL_NO_CLASS_FACTORY_BEGIN(IMetadata)
IMPLEMENT_INITIAL_NO_CLASS_FACTORY_END

IMetadata::IMetadata()
{

}

IMetadata::~IMetadata()
{

}

bool IMetadata::isRedirected() const
{
	return false;
}

NS_JYE_END