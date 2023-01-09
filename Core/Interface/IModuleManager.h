#pragma once

#include "IModuleInterface.h"
#include "../../System/Singleton.hpp"

NS_JYE_BEGIN

class CORE_API IModuleManager
{
	SYSTEM_SINGLETON_DECLEAR(IModuleManager);
public:
	typedef IModuleInterface* (*EntryFuncPtr)();
	typedef void (*ExitFuncPtr)(IModuleInterface*);

	IModuleManager();
	virtual ~IModuleManager();

	IModuleInterface* LoadPlugin(const String& name);
	void ReleasePlugin(IModuleInterface* plugin);

	virtual void AddLibraryPath(const String& path) = 0;

private:

	virtual IModuleInterface* _DoLoadPlugin(const std::string& name) { return nullptr; };
	virtual void _DoReleasePlugin(IModuleInterface* plugin) {};
};

NS_JYE_END