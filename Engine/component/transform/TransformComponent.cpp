
#include "TransformComponent.h"
#include "Engine/object/GObject.h"

NS_JYE_BEGIN

static void TransformComponent_m_GetLocalPosition(TransformComponent* self, Math::Vec3* value)
{
	*value = self->GetLocalPosition();
}

static void TransformComponent_m_SetLocalPosition(TransformComponent* self, Math::Vec3* value)
{
	self->SetLocalPosition(*value);
}

static void TransformComponent_m_GetLocalScale(TransformComponent* self, Math::Vec3* value)
{
	*value = self->GetLocalScale();
}

static void TransformComponent_m_SetLocalScale(TransformComponent* self, Math::Vec3* value)
{
	self->SetLocalScale(*value);
}

static void TransformComponent_m_GetLocalRotation(TransformComponent* self, Math::Quaternion* value)
{
	*value = self->GetLocalRotation();
}

static void TransformComponent_m_SetLocalRotation(TransformComponent* self, Math::Quaternion* value)
{
	self->SetLocalRotation(*value);
}

static void TransformComponent_m_GetLocalEularAngle(TransformComponent* self, Math::Vec3* value)
{
	*value = self->GetLocalEularAngle();
}

static void TransformComponent_m_SetLocalEularAngle(TransformComponent* self, Math::Vec3* value)
{
	self->SetLocalEularAngle(*value);
}

IMPLEMENT_RTTI(TransformComponent, Component);
BEGIN_ADD_PROPERTY(TransformComponent, Component);
REGISTER_PROPERTY(m_LocalPosition, m_LocalPosition, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_LocalScale, m_LocalScale, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_LocalRotation, m_LocalRotation, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_LocalEularAngle, m_LocalEularAngle, Property::F_REFLECT_NAME)
ADD_PROPERTY_MEM_FUNCTION(m_LocalPosition, TransformComponent_m_GetLocalPosition, TransformComponent_m_SetLocalPosition)
ADD_PROPERTY_MEM_FUNCTION(m_LocalScale, TransformComponent_m_GetLocalScale, TransformComponent_m_SetLocalScale)
ADD_PROPERTY_MEM_FUNCTION(m_LocalRotation, TransformComponent_m_GetLocalRotation, TransformComponent_m_SetLocalRotation)
ADD_PROPERTY_MEM_FUNCTION(m_LocalEularAngle, TransformComponent_m_GetLocalEularAngle, TransformComponent_m_SetLocalEularAngle)
END_ADD_PROPERTY

COMPONENT_FACTORY_FUNC(TransformComponent)
IMPLEMENT_INITIAL_NO_FACTORY_BEGIN(TransformComponent)
IMPLEMENT_INITIAL_END

TransformComponent::TransformComponent(GObject* pHostNode)
	: Component(pHostNode)
	, m_LocalScale(1, 1, 1)
	, m_isPositionChange(true)
	, m_isRotationChange(true)
	, m_isScaleChange(true)
	, m_isWorldRotationChange(true)
	, m_isWorldTransfromChange(true)
	, m_isWorldTransfromInversChange(true)
	, m_hMessageScale(NULL)
	, m_hMessagePosition(NULL)
	, m_hMessageRotation(NULL)
	, m_isInherit(true)
{

}

TransformComponent::TransformComponent(const TransformComponent& rhs)
	: Component(rhs)
	, m_LocalScale(rhs.m_LocalScale)
	, m_isPositionChange(rhs.m_isPositionChange)
	, m_isRotationChange(rhs.m_isRotationChange)
	, m_isScaleChange(rhs.m_isScaleChange)
	, m_isWorldRotationChange(m_isWorldRotationChange)
	, m_isWorldTransfromChange(rhs.m_isWorldTransfromChange)
	, m_isWorldTransfromInversChange(rhs.m_isWorldTransfromInversChange)
	, m_hMessageScale(NULL)
	, m_hMessagePosition(NULL)
	, m_hMessageRotation(NULL)
	, m_isInherit(true)
{
}

TransformComponent::~TransformComponent(void)
{
}

