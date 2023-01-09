
#include "ITimeSystem.h"


NS_JYE_BEGIN

SYSTEM_SINGLETON_IMPLEMENT(ITimeSystem);

ITimeSystem::ITimeSystem()
{
	SYSTEM_SINGLETON_INITIALIZE
}

ITimeSystem::~ITimeSystem()
{
	SYSTEM_SINGLETON_DESTROY
}

NS_JYE_END
