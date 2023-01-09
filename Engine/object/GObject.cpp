
#include "GObject.h"
#include "Core/Interface/ILogSystem.h"
#include "Engine/component/transform/TransformComponent.h"
#include "Engine/component/camera/CameraComponent.h"
#include "Engine/component/light/LightComponent.h"
#include "Scene.h"

NS_JYE_BEGIN

static void GObject_m_GetName(GObject* self, String* value)
{
	*value = self->GetName();
}

static void GObject_m_SetName(GObject* self, String* value)
{
	self->SetName(*value);
}

static void GObject_m_GetLayer(GObject* self, uint64* value)
{
	*value = self->GetLayer();
}

static void GObject_m_SetLayer(GObject* self, uint64* value)
{
	self->SetLayer(*value);
}

IMPLEMENT_RTTI(GObject, Animatable);
BEGIN_ADD_PROPERTY(GObject, Animatable);
REGISTER_PROPERTY(m_name, m_name, Property::F_SAVE_LOAD_CLONE)
//m_pHostScene m_RootNode 序列化特殊处理，序列化场景需要，序列化prefab不参与序列化。gameobject可能是prefab，这时候序列化不需要scene，改到在scene里设置
REGISTER_PROPERTY(m_pHostScene, m_pHostScene, Property::F_SAVE_LOAD_CLONE) 
REGISTER_PROPERTY(m_RootNode, m_RootNode, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_isActiveHierarchy, m_isActiveHierarchy, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_NodeDepth, m_NodeDepth, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_MaskLayer, m_MaskLayer, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_components, m_components, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_children, m_children, Property::F_SAVE_LOAD_CLONE)
ADD_PROPERTY_MEM_FUNCTION(m_name, GObject_m_GetName, GObject_m_SetName)
ADD_PROPERTY_MEM_FUNCTION(m_MaskLayer, GObject_m_GetLayer, GObject_m_SetLayer)
END_ADD_PROPERTY

NODE_FACTORY_FUNC(GObject)
IMPLEMENT_INITIAL_NO_FACTORY_BEGIN(GObject)
IMPLEMENT_INITIAL_END

GObject::GObject(Scene* pHostScene)
	: m_pHostScene(pHostScene)
	, m_RootNode(nullptr)
	, m_isActiveHierarchy(true)
	, m_NodeDepth(0)
	, m_MaskLayer(MC_MASK_DEFAULT_LAYER)
	, m_pAttributeDelegateArray(GraphicDefine::GA_COUNT)
	, m_pMessageDelegateArray(NULL)
{
	if (pHostScene)
	{
		// id不参与序列化，构造gameobject的时候重新生成id
		uint64 temp = pHostScene->GenerateID32(); 
		m_id = temp << 32;
	}
}

GObject::~GObject()
{
	for (int i = 0; i < m_pAttributeDelegateArray.size(); ++i)
	{
		//SAFE_DELETE(m_pAttributeDelegateArray[i]);
	}
	SAFE_DELETE_ARRAY(m_pMessageDelegateArray);
}

bool GObject::isActiveHierarchy() const
{
	return m_isActiveHierarchy;
}

void GObject::OnRootNodeChange()
{
	for (auto it : m_components)
	{
		it->OnRootNodeChange();
	}
	for (auto& subnode : m_children)
	{
		subnode->OnRootNodeChange();
	}
}

void GObject::BeforeDelete()
{
	for (auto pair : m_components)
	{
		pair->OnDetachNode();
	}
	//再回收
	for (auto pair : m_components)
	{
		pair->BeforeDelete();
	}
	m_components.clear();
}

bool GObject::HasComponent(const String& compenent_type_name) const
{
	for (const auto& component : m_components)
	{
		if (component->GetType().GetName() == compenent_type_name)
			return true;
	}

	return false;
}

