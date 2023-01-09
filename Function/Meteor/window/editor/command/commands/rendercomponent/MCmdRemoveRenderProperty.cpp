#include "MCmdRemoveRenderProperty.h"
#include "Engine/object/SceneManager.h"
#include "Engine/object/Scene.h"
#include "util/MSceneManagerExt.h"
#include "Engine/component/render/RenderComponent.h"

NS_JYE_BEGIN

MCmdRemoveRenderProperty::MCmdRemoveRenderProperty(int64 id, GraphicDefine::RenderProperty rp)
	: m_goID(id)
	, m_rp(rp)
{

}

MCmdRemoveRenderProperty::~MCmdRemoveRenderProperty()
{

}

Object* MCmdRemoveRenderProperty::_DoIt()
{
	Component* component = GetComponent(m_goID);
	RenderComponent* ren = static_cast<RenderComponent*>(component);
	if (ren)
	{
		ren->EraseRenderProperty(m_rp);
	}
	return component;
}

void MCmdRemoveRenderProperty::_Undo()
{
	Component* component = GetComponent(m_goID);
	RenderComponent* ren = static_cast<RenderComponent*>(component);
	if (ren)
	{
		ren->SetRenderProperty(m_rp);
	}
}

NS_JYE_END