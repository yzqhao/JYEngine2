#include "MCmdAddRenderProperty.h"
#include "Engine/object/SceneManager.h"
#include "Engine/object/Scene.h"
#include "util/MSceneManagerExt.h"
#include "Engine/component/render/RenderComponent.h"

NS_JYE_BEGIN

MCmdAddRenderProperty::MCmdAddRenderProperty(int64 id, GraphicDefine::RenderProperty rp)
	: m_goID(id)
	, m_rp(rp)
{

}

MCmdAddRenderProperty::~MCmdAddRenderProperty()
{

}

Object* MCmdAddRenderProperty::_DoIt()
{
	Component* component = GetComponent(m_goID);
	RenderComponent* ren = static_cast<RenderComponent*>(component);
	if (ren)
	{
		ren->SetRenderProperty(m_rp);
	}
	return component;
}

void MCmdAddRenderProperty::_Undo()
{
	Component* component = GetComponent(m_goID);
	RenderComponent* ren = static_cast<RenderComponent*>(component);
	if (ren)
	{
		ren->EraseRenderProperty(m_rp);
	}
}

NS_JYE_END