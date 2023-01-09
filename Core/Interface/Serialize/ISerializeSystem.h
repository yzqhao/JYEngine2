#pragma once

#include "../../Core.h"
#include "System/Singleton.hpp"

NS_JYE_BEGIN

class ISerializer;

class CORE_API ISerializeSystem
{
	SYSTEM_SINGLETON_DECLEAR(ISerializeSystem);

public:
	ISerializeSystem();
	virtual ~ISerializeSystem();

public:
	virtual ISerializer* SerializerFactory() = 0;
	virtual void RecycleBin(ISerializer* ser) = 0;

};

NS_JYE_END