void TransformComponent::_DoOnAttachNode()
{
	//连接数据源提供者
	GetParentObject()->RegisterAttributeProvider(GraphicDefine::GA_WORLD_SCALE, AttributeDelegate::CreateRaw(this, &TransformComponent::AttributeWorldScale));
	GetParentObject()->RegisterAttributeProvider(GraphicDefine::GA_WORLD_ROTATION, AttributeDelegate::CreateRaw(this, &TransformComponent::AttributeWorldRotation));
	GetParentObject()->RegisterAttributeProvider(GraphicDefine::GA_WORLD_POSITION, AttributeDelegate::CreateRaw(this, &TransformComponent::AttributeWorldPosition));
	GetParentObject()->RegisterAttributeProvider(GraphicDefine::GA_WORLD_TRANSFORM, AttributeDelegate::CreateRaw(this, &TransformComponent::AttributeWorldTransform));
	GetParentObject()->RegisterAttributeProvider(GraphicDefine::GA_WORLD_TRANSFORM_INVERS, AttributeDelegate::CreateRaw(this, &TransformComponent::AttributeWorldTransformInvers));
	GetParentObject()->RegisterAttributeProvider(GraphicDefine::GA_NORMAL_TRANSFORM, AttributeDelegate::CreateRaw(this, &TransformComponent::AttributeWorldRotationMatrix));

	//连接变更监听
	m_hMessageScale = GetParentObject()->RegisterMessageListener(IMessage::SA_SCALE, MessageDelegateDelegate::CreateRaw(this, &TransformComponent::MessageWorldScale));
	m_hMessagePosition = GetParentObject()->RegisterMessageListener(IMessage::SA_POSITION, MessageDelegateDelegate::CreateRaw(this, &TransformComponent::MessageWorldPosition));
	m_hMessageRotation = GetParentObject()->RegisterMessageListener(IMessage::SA_ROTATION, MessageDelegateDelegate::CreateRaw(this, &TransformComponent::MessageWorldRotation));
}

void TransformComponent::_DoOnDetachNode()
{
	GetParentObject()->RemoveAttributeProvider(GraphicDefine::GA_WORLD_SCALE);
	GetParentObject()->RemoveAttributeProvider(GraphicDefine::GA_WORLD_ROTATION);
	GetParentObject()->RemoveAttributeProvider(GraphicDefine::GA_WORLD_POSITION);
	GetParentObject()->RemoveAttributeProvider(GraphicDefine::GA_WORLD_TRANSFORM);
	GetParentObject()->RemoveAttributeProvider(GraphicDefine::GA_WORLD_TRANSFORM_INVERS);
	GetParentObject()->RemoveAttributeProvider(GraphicDefine::GA_NORMAL_TRANSFORM);

	GetParentObject()->RemoveMessageListener(IMessage::SA_SCALE, m_hMessageScale);
	GetParentObject()->RemoveMessageListener(IMessage::SA_POSITION, m_hMessagePosition);
	GetParentObject()->RemoveMessageListener(IMessage::SA_ROTATION, m_hMessageRotation);
}

const void* TransformComponent::AttributeWorldPosition() const
{
	return &GetWorldPosition();
}

const void* TransformComponent::AttributeWorldScale() const
{
	return &GetWorldScale();
}

const void* TransformComponent::AttributeWorldRotation() const
{
	return &GetWorldRotation();
}

const void* TransformComponent::AttributeWorldTransform() const
{
	return &GetWorldTransform();
}

const void* TransformComponent::AttributeWorldRotationMatrix() const
{
	return &GetWorldRotationMatrix();
}

const void* TransformComponent::AttributeWorldTransformInvers() const
{
	return &GetWorldTransformInvers();
}

void TransformComponent::MessageWorldRotation(const IMessage& message)
{
	if (message.isValidMessage(this))
	{
		const Math::Quaternion& r = message.GetData<Math::Quaternion>();
		const Math::Quaternion&& lr = _WorldToLocalRotation(r);
		Math::Vec3&& eular = lr.ToEulerAngle();
		m_LocalEularAngle = Math::Vec3(eular.y, eular.x, eular.z); //dragging updating &&  customer rotation setting
		_DoSetLocalRotation(lr);
	}
}

void TransformComponent::MessageWorldPosition(const IMessage& message)
{
	if (message.isValidMessage(this))
	{
		const Math::Vec3& p = message.GetData<Math::Vec3>();
		const Math::Vec3&& lp = _WorldToLocalPosition(p);
		_DoSetLocalPosition(lp);
	}
}

void TransformComponent::MessageWorldScale(const IMessage& message)
{
	if (message.isValidMessage(this))
	{
		const Math::Vec3& s = message.GetData<Math::Vec3>();
		const Math::Vec3&& ls = _WorldToLocaScalel(s);
		_DoSetLocalScale(ls);
	}
}

