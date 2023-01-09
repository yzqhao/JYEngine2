#pragma once

#include "Engine/private/Define.h"
#include "Engine/component/Component.h"
#include "Math/3DMath.h"

NS_JYE_BEGIN

class IMessage;

class ENGINE_API TransformComponent : public Component
{
	DECLARE_RTTI
	DECLARE_INITIAL
protected:
	bool                        m_isInherit;

	mutable Math::Mat4		m_WorldTransfrom;//����
	mutable Math::Mat4		m_WorldTransfromInvers;//����
	mutable Math::Mat3		m_WorldRotationMatrix;//��ת����
	mutable bool				m_isWorldTransfromInversChange;
	mutable bool				m_isWorldRotationChange;
	mutable bool				m_isWorldTransfromChange;
	mutable bool				m_isScaleChange;
	mutable bool				m_isRotationChange;
	mutable bool				m_isPositionChange;

	mutable Math::Quaternion	m_WorldRotation;//��ת
	Math::Quaternion			m_LocalRotation;//��ת
	Math::Vec3				m_LocalPosition;//λ��
	Math::Vec3				m_LocalScale;//����
	mutable Math::Vec3		m_WorldPosition;//λ��
	mutable Math::Vec3		m_WorldScale;//����
	Math::Vec3               m_LocalEularAngle;//ŷ����(�����޸���ת�����������)
	DelegateHandle				m_hMessageScale;
	DelegateHandle				m_hMessagePosition;
	DelegateHandle				m_hMessageRotation;
protected:
	TransformComponent() = delete;
	TransformComponent(GObject* pHostNode);
	TransformComponent(const TransformComponent& rhs);
	virtual ~TransformComponent(void);
protected:
	virtual void _DoOnAttachNode();
	virtual void _DoOnDetachNode();
public:
	virtual void OnRootNodeChange();
protected:
	virtual void _DoSetLocalRotation(const Math::Quaternion& r);
	virtual void _DoSetLocalPosition(const Math::Vec3& p);	//���ҽӵ�ʱ�򸸽ڵ�����Ż�Ӱ���ֽڵ����λ��
	virtual void _DoSetLocalScale(const Math::Vec3& s);
private:
	Math::Quaternion _WorldToLocalRotation(const Math::Quaternion& r);
	Math::Vec3 _WorldToLocalPosition(const Math::Vec3& r);
	Math::Vec3 _WorldToLocaScalel(const Math::Vec3& r);
public:
	virtual const void* AttributeWorldPosition() const;
	virtual const void* AttributeWorldScale() const;
	virtual const void* AttributeWorldRotation() const;
	const void* AttributeWorldTransform() const;
	const void* AttributeWorldRotationMatrix() const;
	const void* AttributeWorldTransformInvers() const;
	void MessageWorldRotation(const IMessage& message);
	void MessageWorldPosition(const IMessage& message);
	void MessageWorldScale(const IMessage& message);
public:
	void SetLocalRotation(const Math::Quaternion& r);
	void SetLocalPosition(const Math::Vec3& p);	//���ҽӵ�ʱ�򸸽ڵ�����Ż�Ӱ���ֽڵ����λ��
	void SetLocalScale(const Math::Vec3& s);
	void SetWorldRotation(const Math::Quaternion& r);//�����������ԣ�����ھֲ������ܶ�
	void SetWorldPosition(const Math::Vec3& p);
	void SetWorldScale(const Math::Vec3& s);
	void SetWorldTransform(Math::Mat4& t);
	void SetLocalEularAngle(const Math::Vec3& r);
	void SetInherit(bool inherit);
	virtual const Math::Vec3& GetWorldPosition() const;
	virtual const Math::Vec3& GetWorldScale() const;
	virtual const Math::Quaternion& GetWorldRotation() const;
	virtual const Math::Mat4& GetWorldTransform() const;
	virtual const Math::Mat3& GetWorldRotationMatrix() const;
	virtual const Math::Mat4& GetWorldTransformInvers() const;

	virtual void BeforeSave(Stream* pStream) override;
	virtual void PostSave(Stream* pStream) override;
	virtual void PostLateLoad(Stream* pStream) override;

private:

	void _SetPositionXCallBack(const void* data);
	void _SetPositionYCallBack(const void* data);
	void _SetPositionZCallBack(const void* data);

	float _GetPositionXCallBack();
	float _GetPositionYCallBack();
	float _GetPositionZCallBack();

	void _SetRotationXCallBack(const void* data);
	void _SetRotationYCallBack(const void* data);
	void _SetRotationZCallBack(const void* data);
	void _SetRotationQuatCallBack(const void* data);

	float _GetRotationXCallBack();
	float _GetRotationYCallBack();
	float _GetRotationZCallBack();
	float _GetRotationQuatCallBack();

	void _SetScaleXCallBack(const void* data);
	void _SetScaleYCallBack(const void* data);
	void _SetScaleZCallBack(const void* data);

	float _GettScaleXCallBack();
	float _GettScaleYCallBack();
	float _GettScaleZCallBack();

public:
	FORCEINLINE const bool GetInherit();
	FORCEINLINE const Math::Vec3& GetLocalPosition();
	FORCEINLINE const Math::Vec3& GetLocalScale();
	FORCEINLINE const Math::Quaternion& GetLocalRotation();
	FORCEINLINE const Math::Vec3& GetLocalEularAngle();
};
DECLARE_Ptr(TransformComponent);
TYPE_MARCO(TransformComponent);


FORCEINLINE const bool TransformComponent::GetInherit()
{
	return m_isInherit;
}

FORCEINLINE const Math::Vec3& TransformComponent::GetLocalPosition()
{
	return m_LocalPosition;
}

FORCEINLINE const Math::Vec3& TransformComponent::GetLocalScale()
{
	return m_LocalScale;
}

FORCEINLINE const Math::Quaternion& TransformComponent::GetLocalRotation()
{
	return m_LocalRotation;
}

FORCEINLINE const Math::Vec3& TransformComponent::GetLocalEularAngle()
{
	return m_LocalEularAngle;
}

NS_JYE_END