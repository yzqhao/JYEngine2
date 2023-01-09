#pragma once

#include "Engine/private/Define.h"
#include "Engine/GraphicDefine.h"
#include "System/Singleton.hpp"

NS_JYE_BEGIN

class ILightQueue;

class ENGINE_API LightQueueSystem
{
	SINGLETON_DECLARE(LightQueueSystem);
public:
	LightQueueSystem();
	virtual ~LightQueueSystem();

	void Update();

	ILightQueue* CreateEnvironment();
	void DeleteEnvironmen(ILightQueue* en);


protected:

	void OnCreateEnvironment(ILightQueue* en);
	void OnDeleteEnvironmen(ILightQueue* en);

	Map<uint64, ILightQueue*>	m_LightQueues;
};

NS_JYE_END