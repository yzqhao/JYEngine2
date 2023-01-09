#pragma once

#include "Engine/private/Define.h"
#include "Engine/component/Animatable.h"
#include "Math/3DMath.h"

NS_JYE_BEGIN

class GObject;

class ENGINE_API IMessage
{
public:
	enum MessageType
	{
		SA_POSITION = 0,//�����⼸����Ϣ��������Ҫʱworld������
		SA_ROTATION,
		SA_SCALE,

		SA_ATTACH_NODE,
		SA_DETACH_NODE,
		SA_DELETE_PROPERTY,

		SA_MATERIAL_CHANGE,
		SA_MATERIAL_PARAMETER_CHANGE,
		SA_RENDEROBJECT_CHANGE,
		SA_RENDERPROPERTY_CHANGE,

		SA_MATERIAL_SAHDER_CHANGE,

		SA_COMPONENT_ATTACH, //notify when a new component attach to node

		SA_COUNT,
	};
	enum SendType
	{
		MT_SELFNODE = 1,
		MT_FATHERNODE = 1 << 1,
		MT_CHILDNODE = 1 << 2,
	};
	enum AffectChild
	{
		NO_AFFECT = 0,
		NEED_AFFECT,
	};
public:
	explicit IMessage(const Animatable* sender, const void* data, MessageType type)
		: m_Sender(sender)
		, m_MessageData(data)
		, m_MessageType(type)
	{};
	virtual ~IMessage() {};

	FORCEINLINE bool isValidMessage(const Animatable* ptr) const	//�����Լ������Լ��ľ�����Ч��Ϣ
	{
		return m_Sender != ptr;
	}
	FORCEINLINE MessageType GetMessageType() const
	{
		return m_MessageType;
	}
	const Animatable* GetSender() const
	{
		return m_Sender;
	}
	template<typename T> const T& GetData() const
	{
		return *static_cast<const T*>(m_MessageData);
	}
	template<typename T> const T* GetDataPtr() const
	{
		return static_cast<const T*>(m_MessageData);
	}

protected:
	const Animatable* m_Sender;
	const void* m_MessageData;
	MessageType m_MessageType;
	
};

DECLARE_DELEGATE_RET(AttributeDelegate, const void*);	//����Դ�ṩ���¼�
DECLARE_MULTICAST_DELEGATE(MessageDelegate, const IMessage&);	//�¼���Ӧ����
DECLARE_MULTICAST_DELEGATE(CloneDelegate, uint64, GObject*);	//node ��¡

NS_JYE_END