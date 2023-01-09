#include "ISerializeSystem.h"
#include "IDecoder.h"
#include "IEncoder.h"
#include "ISerializer.h"

NS_JYE_BEGIN

SYSTEM_SINGLETON_IMPLEMENT(ISerializeSystem);

ISerializeSystem::ISerializeSystem()
{
	SYSTEM_SINGLETON_INITIALIZE
}

ISerializeSystem::~ISerializeSystem()
{
	SYSTEM_SINGLETON_DESTROY
}

NS_JYE_END