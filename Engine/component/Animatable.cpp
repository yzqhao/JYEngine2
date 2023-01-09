
#include "Animatable.h"
#include "Engine/object/ObjectIdAllocator.h"

NS_JYE_BEGIN

static void Animatable_m_GetActive(Animatable* self, bool* value)
{
	*value = self->isActive();
}

static void Animatable_m_SetActive(Animatable* self, bool* value)
{
	self->SetActive(*value);
}

#ifdef _EDITOR
static void Animatable_m_GetEditorUIType(Animatable* self, Animatable::EditorUIType* value)
{
	*value = self->GetEditorUIType();
}

static void Animatable_m_SetEditorUIType(Animatable* self, Animatable::EditorUIType* value)
{
	self->SetEditorUIType(*value);
}
#endif

IMPLEMENT_RTTI_NoCreateFun(Animatable, Object);
BEGIN_ADD_PROPERTY(Animatable, Object);
REGISTER_PROPERTY(m_id, m_id, Property::F_SAVE_LOAD)
REGISTER_PROPERTY(m_isActive, m_isActive, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_isStarted, m_isStarted, Property::F_SAVE_LOAD_CLONE)
//REGISTER_PROPERTY(m_isAwaked, m_isAwaked, Property::F_SAVE_LOAD_CLONE) // awake不能序列化出去呀！
ADD_PROPERTY_MEM_FUNCTION(m_isActive, Animatable_m_GetActive, Animatable_m_SetActive)
#ifdef _EDITOR
REGISTER_PROPERTY(m_EditorUIType, m_EditorUIType, Property::F_SAVE_LOAD_CLONE)
ADD_PROPERTY_MEM_FUNCTION(m_EditorUIType, Animatable_m_GetEditorUIType, Animatable_m_SetEditorUIType)
#endif
END_ADD_PROPERTY

Animatable::Animatable()
	: m_isActive(true)
	, m_isStarted(false)
	, m_isAwaked(false)
#ifdef _EDITOR
	, m_EditorUIType(VISUAL)
#endif
{
	
}

Animatable::~Animatable()
{

}

void Animatable::Awake()
{
	m_isAwaked = true;
	_OnAwake();
}

void Animatable::Start()
{
	m_isStarted = true;
	_OnStart();
}

void Animatable::Update(float dt)
{
	if (isActiveHierarchy())
	{
		_OnUpdate(dt);
	}
}

void Animatable::LateUpdate(float dt)
{
	if (isActiveHierarchy())
	{
		_OnLateUpdate(dt);
	}
}

void Animatable::Destroy()
{
	_OnDestroy();
}

void Animatable::SetStaticID(uint64 id)
{
	// 与旧id相等或者不是第一次设置id，才发送消息
	if (m_id != id)
	{
		if (m_id != 0)
		{
			m_changeIDCallback.Broadcast(this, m_id, id);
		}
		m_id = id;
	}
}


NS_JYE_END