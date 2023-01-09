
#pragma once

#include "private/Define.h"
#include "../MPanelBase.h"

NS_JYE_BEGIN

class CameraComponent;

class MPCameraComponent : public MPanelBase
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	MPCameraComponent();
	~MPCameraComponent();

	virtual bool OnGui(Component* com, float dt) override;

private:
};
DECLARE_Ptr(MPCameraComponent);

NS_JYE_END