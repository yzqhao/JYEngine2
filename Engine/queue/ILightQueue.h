#pragma once

#include "Engine/private/Define.h"
#include "Engine/GraphicDefine.h"

NS_JYE_BEGIN

class LightComponent;
class CameraComponent;

class ENGINE_API ILightQueue
{
	friend LightComponent;
	typedef std::map< uint64, LightComponent* > LightComponentMap;
public:
	ILightQueue();
	virtual ~ILightQueue();

	void Update();


	void AttachLightComponent(LightComponent* rc);
	void DetachLightComponent(LightComponent* rc);

	void OnCulling(uint64 mask, int seq, bool scull, GraphicDefine::PiplineType pp, CameraComponent* cam);

	FORCEINLINE const GraphicDefine::VisableLight& GetVisableLight()
	{
		return m_rpRendering;
	}

protected:

	LightComponentMap m_pAttachedComponentMap;
	GraphicDefine::VisableLight m_rpRendering;
	
};

NS_JYE_END