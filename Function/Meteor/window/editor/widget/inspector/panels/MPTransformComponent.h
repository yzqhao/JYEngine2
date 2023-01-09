
#pragma once

#include "private/Define.h"
#include "../MPanelBase.h"

NS_JYE_BEGIN

class TransformComponent;

class MPTransformComponent : public MPanelBase
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	MPTransformComponent();
	~MPTransformComponent();

	bool OnGui(Component* com, float dt);

private:
};
DECLARE_Ptr(MPTransformComponent);

NS_JYE_END