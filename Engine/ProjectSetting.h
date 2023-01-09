#pragma once

#include "Core/Core.h"
#include "System/Singleton.hpp"
#include "Engine/private/Define.h"


NS_JYE_BEGIN

class ENGINE_API ProjectSetting
{
	SINGLETON_DECLARE(ProjectSetting);
public:
	ProjectSetting();
	~ProjectSetting();

	void SetCurrentAdvancedShading(bool isAdvanced);

	FORCEINLINE bool IsCurrentAdvancedShading();
	FORCEINLINE float GetShadowRange() const;
	FORCEINLINE void SetShadowRange(float val);

private:

	bool m_CurrentAdvancedShading;
	float m_shadowRange;
};

FORCEINLINE bool ProjectSetting::IsCurrentAdvancedShading()
{
	return m_CurrentAdvancedShading;
}
FORCEINLINE float ProjectSetting::GetShadowRange() const
{
	return m_shadowRange;
}
FORCEINLINE void ProjectSetting::SetShadowRange(float val)
{
	m_shadowRange = val;
}

NS_JYE_END