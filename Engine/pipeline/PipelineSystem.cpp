#include "PipelineSystem.h"
#include "Engine/object/Scene.h"

NS_JYE_BEGIN

SYSTEM_SINGLETON_IMPLEMENT(PipelineSystem);

PipelineSystem::PipelineSystem()
{
	SYSTEM_SINGLETON_INITIALIZE
}

PipelineSystem::~PipelineSystem()
{
	SYSTEM_SINGLETON_DESTROY
}

void PipelineSystem::Update(float dt)
{
	m_RenderScene.clear();
	for (auto itr = m_AttachedScenes.begin(); itr != m_AttachedScenes.end(); itr++)	//每帧重新排序场景，因为可能随时会修改scene sequeence
	{
		m_RenderScene.insert({ itr->second->GetSequence(), itr->second });
	}
	for (auto itr = m_RenderScene.begin(); itr != m_RenderScene.end(); itr++)
	{
		itr->second->Tick(dt);
	}
}

uint PipelineSystem::RenderWindow(IWindow* win, RenderContext& context)
{
	uint drawcall = 0;
	for (auto itr = m_RenderScene.begin(); itr != m_RenderScene.end(); itr++)
	{
		drawcall += itr->second->Render(win, context);
	}
	return drawcall;
}

void PipelineSystem::AttachScene(Scene* sce)
{
	m_AttachedScenes.insert({ sce->GetStaticID(), sce });
}

void PipelineSystem::DetachScene(Scene* sce)
{
	m_AttachedScenes.erase(sce->GetObjectID());
}

NS_JYE_END