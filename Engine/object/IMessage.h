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
		SA_POSITION = 0,//发送这几个消息的数据需要时world的数据
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

	FORCEINLINE bool isValidMessage(const Animatable* ptr) const	//不是自己发给自己的就是有效信息
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

DECLARE_DELEGATE_RET(AttributeDelegate, const void*);	//数据源提供者事件
DECLARE_MULTICAST_DELEGATE(MessageDelegate, const IMessage&);	//事件响应函数
DECLARE_MULTICAST_DELEGATE(CloneDelegate, uint64, GObject*);	//node 克隆

NS_JYE_END