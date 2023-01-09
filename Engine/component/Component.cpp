
#include "Component.h"
#include "Core/Interface/ILogSystem.h"
#include "Engine/object/GObject.h"
#include "Engine/object/Scene.h"

NS_JYE_BEGIN

IMPLEMENT_RTTI(Component, Animatable);
BEGIN_ADD_PROPERTY(Component, Animatable);
REGISTER_PROPERTY(m_Node, m_Node, Property::F_SAVE_LOAD_CLONE)
END_ADD_PROPERTY 

COMPONENT_FACTORY_FUNC(Component)
IMPLEMENT_INITIAL_NO_FACTORY_BEGIN(Component)
IMPLEMENT_INITIAL_END

Component::Component(GObject* pHostNode)
	: m_Node(pHostNode)
{
	if (pHostNode)
	{
		m_id = pHostNode->GetStaticID() | pHostNode->GetScene()->GenerateID32();
	}
}

Component::~Component()
{

}

bool Component::isActiveHierarchy() const
{
	JY_CHECK(m_Node, "Component : node is null!");
	return isActive() && m_Node->isActiveHierarchy();
}

void Component::OnRootNodeChange()
{

}

void Component::OnActivateChange(bool actived)
{

}

void Component::BeforeDelete()
{

}

void Component::BeforeSave(Stream* pStream)
{

}

void Component::PostSave(Stream* pStream)
{

}

void Component::PostLateLoad(Stream* pStream)
{
	// 序列化完之后调用，先走附件node回调，然后start update
	_DoOnAttachNode();
	if (m_isStarted)
	{
		_OnStart();
	}
	if (isActiveHierarchy())
	{
		_OnUpdate(0);
		_OnLateUpdate(0);
	}
}

NS_JYE_END