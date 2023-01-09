
#pragma once

#include "IConfigSystem.h"

NS_JYE_BEGIN

SYSTEM_SINGLETON_IMPLEMENT(IConfigSystem);

IConfigSystem::IConfigSystem()
{
	SYSTEM_SINGLETON_INITIALIZE;
}

IConfigSystem::~IConfigSystem()
{
	SYSTEM_SINGLETON_DESTROY;
}

NS_JYE_END
