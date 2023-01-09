
#include "ObjectIdAllocator.h"
#include "Core/Interface/ILogSystem.h"

NS_JYE_BEGIN

uint seed = std::chrono::system_clock::now().time_since_epoch().count();
static std::mt19937_64 m_RandomGenerator(seed);

uint64 ObjectIDAllocator::AllocID()
{
	return m_RandomGenerator();
}

uint ObjectIDAllocator::AllocID32()
{
	return m_RandomGenerator();		// 强制转换位32位
}

NS_JYE_END