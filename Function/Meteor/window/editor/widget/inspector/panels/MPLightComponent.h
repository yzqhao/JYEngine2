
#pragma once

#include "private/Define.h"
#include "../MPanelBase.h"

NS_JYE_BEGIN

class LightComponent;

class MPLightComponent : public MPanelBase
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	MPLightComponent();
	~MPLightComponent();

	virtual bool OnGui(Component* com, float dt) override;

private:
};
DECLARE_Ptr(MPLightComponent);

NS_JYE_END