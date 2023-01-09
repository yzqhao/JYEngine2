
#include "LightQueueSystem.h"
#include "ILightQueue.h"

NS_JYE_BEGIN

SINGLETON_IMPLEMENT(LightQueueSystem); 

LightQueueSystem::LightQueueSystem()
{

}

LightQueueSystem::~LightQueueSystem()
{

}

void LightQueueSystem::Update()
{
	for (auto renderqueue : m_LightQueues)
	{
		renderqueue.second->Update();
	}
}

ILightQueue* LightQueueSystem::CreateEnvironment()
{
	ILightQueue* en = _NEW ILightQueue;
	OnCreateEnvironment(en);
	return en;
}

void LightQueueSystem::DeleteEnvironmen(ILightQueue* en)
{
	OnDeleteEnvironmen(en);
}

void LightQueueSystem::OnCreateEnvironment(ILightQueue* en)
{
	m_LightQueues.insert({ (uint64)en, en });
}

void LightQueueSystem::OnDeleteEnvironmen(ILightQueue* en)
{
	m_LightQueues.erase((uint64)en);
}

NS_JYE_END