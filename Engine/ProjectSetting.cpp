#include "ProjectSetting.h"
#include "IGraphicSystem.h"
#include "RHI/RHI.h"

NS_JYE_BEGIN

SINGLETON_IMPLEMENT(ProjectSetting);

ProjectSetting::ProjectSetting()
	: m_CurrentAdvancedShading(false)
	, m_shadowRange(5.0f)
{

}

ProjectSetting::~ProjectSetting()
{

}

void ProjectSetting::SetCurrentAdvancedShading(bool isAdvanced)
{
	m_CurrentAdvancedShading = isAdvanced && RHIIsSupportsSRGB();
}

NS_JYE_END