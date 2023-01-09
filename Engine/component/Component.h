#pragma once

#include "Animatable.h"

NS_JYE_BEGIN

class GObject;

#define COMPONENT_FACTORY_FUNC(type)	\
	Object* type::FactoryFunc(Object* host)\
	{\
		return _NEW type(static_cast<GObject*>(host));\
	}

class ENGINE_API Component : public Animatable
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	Component() = delete;
	Component(GObject* pHostNode);
	virtual ~Component();

	GObject* GetParentObject() { return m_Node; }
	const GObject* GetParentObject() const { return m_Node; }

	virtual bool isActiveHierarchy() const override;//自己包括自己的父节点都在启用状态
	virtual void OnRootNodeChange() override;
	virtual void OnActivateChange(bool actived);
	virtual void BeforeDelete();

	FORCEINLINE void OnAttachNode(GObject* node);
	FORCEINLINE void OnDetachNode();

	virtual void BeforeSave(Stream* pStream) override;
	virtual void PostSave(Stream* pStream) override;
	virtual void PostLateLoad(Stream* pStream) override;

protected:

	virtual void _DoOnAttachNode() {};	//当挂接在node上
	virtual void _DoOnDetachNode() {};	//当从node上移除

	GObject* m_Node;
};
DECLARE_Ptr(Component);
TYPE_MARCO(Component);

FORCEINLINE void Component::OnAttachNode(GObject* node)
{
	_DoOnAttachNode();
}

FORCEINLINE void Component::OnDetachNode()
{
	_DoOnDetachNode();
	m_Node = NULL;
}

NS_JYE_END