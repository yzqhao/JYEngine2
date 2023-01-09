
#pragma once

#include "private/Define.h"
#include "../MPanelBase.h"

NS_JYE_BEGIN

class RenderComponent;

class MPRenderComponent : public MPanelBase
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	MPRenderComponent();
	~MPRenderComponent();

	virtual bool OnGui(Component* com, float dt) override;

private:
};
DECLARE_Ptr(MPRenderComponent);

NS_JYE_END