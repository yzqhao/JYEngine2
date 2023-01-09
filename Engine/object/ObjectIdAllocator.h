#pragma once

#include "Engine/private/Define.h"

#include <atomic>
#include <random>

NS_JYE_BEGIN

class ENGINE_API ObjectIDAllocator
{
public:

	static uint64 AllocID();
	static uint AllocID32();

private:
};

NS_JYE_END