Component* GObject::CreateComponent(const String& compenent_type_name)
{
	for (const auto& component : m_components)
	{
		if (component->GetType().GetName() == compenent_type_name)
		{
			JYLOG("can't create component with existed content type %s", compenent_type_name);
			return component;
		}
	}

	Component* com = static_cast<Component*>(GetInstance(compenent_type_name, this));
	com->OnAttachNode(this);
	m_components.push_back(com);

#ifdef _EDITOR
	if (&com->GetType() == &CameraComponent::RTTI())
	{
		CameraComponent* cameracom = static_cast<CameraComponent*>(com);
		cameracom->CreateFrustumResource();
	}
	else if (&com->GetType() == &LightComponent::RTTI())
	{
		LightComponent* lightcom = static_cast<LightComponent*>(com);
		lightcom->CreateLightResource();
	}
#endif

	return com;
}

Component* GObject::GetComponent(const String& compenent_type_name)
{
	for (auto& component : m_components)
	{
		if (component->GetType().GetName() == compenent_type_name)
		{
			return component.GetEObject();
		}
	}

	return nullptr;
}

void GObject::RemoveComponent(Component* component)
{
	for (int i = 0; i < m_components.size(); ++i)
	{
		if (component == m_components[i])
		{
			RemoveComponent(i);
			break;
		}
	}
}

void GObject::RemoveComponent(const String& compenent_type_name)
{
	for (int i = 0; i < m_components.size(); ++i)
	{
		if (m_components[i]->GetType().GetName() == compenent_type_name)
		{
			RemoveComponent(i);
			break;
		}
	}
}

void GObject::RemoveComponent(int idx)
{
	m_components[idx]->OnDetachNode();
	m_components.erase(m_components.begin() + idx);
}

Vector<Component*> JYE::GObject::GetComponents() const
{
	Vector<Component*> res(m_components.size());
	for (int i = 0; i < m_components.size(); ++i)
	{
		res[i] = m_components[i];
	}
	return res;
}

bool GObject::AddChild(GObject* actor)
{
	if (NULL != actor
		&& std::find(m_children.begin(), m_children.end(), actor) == m_children.end()
		&& m_RootNode != actor)
	{
		if (NULL != actor->m_RootNode)
		{
			actor->m_RootNode->_DetachNode(actor);
		}

		actor->_SetFather(this);
		m_children.push_back(actor);
		return true;
	}
	JYERROR("Can't attach node object !!");
	return false;
}

bool GObject::DetachNode(GObject* actor)
{
	if (_DetachNode(actor))
	{
		actor->_SetFather(NULL);
		return true;
	}
	return false;
}

void GObject::_RecaculateDepth()
{
	m_NodeDepth = NULL == m_RootNode ? 0 : 1 + m_RootNode->NodeDepth();
	for (auto& subnode : m_children)
	{
		subnode->_RecaculateDepth();
	}
}

void GObject::_DoSetFather(GObject* root)
{
	m_RootNode = root;
}

void GObject::_SetFather(GObject* root)
{
	_DoSetFather(root);
	_SetActiveHierarchy(
		m_RootNode
		? m_RootNode->isActiveHierarchy()
		: true);
	_RecaculateDepth();
	OnRootNodeChange();
}

bool GObject::_DetachNode(GObject* actor)
{
	auto it = std::find(m_children.begin(), m_children.end(), actor);
	if (m_children.end() != it)
	{
		m_children.erase(it);
		return true;
	}
	return false;
}

void GObject::_SetActiveHierarchy(bool act)
{
	bool rootActiveHierarchy = m_RootNode ? m_RootNode->isActiveHierarchy() : true;
	m_isActiveHierarchy = rootActiveHierarchy && isActive() && act;
	for (auto& comp : m_components)
	{
		comp->OnActivateChange(m_isActiveHierarchy);
	}
	for (auto& subnode : m_children)
	{
		subnode->_SetActiveHierarchy(m_isActiveHierarchy);
	}
}

TransformComponent* GObject::_GetTransCom()
{
	return TryGetComponent<TransformComponent>();
}

void GObject::_OnAwake()
{
	for (auto com : m_components)
	{
		com->Awake();
	}
}

void GObject::_OnStart()
{
	for (auto com : m_components)
	{
		com->Start();
	}
}

