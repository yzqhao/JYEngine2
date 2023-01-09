#pragma once

#include "Engine/private/Define.h"
#include "Math/Frustum.h"
#include "Math/AABB.h"

NS_JYE_BEGIN

class LightComponent;
class CameraComponent;

class ILightInfo
{
public:
	ILightInfo(const LightComponent* lc) :m_pHost(lc) {}
	virtual ~ILightInfo() {}

	virtual bool Intersect(const Math::Frustum& frustum) = 0;//判断是否可见
	virtual void RecaculateLight() = 0;
	virtual float CaculateEffective(const CameraComponent& camera, const Math::AABB& bind) = 0;	//计算衰减
	virtual bool isEffectOnRender(const CameraComponent& camera, const Math::AABB& bind) = 0;	//是否照射在可渲染物体上

protected:
	const LightComponent* const m_pHost;
};

class AmbientLight : public ILightInfo
{
public:
	AmbientLight(const LightComponent* lc) : ILightInfo(lc) {}
	virtual ~AmbientLight() {}
public:
	virtual bool Intersect(const Math::Frustum& frustum);
	virtual void RecaculateLight();
	virtual float CaculateEffective(const CameraComponent& camera, const Math::AABB& bind);
	virtual bool isEffectOnRender(const CameraComponent& camera, const Math::AABB& bind);
};

class DirectionLightInfo : public ILightInfo
{
public:
	DirectionLightInfo(const LightComponent* lc) : ILightInfo(lc) {}
	virtual ~DirectionLightInfo() {}

	virtual bool Intersect(const Math::Frustum& frustum) final override;
	virtual void RecaculateLight() final override;
	virtual float CaculateEffective(const CameraComponent& camera, const Math::AABB& bind) final override;
	virtual bool isEffectOnRender(const CameraComponent& camera, const Math::AABB& bind) final override;
};

class PointLightInfo : public ILightInfo
{
public:
	PointLightInfo(const LightComponent* lc) : ILightInfo(lc) {}
	virtual ~PointLightInfo() {}

	virtual bool Intersect(const Math::Frustum& frustum) final override;
	virtual void RecaculateLight() final override;
	virtual float CaculateEffective(const CameraComponent& camera, const Math::AABB& bind) final override;
	virtual bool isEffectOnRender(const CameraComponent& camera, const Math::AABB& bind) final override;

	Math::AABB m_BindBox;
};

class SpotLightInfo : public ILightInfo
{
public:
	SpotLightInfo(const LightComponent* lc) : ILightInfo(lc) {}
	virtual ~SpotLightInfo() {}

	virtual bool Intersect(const Math::Frustum& frustum) final override;
	virtual void RecaculateLight() final override;
	virtual float CaculateEffective(const CameraComponent& camera, const Math::AABB& bind) final override;
	virtual bool isEffectOnRender(const CameraComponent& camera, const Math::AABB& bind) final override;

	Math::Frustum m_BindFrustum;
};

NS_JYE_END