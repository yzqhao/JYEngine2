
#pragma once

#include "private/Define.h"
#include "Core/Interface/IModuleInterface.h"

NS_JYE_BEGIN

class PipelineSystemModulde : public IModuleInterface
{
public:
	PipelineSystemModulde() {}
	virtual ~PipelineSystemModulde() {}

	virtual void StartupModule();
	virtual void ShutdownModule();
};

JY_EXTERN RENDER_PIPELINE_SYSTEM_API IModuleInterface* CreateSystem();
JY_EXTERN RENDER_PIPELINE_SYSTEM_API void DestroySystem(IModuleInterface* plugin);

NS_JYE_END