void GObject::_OnUpdate(float dt)
{
	for (auto com : m_components)
	{
		com->Update(dt);
	}
}

void GObject::_OnLateUpdate(float dt)
{
	for (auto com : m_components)
	{
		com->LateUpdate(dt);
	}
}

void GObject::_OnDestroy()
{
	for (auto com : m_components)
	{
		com->Destroy();
	}
}

void GObject::_OnActiveChanged()
{
	_SetActiveHierarchy(isActive());
}

// ----------------------------------------------- Message ---------------------------------------
void GObject::SendMessage(const IMessage& message, IMessage::SendType type, IMessage::AffectChild aff)
{
	_SendMessage(message, type, aff);
}

void GObject::_SendMessage(const IMessage& message, IMessage::SendType type, IMessage::AffectChild aff)
{
	if (m_pMessageDelegateArray)
	{
		m_pMessageDelegateArray[message.GetMessageType()].Broadcast(message);
	}
	if (aff == IMessage::NEED_AFFECT)
	{
		for (auto& subnode : m_children)
		{
			subnode->OnRootNodeChange();
		}
	}

	if (type & IMessage::MT_FATHERNODE)
	{
		GObject* root = GetRoot();
		if (root)
		{
			root->SendMessage(message, type, aff);
		}
	}
	if (type & IMessage::MT_CHILDNODE)
	{
		for (auto& a : m_children)
		{
			a->SendMessage(message, type, aff);
		}
	}
}

DelegateHandle GObject::RegisterMessageListener(IMessage::MessageType mt, MessageDelegateDelegate& func)
{
	if (NULL == m_pMessageDelegateArray)
	{
		m_pMessageDelegateArray = _NEW MessageDelegate[IMessage::SA_COUNT];
	}
	return m_pMessageDelegateArray[mt].Add(std::move(func));
}

void GObject::RemoveMessageListener(IMessage::MessageType mt, DelegateHandle handle)
{
	if (m_pMessageDelegateArray)
	{
		m_pMessageDelegateArray[mt].Remove(handle);
	}
	else
	{
		JYERROR("node %s do't have any message listener");
	}
}

void GObject::SetAttribute(IMessage::MessageType mt, const Object& obj)
{
	SendMessage(IMessage(NULL, &obj, mt), IMessage::MT_SELFNODE, IMessage::NO_AFFECT);
}

void GObject::RegisterAttributeProvider(GraphicDefine::GetAttribute na, AttributeDelegate& func)
{
	m_pAttributeDelegateArray[na] = std::move(func);
}

void GObject::RemoveAttributeProvider(GraphicDefine::GetAttribute na)
{
	m_pAttributeDelegateArray[na].Clear();
}

DelegateHandle GObject::RegisterCloneNodeListener(CloneDelegateDelegate& func)
{
	return m_cloneDelegate.Add(std::move(func));
}

void GObject::RemoveCloneNodeListener(DelegateHandle handle)
{
	m_cloneDelegate.Remove(handle);
}

GObject* GObject::CloneNode()
{
	GObject* clonenode = static_cast<GObject*>(CloneCreateObject(this));
	GetScene()->CloneEvent(this->GetStaticID(), clonenode);
	return clonenode;
}

void GObject::BeforeSave(Stream* pStream)
{

}

void GObject::PostSave(Stream* pStream)
{

}

void GObject::PostLateLoad(Stream* pStream)
{
	Animatable::PostLoad(pStream);

	for (auto& comp : m_components)
	{
#ifdef _EDITOR
		if (&comp->GetType() == &CameraComponent::RTTI())
		{
			CameraComponent* cameracom = static_cast<CameraComponent*>(&*comp);
			cameracom->CreateFrustumResource();
		}
		else if (&comp->GetType() == &LightComponent::RTTI())
		{
			LightComponent* lightcom = static_cast<LightComponent*>(&*comp);
			lightcom->CreateLightResource();
		}
#endif
		comp->Awake();
	}

	for (auto& child : m_children)
	{
		child->_DoSetFather(this);
	}
}


NS_JYE_END