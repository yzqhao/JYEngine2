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

	virtual bool isActiveHierarchy() const override;//�Լ������Լ��ĸ��ڵ㶼������״̬
	virtual void OnRootNodeChange() override;
	virtual void OnActivateChange(bool actived);
	virtual void BeforeDelete();

	FORCEINLINE void OnAttachNode(GObject* node);
	FORCEINLINE void OnDetachNode();

	virtual void BeforeSave(Stream* pStream) override;
	virtual void PostSave(Stream* pStream) override;
	virtual void PostLateLoad(Stream* pStream) override;

protected:

	virtual void _DoOnAttachNode() {};	//���ҽ���node��
	virtual void _DoOnDetachNode() {};	//����node���Ƴ�

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