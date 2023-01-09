
#pragma once

#include "private/Define.h"
#include "../MCommandBase.h"
#include "Math/3DMath.h"
#include "Engine/object/GObject.h"
#include "Engine/component/Component.h"

NS_JYE_BEGIN

template <typename T>
class MCmdPropertyChange : public MCommandBase
{
public:
	MCmdPropertyChange(int64 id, const String& keyname, const T& value)
		: m_goID(id)
		, m_keyname(keyname)
		, m_value(value)
	{
		
	}
	virtual ~MCmdPropertyChange() {}

	virtual Object* _DoIt()
	{
		GObject* node = GetGameObject(m_goID);
		Component* com = GetComponent(m_goID);
		if (com || node)
		{
			const Rtti& rtti = com ? com->GetType() : node->GetType();
			Object* pObj = (Object*)com ? (Object*)com : (Object*)node;
			Property* pro = rtti.GetProperty(m_keyname);
			pro->Get(pObj, &m_prevalue);
			pro->Set(pObj, &m_value);
		}
		return nullptr;
	}

	virtual void _Undo()
	{
		GObject* node = GetGameObject(m_goID);
		Component* com = GetComponent(m_goID);
		if (com || node)
		{
			const Rtti& rtti = com ? com->GetType() : node->GetType();
			Object* pObj = (Object*)com ? (Object*)com : (Object*)node;
			Property* pro = rtti.GetProperty(m_keyname);
			pro->Set(pObj, &m_prevalue);
		}
	}
		 
private:
	int64 m_goID;
	String m_keyname;
	T m_value;
	T m_prevalue;
};

#define DEFINE_M_COM_PROPERTY_CHANGE_TYPE(type)	\
	using MCmdPropertyChange##type = MCmdPropertyChange<type>;	\
	using MCmdPropertyChange##type##Ptr = Pointer<MCmdPropertyChange##type>;	\
	DEFINE_MCMD_RETURN_TYPE(MCmdPropertyChange##type, Object*);

#define DEFINE_M_COM_PROPERTY_CHANGE_TYPE_Math(type)	\
	using MCmdPropertyChange##type = MCmdPropertyChange<Math::type>;	\
	using MCmdPropertyChange##type##Ptr = Pointer<MCmdPropertyChange##type>;	\
	DEFINE_MCMD_RETURN_TYPE(MCmdPropertyChange##type, Object*);

DEFINE_M_COM_PROPERTY_CHANGE_TYPE(bool);
DEFINE_M_COM_PROPERTY_CHANGE_TYPE(int);
DEFINE_M_COM_PROPERTY_CHANGE_TYPE(uint);
DEFINE_M_COM_PROPERTY_CHANGE_TYPE(int64);
DEFINE_M_COM_PROPERTY_CHANGE_TYPE(uint64);
DEFINE_M_COM_PROPERTY_CHANGE_TYPE(float);
DEFINE_M_COM_PROPERTY_CHANGE_TYPE(String);
DEFINE_M_COM_PROPERTY_CHANGE_TYPE_Math(Vec2);
DEFINE_M_COM_PROPERTY_CHANGE_TYPE_Math(Vec3);
DEFINE_M_COM_PROPERTY_CHANGE_TYPE_Math(Vec4);
DEFINE_M_COM_PROPERTY_CHANGE_TYPE_Math(Quaternion);
DEFINE_M_COM_PROPERTY_CHANGE_TYPE_Math(FLinearColor);
	

NS_JYE_END