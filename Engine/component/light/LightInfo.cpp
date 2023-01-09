
#include "LightInfo.h"
#include "LightComponent.h"
#include "../camera/CameraComponent.h"
#include "Math/MathUtil.h"

NS_JYE_BEGIN

//
// AmbientLight
//
bool AmbientLight::Intersect(const Math::Frustum& frustum)
{
	return true;
}

void AmbientLight::RecaculateLight()
{
}

bool AmbientLight::isEffectOnRender(const CameraComponent& camera, const Math::AABB& bind)
{
	return true;
}

float AmbientLight::CaculateEffective(const CameraComponent& camera, const Math::AABB& bind)
{
	return 0;
}

//
// DirectionLightInfo
//
bool DirectionLightInfo::Intersect(const Math::Frustum& frustum)
{
	return true;
}

void DirectionLightInfo::RecaculateLight()
{
}

bool DirectionLightInfo::isEffectOnRender(const CameraComponent& camera, const Math::AABB& bind)
{
	return true;
}

float DirectionLightInfo::CaculateEffective(const CameraComponent& camera, const Math::AABB& bind)
{
	Math::Vec3 dir = bind.GetCenter() - camera.GetPosition();
	dir.Normalize();
	const Math::Vec3& lightdir = m_pHost->GetWorldDirection();
	float at = lightdir.Dot(dir);
	const Math::Vec3& clr = m_pHost->GetColor();
	return at * (clr.x + clr.y + clr.z);
}

//
// PointLightInfo
//
bool PointLightInfo::Intersect(const Math::Frustum& frustum)
{
	return Math::MathUtil::intersects(frustum, m_BindBox); 
}

void PointLightInfo::RecaculateLight()
{
	const float& r = m_pHost->GetRange();
	m_BindBox = Math::AABB(m_pHost->GetWorldPosition() - Math::Vec3(r, r, r), m_pHost->GetWorldPosition() + Math::Vec3(r, r, r));
}

bool PointLightInfo::isEffectOnRender(const CameraComponent& camera, const Math::AABB& bind)
{
	return Math::MathUtil::intersects(m_BindBox, bind);
}

float PointLightInfo::CaculateEffective(const CameraComponent& camera, const Math::AABB& bind)
{
	float distance = (m_pHost->GetWorldPosition() - bind.GetCenter()).Length();
	distance = Math::Clamp<float>(distance / m_pHost->GetRange(), 0, 1);
	const Math::Vec4& vAttenuation = m_pHost->GetAttenuation();
	float fAttenuation = (1.0 - distance) / (vAttenuation.x + distance * vAttenuation.y + distance * distance * vAttenuation.z);
	const Math::Vec3& clr = m_pHost->GetColor();
	return clr.x * fAttenuation + clr.y * fAttenuation + clr.z * fAttenuation;
}

//
// SpotLightInfo
//
bool SpotLightInfo::Intersect(const Math::Frustum& frustum)
{
	return Math::MathUtil::intersects(m_BindFrustum, frustum);
}

void SpotLightInfo::RecaculateLight()
{
	Math::Mat4 mat =
		Math::Mat4::CreateLookAt(
			m_pHost->GetWorldPosition(),
			m_pHost->GetWorldPosition() + m_pHost->GetWorldDirection(),
			m_pHost->GetWorldUp())
		* Math::Mat4::CreateOrthographic(
			m_pHost->GetLightAngle().y,
			1.0f,
			0.001f,
			m_pHost->GetRange());

	m_BindFrustum.FromMatrix(mat, mat.GetInversed());
}

bool SpotLightInfo::isEffectOnRender(const CameraComponent& camera, const Math::AABB& bind)
{
	return Math::MathUtil::intersects(m_BindFrustum, bind);
}

float SpotLightInfo::CaculateEffective(const CameraComponent& camera, const Math::AABB& bind)
{
	Math::Vec3 vLightDiff = m_pHost->GetWorldPosition() - bind.GetCenter();
	float distance = Math::Clamp<float>(vLightDiff.Length() / m_pHost->GetRange(), 0, 1);
	const Math::Vec4& vAttenuation = m_pHost->GetAttenuation();
	float fAttenuation = (1.0 - distance) / (vAttenuation.x + distance * vAttenuation.y + distance * distance * vAttenuation.z);
	const Math::Vec2& vInner_DiffInv = m_pHost->GetLightInnerDiffInv();
	const Math::Vec3& vLight = m_pHost->GetWorldDirection();
	vLightDiff.Normalize();

	const Math::Vec3& clr = m_pHost->GetColor();
	return clr.x * fAttenuation + clr.y * fAttenuation + clr.z * fAttenuation;
}

NS_JYE_END