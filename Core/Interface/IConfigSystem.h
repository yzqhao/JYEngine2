
#pragma once

#include "../Core.h"
#include "../../System/Singleton.hpp"
#include "IConfig.h"

NS_JYE_BEGIN

class CORE_API IConfigSystem
{
	SYSTEM_SINGLETON_DECLEAR(IConfigSystem);
public:
    explicit IConfigSystem();
    ~IConfigSystem();

	virtual IConfig* ConfigureFactory(const String& path) = 0;
	virtual void RecycleBin(IConfig* cf) = 0;

	virtual void SetDefaultConfigure(const String& path) = 0;
	virtual IConfig* GetDefaultConfigure() = 0;
};

NS_JYE_END