void TransformComponent::OnRootNodeChange()
{
	m_isWorldTransfromInversChange = true;
	m_isWorldRotationChange = true;
	m_isWorldTransfromChange = true;
	m_isScaleChange = true;
	m_isRotationChange = true;
	m_isPositionChange = true;
}

void TransformComponent::_DoSetLocalRotation(const Math::Quaternion& r)
{
	m_isWorldTransfromChange = true;
	m_isWorldRotationChange = true;
	m_isRotationChange = true;
	m_LocalRotation = r;
	JY_ASSERT(!Equals(m_LocalRotation.Length(), 0.0));
}

void TransformComponent::_DoSetLocalPosition(const Math::Vec3& p)
{
	m_isWorldTransfromChange = true;
	m_isWorldRotationChange = true;
	m_isPositionChange = true;
	m_LocalPosition = p;
}

void TransformComponent::_DoSetLocalScale(const Math::Vec3& s)
{
	m_isWorldTransfromChange = true;
	m_isWorldRotationChange = true;
	m_isScaleChange = true;
	m_LocalScale = s;
}

Math::Quaternion TransformComponent::_WorldToLocalRotation(const Math::Quaternion& r)
{
	const Math::Quaternion* faRot =
		GetParentObject() && m_isInherit ?
		static_cast<const Math::Quaternion*>(GetParentObject()->RootAttribute<Math::Quaternion>(GraphicDefine::GA_WORLD_ROTATION))
		: NULL;
	return faRot ? r * faRot->Inverse() : r;
}

Math::Vec3 TransformComponent::_WorldToLocalPosition(const Math::Vec3& p)
{
	if (GetParentObject() && m_isInherit)
	{
		Math::Vec3* pos = (Math::Vec3*)GetParentObject()->RootAttribute<Math::Vec3>(GraphicDefine::GA_WORLD_POSITION);
		Math::Vec3* scale = (Math::Vec3*)GetParentObject()->RootAttribute<Math::Vec3>(GraphicDefine::GA_WORLD_SCALE);
		Math::Quaternion* quat = (Math::Quaternion*)GetParentObject()->RootAttribute<Math::Vec3>(GraphicDefine::GA_WORLD_ROTATION);

		Math::Vec3 wpos = pos ? *pos : Math::Vec3(0, 0, 0);
		Math::Vec3 wscale = scale ? *scale : Math::Vec3(1, 1, 1);
		Math::Quaternion wquat = quat ? *quat : Math::Quaternion();

		Math::Quaternion inversequat = wquat.Inverse();
		return  inversequat * (p - wpos) / wscale;
	}
	else
	{
		return p;
	}
}

Math::Vec3 TransformComponent::_WorldToLocaScalel(const Math::Vec3& s)
{
	const Math::Vec3* faSca =
		GetParentObject() && m_isInherit ?
		static_cast<const Math::Vec3*>(GetParentObject()->RootAttribute<Math::Vec3>(GraphicDefine::GA_WORLD_SCALE))
		: NULL;
	return faSca ? s / *faSca : s;
}

void TransformComponent::SetLocalRotation(const Math::Quaternion& r)
{
	Math::Vec3&& eular = r.ToEulerAngle();
	m_LocalEularAngle = Math::Vec3(eular.y, eular.x, eular.z); //dragging updating &&  customer rotation setting
	_DoSetLocalRotation(r);
	if (GetParentObject())GetParentObject()->SendMessage(IMessage(this, &GetWorldRotation(), IMessage::SA_ROTATION), IMessage::MT_SELFNODE, IMessage::NEED_AFFECT);
}

void TransformComponent::SetLocalPosition(const Math::Vec3& p)
{
	_DoSetLocalPosition(p);
	if (GetParentObject())GetParentObject()->SendMessage(IMessage(this, &GetWorldPosition(), IMessage::SA_POSITION), IMessage::MT_SELFNODE, IMessage::NEED_AFFECT);
}

void TransformComponent::SetLocalScale(const Math::Vec3& s)
{
	_DoSetLocalScale(s);
	if (GetParentObject())GetParentObject()->SendMessage(IMessage(this, &GetWorldScale(), IMessage::SA_SCALE), IMessage::MT_SELFNODE, IMessage::NEED_AFFECT);
}

void TransformComponent::SetWorldRotation(const Math::Quaternion& r)
{
	const Math::Quaternion&& rot = _WorldToLocalRotation(r);
	SetLocalRotation(rot);
}

void TransformComponent::SetWorldPosition(const Math::Vec3& p)
{
	const Math::Vec3&& pos = _WorldToLocalPosition(p);
	SetLocalPosition(pos);
}

