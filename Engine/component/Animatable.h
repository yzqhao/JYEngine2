#pragma once

#include "Engine/private/Define.h"
#include "Core/Object.h"
#include "System/Delegate.h"

NS_JYE_BEGIN

static const uint64 s_gameobjectIDMask = ((uint64)(UINT_MAX)) << 32;

class ENGINE_API Animatable : public Object
{
	DECLARE_RTTI
	DECLARE_INITIAL
protected:
	DECLARE_MULTICAST_DELEGATE(ChangeStaticIDEvent, Animatable*, uint64, uint64);
	Animatable();
public:
	virtual ~Animatable();

	virtual bool isActiveHierarchy() const = 0;
	virtual void OnRootNodeChange() = 0;
	virtual void BeforeDelete() = 0;

	void Awake();
	void Start();
	void Update(float dt);
	void LateUpdate(float dt);
	void Destroy();

	void SetStaticID(uint64 id);

	FORCEINLINE bool isActive() const;
	FORCEINLINE void SetActive(bool act);
	FORCEINLINE uint64 GetStaticID() const { return m_id; }
	FORCEINLINE DelegateHandle RegisterChangeIDListener(ChangeStaticIDEventDelegate& func) { return m_changeIDCallback += std::move(func); }
	FORCEINLINE void RemoveChangeIDListener(DelegateHandle handle) { m_changeIDCallback.Remove(handle); }

	FORCEINLINE bool isStarted() const;
	FORCEINLINE bool IsAwaked() const;

protected:

	virtual void _OnAwake() {};
	virtual void _OnStart() {};
	virtual void _OnUpdate(float dt) {};
	virtual void _OnLateUpdate(float dt) {};
	virtual void _OnDestroy() {};
	virtual void _OnActiveChanged() {}

	uint64 m_id;
	bool m_isActive;
	bool m_isStarted;
	bool m_isAwaked;
	ChangeStaticIDEvent m_changeIDCallback;
#ifdef _EDITOR
public:
	enum EditorUIType
	{
		VISUAL = 1 << 0,
		UNVISUAL = 1 << 1,
	};
private:
	EditorUIType m_EditorUIType;
public:
	FORCEINLINE EditorUIType GetEditorUIType()
	{
		return m_EditorUIType;
	}
	FORCEINLINE void SetEditorUIType(EditorUIType type)
	{
		m_EditorUIType = type;
	}
#endif
};
DECLARE_Ptr(Animatable);
TYPE_MARCO(Animatable);

FORCEINLINE bool Animatable::isActive() const
{
	return m_isActive;
}

FORCEINLINE void Animatable::SetActive(bool act)
{
	if (m_isActive != act)
	{
		m_isActive = act;
		_OnActiveChanged();
	}
}

FORCEINLINE bool Animatable::isStarted() const
{
	return m_isStarted;
}

FORCEINLINE bool Animatable::IsAwaked() const
{
	return m_isAwaked;
}

NS_JYE_END