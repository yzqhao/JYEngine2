#include "MCmdSetParameter.h"
#include "Engine/object/SceneManager.h"
#include "Engine/object/Scene.h"
#include "util/MSceneManagerExt.h"
#include "Engine/component/render/RenderComponent.h"
#include "../../../widget/assets/MatTool.h"

NS_JYE_BEGIN

MCmdSetParameter::MCmdSetParameter(int64 id, int idx, RHIDefine::ParameterSlot slot, MaterialParameterPtr cur)
	: m_goID(id)
	, m_idx(idx)
	, m_slot(slot)
	, m_pre(nullptr)
	, m_cur(cur)
{

}

MCmdSetParameter::~MCmdSetParameter()
{

}

Object* MCmdSetParameter::_DoIt()
{
	Component* component = GetComponent(m_goID);
	RenderComponent* ren = static_cast<RenderComponent*>(component);
	if (ren)
	{
		MaterialEntity* mat = ren->GetMaterialEntity(m_idx);
		m_pre = mat->GetParameter(m_slot);
		mat->SetParameter(m_slot, m_cur);
		MatTool::SaveMat(mat);
	}
	return component;
}

void MCmdSetParameter::_Undo()
{
	Component* component = GetComponent(m_goID);
	RenderComponent* ren = static_cast<RenderComponent*>(component);
	if (ren)
	{
		MaterialEntity* mat = ren->GetMaterialEntity(m_idx);
		mat->SetParameter(m_slot, m_pre);
		MatTool::SaveMat(mat);
	}
}

NS_JYE_END