#pragma once

#include "../Core.h"

NS_JYE_BEGIN

class CORE_API IModuleInterface
{
public:
	virtual ~IModuleInterface()
	{
	}

	virtual void StartupModule()
	{
	}

	virtual void PreUnloadCallback()
	{
	}

	virtual void PostLoadCallback()
	{
	}

	virtual void ShutdownModule()
	{
	}

	virtual bool SupportsDynamicReloading()
	{
		return true;
	}

	virtual bool SupportsAutomaticShutdown()
	{
		return true;
	}

	virtual bool IsGameModule() const
	{
		return false;
	}
};

NS_JYE_END