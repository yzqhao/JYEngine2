#include "ILightQueue.h"
#include "Engine/component/light/LightComponent.h"
#include "Engine/object/GObject.h"

NS_JYE_BEGIN

ILightQueue::ILightQueue()
{

}

ILightQueue::~ILightQueue()
{

}

void ILightQueue::Update()
{
	for (auto& it : m_pAttachedComponentMap)
	{
		it.second->TryRecaculateLight();
		it.second->UpdateShadow();
	}
}

void ILightQueue::AttachLightComponent(LightComponent* rc)
{
	m_pAttachedComponentMap.insert({ rc->GetStaticID(), rc });
}

void ILightQueue::DetachLightComponent(LightComponent* rc)
{
	auto it = m_pAttachedComponentMap.find(rc->GetStaticID());
	if (m_pAttachedComponentMap.end() != it)
	{
		m_pAttachedComponentMap.erase(it);
	}
}

void ILightQueue::OnCulling(uint64 mask, int seq, bool scull, GraphicDefine::PiplineType pp, CameraComponent* cam)
{
	m_rpRendering.clear();
	for (auto& it : m_pAttachedComponentMap)
	{
		if (
			it.second->GetParentObject()->isLayer(mask)
			&& it.second->isActiveHierarchy()
			)
		{
			m_rpRendering.push_back(it.second);
		}
	}
}

NS_JYE_END