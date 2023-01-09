#pragma once

#include "Engine/private/Define.h"
#include "Engine/component/Component.h"
#include "LightInfo.h"
#include "Math/3DMath.h"
#include "Engine/GraphicDefine.h"
#include "LightGizmo.h"

NS_JYE_BEGIN

class Scene;
class GObject;
class CameraComponent;
class IMessage;
class RenderTargetEntity;
class TextureEntity;

class ENGINE_API LightComponent : public Component
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	LightComponent() = delete;
	LightComponent(GObject* pHostNode);
	virtual ~LightComponent();

	virtual void BeforeDelete();
	virtual void OnRootNodeChange();

	virtual void BeforeSave(Stream* pStream) override;
	virtual void PostSave(Stream* pStream) override;
	virtual void PostLateLoad(Stream* pStream) override;

	void Setup(GraphicDefine::LightType lt);
	void MessageLoaclPosition(const IMessage& message);
	void MessageLoaclScale(const IMessage& message);
	void MessageLoaclRotation(const IMessage& message);
	bool TryRecaculateLight();
	void UpdateShadow();
	void SetShadowType(GraphicDefine::ShadowType st);
	bool IsEnabledShadowCaster() const;
	int GetShadowResolution() const;
	const Math::Mat4& GetLightCameraViewMatrix() const;
	const Math::Mat4& GetLightCameraProjMatrix() const;
	const Math::Vec3& GetLightCameraLineDepthParam() const;
	const Math::Vec3& GetLightCameraPosition() const;

#ifdef _EDITOR
	void SetLightShow(bool show);
	bool GetGizmoActive();
	void SetGizmoActive(bool isActive);
	void CreateLightResource(); //NodeWrap调用
#endif

	FORCEINLINE const Math::Vec3& GetColor() const;
	FORCEINLINE const Math::Vec3& GetWorldPosition() const;
	FORCEINLINE void SetColor(const Math::Vec3& clr);
	FORCEINLINE void SetRange(float range);
	FORCEINLINE float GetRange() const;
	FORCEINLINE void SetAttenuation(float constant, float liner, float quadratic, float spotexp);
	FORCEINLINE void SetAttenuation(const Math::Vec4& atten);
	FORCEINLINE const Math::Vec4& GetAttenuation() const;
	FORCEINLINE const Math::Vec3& GetWorldDirection() const;
	FORCEINLINE const Math::Vec3& GetWorldUp() const;
	FORCEINLINE void SetLightAngle(const Math::Vec2& a);
	FORCEINLINE const Math::Vec2& GetLightInnerDiffInv() const;
	FORCEINLINE const Math::Vec2& GetLightAngle() const;
	FORCEINLINE void SetIntensity(float intensity);
	FORCEINLINE float GetIntensity() const;
	FORCEINLINE GraphicDefine::LightType GetLightType() const;
	FORCEINLINE GraphicDefine::ShadowType GetShadowType() const;
	FORCEINLINE GraphicDefine::SoftShadowType GetSoftShadowType() const;
	FORCEINLINE void SetSoftShadowType(GraphicDefine::SoftShadowType sst);
	FORCEINLINE float GetShadowBais() const;
	FORCEINLINE void SetShadowBais(float bais);
	FORCEINLINE float GetShadowStrength() const;
	FORCEINLINE void SetShadowStrength(float shadowStrength);
	FORCEINLINE const TextureEntity* GetShadowMap() const;
	FORCEINLINE bool Intersect(const Math::Frustum& frustum);
	FORCEINLINE float CaculateEffective(const CameraComponent& camera, const Math::AABB& bind);
	FORCEINLINE bool isEffectOnRender(const CameraComponent& camera, const Math::AABB& bind);

protected:
	virtual void _DoOnAttachNode() override;
	virtual void _DoOnDetachNode() override;
	virtual void _OnAwake()	override;

	void InitShadowCamera();
	void UpdateShadowSetting();
	void UpdateLightCamera();
	void MakeSurceCreateShadowMap();

	void _ChangeID(Animatable* self, uint64 oldid, uint64 newid);

	Scene* m_pScene;
	GraphicDefine::LightType m_eLightType;
	Math::Vec3 m_Color;	
	float m_LightRange;
	float m_Intensity;
	float m_MaxScale;//缩放转化
	Math::Vec4 m_Attenuation;//衰减，分别代表衰减的常数，1次，2次项, 聚光灯衰减
	Math::Vec3 m_WorldDirection;//光在世界的方向
	Math::Vec3 m_WorldPosition;//光在世界的方向
	Math::Vec3 m_WorldUp;//光在世界的Up
	Math::Vec2 m_LightAngle;//spot光源的内角和外角
	Math::Vec2 m_LightInnerDiffInv;//spot光源的内角和内外角差的逆
	ILightInfo* m_pLightEntity;
	DelegateHandle m_hMessagePosition;
	DelegateHandle m_hMessageScale;
	DelegateHandle m_hMessageRot;
	DelegateHandle m_hChangeStaticID;
	// shadow
	GObject* m_pCameraProxy;
	CameraComponent* m_pShadowMapCamera;
	GraphicDefine::ShadowType m_shadowType;
	GraphicDefine::SoftShadowType m_softShadowType;
	RenderTargetEntity* m_pRenderTargetEntity;
	TextureEntity* m_pShadowColorMap; //绘制线性深度
	float m_shadowBais;
	float m_shadowStrength;
	bool m_isCastShadow;
