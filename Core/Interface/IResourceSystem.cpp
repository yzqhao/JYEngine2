#include "IResourceSystem.h"
#include "ILoader.h"

NS_JYE_BEGIN

SYSTEM_SINGLETON_IMPLEMENT(IResourceSystem);

IResourceSystem::IResourceSystem()
{
	SYSTEM_SINGLETON_INITIALIZE;
}

IResourceSystem::~IResourceSystem()
{
	SYSTEM_SINGLETON_DESTROY;
}


NS_JYE_END