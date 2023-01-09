#pragma once

#include "Engine/private/Define.h"
#include "Engine/component/Component.h"
#include "Math/3DMath.h"
#include "IMessage.h"
#include "../GraphicDefine.h"

NS_JYE_BEGIN

class Scene;
class TransformComponent;

enum MaskConstant
{
	MC_MASK_DEFAULT_LAYER = 1,
	MC_MASK_EDITOR_UI_LAYER = 1 << 1,
	MC_MASK_EDITOR_SCENE_LAYER = 1 << 2,
	MC_MASK_EDITOR_CLOTHDEBUG_LAYER = 1 << 3,
	MC_MASK_MAX = 64,
};
static const char* MASK_DEFAULT_LAYER_NAME("default");
static const char* MASK_EDITOR_SCENE_LAYER_NAME("editor scene");
static const char* MASK_EDITOR_UI_LAYER_NAME("editor ui");
static const char* MC_MASK_EDITOR_CLOTHDEBUG_LAYER_NAME("editor clothdebug");

#define NODE_FACTORY_FUNC(type)	\
	Object* type::FactoryFunc(Object* host)\
	{\
		return _NEW type(static_cast<Scene*>(host));\
	}

class ENGINE_API GObject : public Animatable
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	GObject() = delete;
	GObject(Scene* pHostScene);
	virtual ~GObject();

	virtual bool isActiveHierarchy() const;
	virtual void OnRootNodeChange();
	virtual void BeforeDelete();

	FORCEINLINE void SetScene(Scene* scene) { m_pHostScene = scene; }
	FORCEINLINE Scene* GetScene() const { return m_pHostScene; }
	FORCEINLINE void SetName(const String& name);
	FORCEINLINE const String& GetName();
	FORCEINLINE GObject* GetRoot();
	FORCEINLINE const GObject* GetRoot() const;
	FORCEINLINE int NodeDepth() const;//该节点的迭代深度
	FORCEINLINE void SetLayer(uint64 mask);
	FORCEINLINE bool isLayer(uint64 mask) { return 0 != (m_MaskLayer & mask); }
	FORCEINLINE uint64 GetLayer() const { return m_MaskLayer; }
	FORCEINLINE void SetPrefabPath(const String& name) { m_PrefabPath = name; }
	FORCEINLINE const String& GetPrefabPath() { return m_PrefabPath; }
	FORCEINLINE void SetHostPrefabPath(const String& name) { m_HostPrefabPath = name; }
	FORCEINLINE const String& GetHostPrefabPath() { return m_HostPrefabPath; }

	bool HasComponent(const String& compenent_type_name) const;
	Component* CreateComponent(const String& compenent_type_name);
	Component* GetComponent(const String& compenent_type_name);
	void RemoveComponent(Component* component);
	void RemoveComponent(const String& compenent_type_name);

	Vector<Pointer<Component>> GetComponentsInteral() const { return m_components; }
	Vector<Component*> GetComponents() const;
	const Vector<GObject*>& GetChildren() const { return m_children; }

	bool AddChild(GObject* node);
	bool DetachNode(GObject* actor);

	void SendMessage(const IMessage& message, IMessage::SendType type, IMessage::AffectChild aff);
	DelegateHandle RegisterMessageListener(IMessage::MessageType mt, MessageDelegateDelegate& func);//注册事件监听
	void RemoveMessageListener(IMessage::MessageType mt, DelegateHandle handle);
	void SetAttribute(IMessage::MessageType mt, const Object& obj);		//设置某个节点属性
	void RegisterAttributeProvider(GraphicDefine::GetAttribute na, AttributeDelegate& func);//注册为一个数据原
	void RemoveAttributeProvider(GraphicDefine::GetAttribute na);
	DelegateHandle RegisterCloneNodeListener(CloneDelegateDelegate& func);
	void RemoveCloneNodeListener(DelegateHandle handle);

	GObject* CloneNode();

	virtual void BeforeSave(Stream* pStream) override;
	virtual void PostSave(Stream* pStream) override;
	virtual void PostLateLoad(Stream* pStream) override;

	template<typename T> const T* Attribute(GraphicDefine::GetAttribute na) const
	{
		return m_pAttributeDelegateArray[na].IsBound() ? (const T*)m_pAttributeDelegateArray[na].Execute() : NULL;
	}

	template<typename T> const T* RootAttribute(GraphicDefine::GetAttribute na) const
	{
		return GetRoot() ? GetRoot()->Attribute<T>(na) : NULL;
	}

	template<typename TComponent> TComponent* TryGetComponent();
	template<typename TComponent> const TComponent* TryGetComponentConst() const;
	template<typename TComponent> TComponent* CreateComponent();
	template<typename TComponent> void RemoveComponent();