#ifdef _EDITOR
	LightGizmo* m_LightGizmo;
#endif

	bool m_isLightChange;
};
DECLARE_Ptr(LightComponent);
TYPE_MARCO(LightComponent);

FORCEINLINE const Math::Vec3& LightComponent::GetColor() const 
{ 
	return m_Color; 
}
FORCEINLINE const Math::Vec3& LightComponent::GetWorldPosition() const
{ 
	return m_WorldPosition; 
}
FORCEINLINE void LightComponent::SetColor(const Math::Vec3& clr) 
{ 
	m_Color = clr; 
}
FORCEINLINE void LightComponent::SetRange(float range)
{
	m_LightRange = range;
	m_isLightChange = true;
}
FORCEINLINE float LightComponent::GetRange() const
{
	return m_LightRange * m_MaxScale;
}
FORCEINLINE void LightComponent::SetAttenuation(float constant, float liner, float quadratic, float spotexp)
{
	m_Attenuation.x = constant;
	m_Attenuation.y = liner;
	m_Attenuation.z = quadratic;
	m_Attenuation.w = spotexp;
}
FORCEINLINE void LightComponent::SetAttenuation(const Math::Vec4& atten)
{
	m_Attenuation = atten;
}
FORCEINLINE const Math::Vec4& LightComponent::GetAttenuation() const
{
	return m_Attenuation;
}
FORCEINLINE const Math::Vec3& LightComponent::GetWorldDirection() const
{
	return m_WorldDirection;
}
FORCEINLINE const Math::Vec3& LightComponent::GetWorldUp() const
{
	return m_WorldUp;
}
FORCEINLINE void LightComponent::SetLightAngle(const Math::Vec2& a)
{
	m_LightAngle = Math::Vec2(Math::Min(a.x, a.y), Math::Max(a.x, a.y));
	m_LightInnerDiffInv.x = (float)Math::Cos(a.x * 0.5f);
	m_LightInnerDiffInv.y = 1.0f / (Math::Cos(a.x * 0.5f) - Math::Cos(a.y * 0.5f));
	m_isLightChange = true;
}
FORCEINLINE const Math::Vec2& LightComponent::GetLightInnerDiffInv() const
{
	return m_LightInnerDiffInv;
}
FORCEINLINE const Math::Vec2& LightComponent::GetLightAngle() const
{
	return m_LightAngle;
}
FORCEINLINE void LightComponent::SetIntensity(float intensity)
{
	m_Intensity = intensity;
}
FORCEINLINE float LightComponent::GetIntensity() const
{
	return m_Intensity;
}
FORCEINLINE GraphicDefine::LightType LightComponent::GetLightType() const
{
	return m_eLightType;
}
FORCEINLINE GraphicDefine::ShadowType LightComponent::GetShadowType() const
{
	return m_shadowType;
}
FORCEINLINE GraphicDefine::SoftShadowType LightComponent::GetSoftShadowType() const
{
	return m_softShadowType;
}
FORCEINLINE void LightComponent::SetSoftShadowType(GraphicDefine::SoftShadowType sst)
{
	m_softShadowType = sst;
}
FORCEINLINE float LightComponent::GetShadowBais() const
{
	return m_shadowBais;
}
FORCEINLINE void LightComponent::SetShadowBais(float bais)
{
	m_shadowBais = bais;
}
FORCEINLINE float LightComponent::GetShadowStrength() const
{
	return m_shadowStrength;
}
FORCEINLINE void LightComponent::SetShadowStrength(float shadowStrength)
{
	m_shadowStrength = shadowStrength;
}
FORCEINLINE const TextureEntity* LightComponent::GetShadowMap() const
{
	return m_pShadowColorMap;
}
FORCEINLINE bool LightComponent::Intersect(const Math::Frustum& frustum)//判断是否可见
{
	return m_pLightEntity ? m_pLightEntity->Intersect(frustum) : false;
}
FORCEINLINE float LightComponent::CaculateEffective(const CameraComponent& camera, const Math::AABB& bind)//计算衰减
{
	return m_pLightEntity ? m_pLightEntity->CaculateEffective(camera, bind) : 0.0f;
}
FORCEINLINE bool LightComponent::isEffectOnRender(const CameraComponent& camera, const Math::AABB& bind)//是否照射在可渲染物体上
{
	return m_pLightEntity ? m_pLightEntity->isEffectOnRender(camera, bind) : false;
}
NS_JYE_END