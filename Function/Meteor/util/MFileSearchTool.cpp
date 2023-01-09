#include "MFileSearchTool.h"
#include "Core/Interface/IFileSystem.h"
#include "window/editor/system/MDefined.h"
#include "window/editor/MEditorService.h"
#include "window/editor/widget/MMemuWindowBase.h"
#include "MFileUtility.h"
#include "MFileTree.h"

NS_JYE_BEGIN

SINGLETON_IMPLEMENT(MFileSearchTool);

MFileSearchTool::MFileSearchTool()
{

}

MFileSearchTool::~MFileSearchTool()
{

}

void MFileSearchTool::_OnCreate()
{

}

void MFileSearchTool::_OnDestroy()
{

}

void MFileSearchTool::GetFileTree(MMemuWindowBase* window, StaticDelegate<void()>& event)
{
	DelegateHandle hd = g_editorSvc->RegisterCallback(MulticastDelegate<>::DelegateT::CreateLambda([&]() 
	{
		if (window)
		{
			window->FileSearchingFinish();
		}
		if (event.IsExecute())
		{
			event.Execute();
		}
	}));
	String assetspath = IFileSystem::Instance()->PathAssembly(MDefined::SystemDirectories["assets"]);
	String scene_path = IFileSystem::Instance()->PathAssembly(MDefined::SystemDirectories["scene_asset"]);
	SAFE_DELETE(g_FileTree);
	g_FileTree = MFileUtility::GetFileSystemTree(assetspath);
	g_editorSvc->CallbackFunction(hd);
}

NS_JYE_END