void TransformComponent::SetWorldScale(const Math::Vec3& s)
{
	const Math::Vec3&& sca = _WorldToLocaScalel(s);
	SetLocalScale(sca);
}

void TransformComponent::SetWorldTransform(Math::Mat4& t)
{
	m_WorldTransfrom = t;
	m_isWorldTransfromChange = false;
	m_isWorldTransfromInversChange = true;
}

const Math::Vec3& TransformComponent::GetWorldPosition() const
{
	if (m_isPositionChange)
	{
		const void* data;
		m_isPositionChange = false;
		data = GetParentObject() && m_isInherit ? GetParentObject()->RootAttribute<Math::Vec3>(GraphicDefine::GA_WORLD_SCALE) : NULL;
		Math::Vec3 fatherscale = NULL == data ? Math::Vec3(1, 1, 1) : *((const Math::Vec3*)data);
		data = GetParentObject() && m_isInherit ? GetParentObject()->RootAttribute<Math::Vec3>(GraphicDefine::GA_WORLD_ROTATION) : NULL;
		Math::Quaternion rot = NULL == data ? Math::Quaternion(0, 0, 0, 1) : *((const Math::Quaternion*)data);
		data = GetParentObject() && m_isInherit ? GetParentObject()->RootAttribute<Math::Vec3>(GraphicDefine::GA_WORLD_POSITION) : NULL;
		m_WorldPosition = NULL == data ? m_LocalPosition * fatherscale : rot * (m_LocalPosition * fatherscale) + *((const Math::Vec3*)data);
	}
	return m_WorldPosition;
}

const Math::Vec3& TransformComponent::GetWorldScale() const
{
	if (m_isScaleChange)
	{
		const void* data;
		m_isScaleChange = false;
		data = GetParentObject() && m_isInherit ? GetParentObject()->RootAttribute<Math::Vec3>(GraphicDefine::GA_WORLD_SCALE) : NULL;
		m_WorldScale = NULL == data ? m_LocalScale : m_LocalScale * *((const Math::Vec3*)data);
	}
	return m_WorldScale;
}

const Math::Quaternion& TransformComponent::GetWorldRotation() const
{
	if (m_isRotationChange)
	{
		const void* data;
		m_isRotationChange = false;
		data = GetParentObject() && m_isInherit ? GetParentObject()->RootAttribute<Math::Vec3>(GraphicDefine::GA_WORLD_ROTATION) : NULL;
		m_WorldRotation = NULL == data ? m_LocalRotation : m_LocalRotation * *((const Math::Quaternion*)data);
	}
	return m_WorldRotation;
}

const Math::Mat4& TransformComponent::GetWorldTransform() const
{
	if (m_isWorldTransfromChange)
	{
		m_isWorldTransfromChange = false;
		m_isWorldTransfromInversChange = true;
		m_WorldTransfrom = Math::MathUtil::Transformation(GetWorldScale(), GetWorldRotation(), GetWorldPosition());
	}
	return m_WorldTransfrom;
}

const Math::Mat3& TransformComponent::GetWorldRotationMatrix() const
{
	if (m_isWorldRotationChange)
	{
		m_isWorldRotationChange = false;
		m_WorldRotationMatrix = GetWorldRotation().ToMatrix33();
	}
	return m_WorldRotationMatrix;
}

const Math::Mat4& TransformComponent::GetWorldTransformInvers() const
{
	if (m_isWorldTransfromInversChange || m_isWorldTransfromChange)
	{
		m_WorldTransfromInvers = GetWorldTransform().GetInversed();
		m_isWorldTransfromInversChange = false;
	}
	return m_WorldTransfromInvers;
}

void TransformComponent::SetLocalEularAngle(const Math::Vec3& r)
{
	m_LocalEularAngle = r; //inpector updating  
	Math::Quaternion quat(r);
	//把SetLocalRotation的内容拆出来用 
	//防止进入欧拉角转四元数再转欧拉角导致反射面板值限制
	_DoSetLocalRotation(quat);
	if (GetParentObject())GetParentObject()->SendMessage(IMessage(this, &GetWorldRotation(), IMessage::SA_ROTATION), IMessage::MT_SELFNODE, IMessage::NEED_AFFECT);
}

