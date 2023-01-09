
#include "Meteor.h"
#include "Core/Interface/ITimeSystem.h"
#include "window/MWindow.h"
#include "imguifunction/AdapterImgui.h"
#include "window/project/MProjectManager.h"
#include "util/MSceneManagerExt.h"
#include "window/editor/system/EditorSystem.h"
#include "util/MFileSearchTool.h"
#include "window/editor/command/MCommandManager.h"
#include "window/editor/widget/hierarchy/MHierarchyResManager.h"

NS_JYE_BEGIN

SINGLETON_IMPLEMENT(Meteor);

Meteor::Meteor()
	: m_wnd(nullptr)
	, m_bInit(false)
{
	
}

Meteor::~Meteor()
{
	SAFE_DELETE(m_wnd);
}

void Meteor::_OnCreate()
{
}

void Meteor::_OnDestroy()
{
	if (m_bInit)
	{
		MHierarchyResManager::Destroy();
		MCommandManager::Destroy();
		MFileSearchTool::Destroy();
		EditorSystem::Destroy();
		MProjectManager::Destroy();
		AdapterImgui::Destroy();
		MSceneManagerExt::Destroy();
	}
}

bool Meteor::_MakeSureInit()
{
	if (!m_bInit)
	{
		MSceneManagerExt::Create();
		AdapterImgui::Create();
		MProjectManager::Create();
		EditorSystem::Create();
		MFileSearchTool::Create();
		MCommandManager::Create();
		MHierarchyResManager::Create();

		m_wnd = _NEW MWindow();
		m_bInit = true;
		return false;
	}

	return true;
}

void Meteor::Update()
{
	bool bupdate = _MakeSureInit();

	if (bupdate)	// 没初始化完不要刷新
	{
		m_wnd->Update(ITimeSystem::Instance()->GetDetTime());
	}
}

NS_JYE_END