
#pragma once

#include "private/Define.h"
#include "Core/Interface/IModuleInterface.h"

NS_JYE_BEGIN

class FileSystemModulde : public IModuleInterface
{
public:
	FileSystemModulde() {}
	virtual ~FileSystemModulde() {}

	virtual void StartupModule();
	virtual void ShutdownModule();
};

JY_EXTERN FILE_SYSTEM_API IModuleInterface* CreateSystem();
JY_EXTERN FILE_SYSTEM_API void DestroySystem(IModuleInterface* plugin);

NS_JYE_END