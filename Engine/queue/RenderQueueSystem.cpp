
#include "RenderQueueSystem.h"
#include "IRenderQueue.h"
#include "Engine/object/Scene.h"

NS_JYE_BEGIN

SINGLETON_IMPLEMENT(RenderQueueSystem);

RenderQueueSystem::RenderQueueSystem()
{

}

RenderQueueSystem::~RenderQueueSystem()
{

}

void RenderQueueSystem::Update()
{
	for (auto renderqueue : m_RenderQueues)
	{
		renderqueue.second->Update();
	}
}

IRenderQueue* RenderQueueSystem::CreateRenderQueue(Scene* scenerender)
{
	IRenderQueue* res = _NEW IRenderQueue;
	AttachRenderQueue(res);
	return res;
}

void RenderQueueSystem::DeleteRenderQueue(Scene* scenerender)
{
	uint64 renderkey = scenerender->GetObjectID();
	auto itr = m_RenderQueues.find(renderkey);
	if (itr != m_RenderQueues.end())
	{
		DetachRenderQueue(itr->second);
		SAFE_DELETE(itr->second);
	}
}

void RenderQueueSystem::AttachRenderQueue(IRenderQueue* renderqueue)
{
	m_RenderQueues.insert({ (uint64)renderqueue, renderqueue });
}

void RenderQueueSystem::DetachRenderQueue(IRenderQueue* renderqueue)
{
	m_RenderQueues.erase((uint64)renderqueue);
}

NS_JYE_END