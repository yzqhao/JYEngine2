
#include "RHI.h"
#include "Core/Interface/IApplication.h"
#include "Engine/platform/windows/ModuleManager.h"

NS_JYE_BEGIN

// Globals.
FDynamicRHI* GDynamicRHI = NULL;

void InitNullRHI()
{
	// Use the null RHI if it was specified on the command line, or if a commandlet is running.
	//IDynamicRHIModule* DynamicRHIModule = &FModuleManager::LoadModuleChecked<IDynamicRHIModule>(TEXT("NullDrv"));

	//GDynamicRHI = DynamicRHIModule->CreateRHI();
	GDynamicRHI->Init();

	GUsingNullRHI = true;
}

static IDynamicRHIModule* LoadDynamicRHIModule()
{
	IDynamicRHIModule* DynamicRHIModule = nullptr;
#if PLATFORM_WIN32
	const char* moduleName = ("BgfxRHI");
	DynamicRHIModule = static_cast<IDynamicRHIModule*>(ModuleManager::Instance()->LoadPlugin(moduleName));
	if (!DynamicRHIModule->IsSupported())
	{
		JYERROR("not support %s", moduleName);
		DynamicRHIModule = NULL;
	}
#endif
	return DynamicRHIModule;
}

FDynamicRHI* PlatformCreateDynamicRHI()
{
	FDynamicRHI* DynamicRHI = nullptr;

	// Load the dynamic RHI module.
	IDynamicRHIModule* DynamicRHIModule = LoadDynamicRHIModule();

	if (DynamicRHIModule)
	{
		// Create the dynamic RHI.
		DynamicRHI = DynamicRHIModule->CreateRHI();
	}

	return DynamicRHI;
}

void RHIInit()
{
	if (!GDynamicRHI)
	{
		if (IApplication::Instance()->IsNullRender())
		{
			InitNullRHI();
		}
		else
		{
			GDynamicRHI = PlatformCreateDynamicRHI();
			if (GDynamicRHI)
			{
				GDynamicRHI->Init();
			}
			else
			{
				// If the platform supports doing so, fall back to the NULL RHI on failure
				InitNullRHI();
			}
		}

		JY_ASSERT(GDynamicRHI);
	}
}

void RHIPostInit(const Vector<uint>& InPixelFormatByteWidth)
{
	JY_ASSERT(GDynamicRHI);
	GDynamicRHI->PostInit();
}

void RHIExit()
{
	if (GDynamicRHI != NULL)
	{
		// Destruct the dynamic RHI.
		GDynamicRHI->Shutdown();
		delete GDynamicRHI;
		GDynamicRHI = NULL;
	}
}

NS_JYE_END