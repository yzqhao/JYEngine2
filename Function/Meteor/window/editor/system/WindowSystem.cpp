#include "WindowSystem.h"
#include "../widget/hierarchy/MHierarchy.h"
#include "../widget/game/MGame.h"
#include "../widget/assets/MAssets.h"
#include "../widget/inspector/MInspector.h"
#include "../widget/scene/MScene.h"
#include "../widget/toolbar/MToolbar.h"
#include "../widget/selectwindow/MSelectWindow.h"

NS_JYE_BEGIN

MSelectWindow* g_SeclectWindow = nullptr;

SINGLETON_IMPLEMENT(WindowSystem);

WindowSystem::WindowSystem()
	: m_game(nullptr)
{

}

WindowSystem::~WindowSystem()
{

}

void WindowSystem::_OnCreate()
{
	m_game = _NEW MGame();
	g_SeclectWindow = _NEW MSelectWindow();

	m_memuWindowlist.push_back(g_SeclectWindow);
	m_memuWindowlist.push_back(_NEW MAssets());
	m_memuWindowlist.push_back(m_game);
	m_memuWindowlist.push_back(_NEW MHierarchy());
	m_memuWindowlist.push_back(_NEW MInspector());
	m_memuWindowlist.push_back(_NEW MScene());
	m_memuWindowlist.push_back(_NEW MToolbar());
}


void WindowSystem::_OnDestroy()
{

}

void WindowSystem::ClearScene()
{
	for (auto& it : m_memuWindowlist)
	{
		it->ClearScene();
	}
}

void WindowSystem::Update(float dt)
{
	for (auto& it : m_memuWindowlist)
	{
		it->OnGui(dt);
	}
}

NS_JYE_END