void TransformComponent::SetInherit(bool inherit)
{
	if (m_isInherit != inherit)
	{
		Math::Vec3 worldPose = this->GetWorldPosition();
		Math::Vec3 worldScale = this->GetWorldScale();
		Math::Quaternion worldRot = this->GetWorldRotation();
		m_isInherit = inherit;
		this->SetWorldPosition(worldPose);
		this->SetWorldRotation(worldRot);
		this->SetWorldScale(worldScale);
	}
}

void TransformComponent::_SetPositionXCallBack(const void* data)
{
	const Math::Vec3& position = GetLocalPosition();
	Math::Vec3 newPosition(*(float*)data, position.y, position.z);
	SetLocalPosition(newPosition);
}

void TransformComponent::_SetPositionYCallBack(const void* data)
{
	const Math::Vec3& position = GetLocalPosition();
	Math::Vec3 newPosition(position.x, *(float*)data, position.z);
	SetLocalPosition(newPosition);
}

void TransformComponent::_SetPositionZCallBack(const void* data)
{
	const Math::Vec3& position = GetLocalPosition();
	Math::Vec3 newPosition(position.x, position.y, *(float*)data);
	SetLocalPosition(newPosition);
}

void TransformComponent::_SetRotationXCallBack(const void* data)
{
	Math::Vec3 ro(*(float*)data / 180 * Math::PI, m_LocalEularAngle.y, m_LocalEularAngle.z);
	SetLocalEularAngle(ro);
}

void TransformComponent::_SetRotationYCallBack(const void* data)
{
	Math::Vec3 ro(m_LocalEularAngle.x, *(float*)data / 180 * Math::PI, m_LocalEularAngle.z);
	SetLocalEularAngle(ro);
}

void TransformComponent::_SetRotationZCallBack(const void* data)
{
	Math::Vec3 ro(m_LocalEularAngle.x, m_LocalEularAngle.y, *(float*)data / 180 * Math::PI);
	SetLocalEularAngle(ro);
}

void TransformComponent::_SetRotationQuatCallBack(const void* data)
{
	Math::Quaternion quat = *(Math::Quaternion*)(data);
	SetLocalRotation(quat);
}

void TransformComponent::_SetScaleXCallBack(const void* data)
{
	const Math::Vec3& scale = GetLocalScale();
	Math::Vec3 newScale(*(float*)data, scale.y, scale.z);
	SetLocalScale(newScale);
}

void TransformComponent::_SetScaleYCallBack(const void* data)
{
	const Math::Vec3& scale = GetLocalScale();
	Math::Vec3 newScale(scale.x, *(float*)data, scale.z);
	SetLocalScale(newScale);
}

void TransformComponent::_SetScaleZCallBack(const void* data)
{
	const Math::Vec3& scale = GetLocalScale();
	Math::Vec3 newScale(scale.x, scale.y, *(float*)data);
	SetLocalScale(newScale);
}

float TransformComponent::_GetPositionXCallBack()
{
	return GetLocalPosition().x;
}

float TransformComponent::_GetPositionYCallBack()
{
	return GetLocalPosition().y;
}

float TransformComponent::_GetPositionZCallBack()
{
	return GetLocalPosition().z;
}

float TransformComponent::_GetRotationXCallBack()
{
	Math::Vec3 euler;
	const Math::Quaternion& rot = GetLocalRotation();
	euler = rot.ToEulerAngle();
	return euler.x;
}

float TransformComponent::_GetRotationYCallBack()
{
	Math::Vec3 euler;
	const Math::Quaternion& rot = GetLocalRotation();
	euler = rot.ToEulerAngle();
	return euler.y;
}

float TransformComponent::_GetRotationZCallBack()
{
	Math::Vec3 euler;
	const Math::Quaternion& rot = GetLocalRotation();
	euler = rot.ToEulerAngle();
	return euler.z;
}

float TransformComponent::_GetRotationQuatCallBack()
{
	return 1.0f;
}

float TransformComponent::_GettScaleXCallBack()
{
	return GetLocalScale().x;
}

float TransformComponent::_GettScaleYCallBack()
{
	return GetLocalScale().y;
}

float TransformComponent::_GettScaleZCallBack()
{
	return GetLocalScale().z;
}

void TransformComponent::BeforeSave(Stream* pStream)
{
	Component::BeforeSave(pStream);
}

void TransformComponent::PostSave(Stream* pStream)
{
	Component::PostSave(pStream);
}

void TransformComponent::PostLateLoad(Stream* pStream)
{
	Component::PostLateLoad(pStream);

	SetLocalPosition(m_LocalPosition);
	SetLocalScale(m_LocalScale);
	SetLocalRotation(m_LocalRotation);
}

NS_JYE_END