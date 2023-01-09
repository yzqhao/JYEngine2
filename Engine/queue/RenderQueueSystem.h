#pragma once

#include "Engine/private/Define.h"
#include "Engine/GraphicDefine.h"
#include "System/Singleton.hpp"

NS_JYE_BEGIN

class IRenderQueue;
class Scene;

class ENGINE_API RenderQueueSystem
{
	SINGLETON_DECLARE(RenderQueueSystem);
public:
	RenderQueueSystem();
	virtual ~RenderQueueSystem();

	void Update();

	IRenderQueue* CreateRenderQueue(Scene* scenerender);
	void DeleteRenderQueue(Scene* scenerender);
	

protected:

	void AttachRenderQueue(IRenderQueue* renderqueue);
	void DetachRenderQueue(IRenderQueue* renderqueue);

	Map<uint64, IRenderQueue*>	m_RenderQueues;
};

NS_JYE_END