protected:
	virtual void _OnAwake();
	virtual void _OnStart();
	virtual void _OnUpdate(float dt);
	virtual void _OnLateUpdate(float dt);
	virtual void _OnDestroy();
	virtual void _OnActiveChanged();

	virtual void _SendMessage(const IMessage& message, IMessage::SendType type, IMessage::AffectChild aff);

	void _RecaculateDepth();
	void _DoSetFather(GObject* root);
	void _SetFather(GObject* root);
	bool _DetachNode(GObject* actor);
	void _SetActiveHierarchy(bool act);
	void RemoveComponent(int idx);	// do remove
	TransformComponent* _GetTransCom();

	String m_name;
	String m_PrefabPath;		//prefab根节点标记 
	String m_HostPrefabPath;	//prefab中所有节点的所属prefab路径标记
	Scene* m_pHostScene;
	GObject* m_RootNode;	//根节点
	bool m_isActiveHierarchy;	//自己和自己的父节点是否激活
	int m_NodeDepth;
	uint64 m_MaskLayer;
	MessageDelegate* m_pMessageDelegateArray;
	Vector<AttributeDelegate> m_pAttributeDelegateArray;	//组建注册提供某些数据
	CloneDelegate m_cloneDelegate;

	Vector<Pointer<Component>> m_components;
	Vector<GObject*> m_children;
};
DECLARE_Ptr(GObject);
TYPE_MARCO(GObject);

FORCEINLINE void GObject::SetName(const String& name)
{
	m_name = name;
}

FORCEINLINE const String& GObject::GetName()
{
	return m_name;
}

FORCEINLINE GObject* GObject::GetRoot()
{
	return m_RootNode;
}

FORCEINLINE const GObject* GObject::GetRoot() const
{
	return m_RootNode;
}

FORCEINLINE int GObject::NodeDepth() const
{
	return m_NodeDepth;
}

FORCEINLINE void GObject::SetLayer(uint64 mask) 
{ 
	m_MaskLayer = mask; 
	if (isLayer(MC_MASK_EDITOR_SCENE_LAYER) || isLayer(MC_MASK_EDITOR_UI_LAYER) || isLayer(MC_MASK_EDITOR_CLOTHDEBUG_LAYER))
	{
		SetSerialize(false);
	}
	else
	{
		SetSerialize(true);
	}
}

template<typename TComponent> 
TComponent* GObject::TryGetComponent()
{
	for (auto& component : m_components)
	{
		if (component->GetType().GetName() == TComponent::ms_Type.GetName())
		{
			return static_cast<TComponent*>(component.GetEObject());
		}
	}

	return nullptr;
}

template<typename TComponent> 
const TComponent* GObject::TryGetComponentConst() const
{
	for (const auto& component : m_components)
	{
		if (component->GetType().GetName() == TComponent::ms_Type.GetName()e)
		{
			return static_cast<const TComponent*>(component.GetEObject());
		}
	}
	return nullptr;
}

template<typename TComponent> 
TComponent* GObject::CreateComponent()
{
	return static_cast<TComponent*>(CreateComponent(TComponent::ms_Type.GetName()));
}

template<typename TComponent> 
void GObject::RemoveComponent()
{
	return static_cast<TComponent*>(RemoveComponent(TComponent::ms_Type.GetName()));
}

NS_JYE_END