
#include "Object.h"
#include "Property.h"
#include "Rtti.h"

NS_JYE_BEGIN

IMPLEMENT_RTTI_NoParent_NoCreateFun(Object)
HashMap<String, Object::FactoryFunction> Object::ms_ClassFactory;
IMPLEMENT_INITIAL_NO_CLASS_FACTORY_BEGIN(Object)
IMPLEMENT_INITIAL_NO_CLASS_FACTORY_END
BEGIN_ADD_PROPERTY_ROOT(Object)
REGISTER_PROPERTY(m_uiFlag, Flag, Property::F_CLONE);
END_ADD_PROPERTY

FastObjectManager& Object::GetObjectManager()
{
    static FastObjectManager ms_ObjectManager;
    return  ms_ObjectManager;
}

Object* Object::CloneCreateObject(Object * pObject)
{
	HashMap<Object*, Object*> CloneMap; 
	Object * pNewObject =  _CloneCreateObject(pObject, CloneMap);
	for (auto it : CloneMap)
	{
		it.second->PostClone(it.first);
	}
	return pNewObject;
}

void Object::CloneObject(Object* pObjectSrc, Object* pObjectDest)
{
	HashMap<Object *,Object*> CloneMap;
	_CloneObject(pObjectSrc,pObjectDest, CloneMap);
	for (auto it : CloneMap)
	{
		it.second->PostClone(it.first);
	}
}

Object* Object::_CloneCreateObject(Object * pObject, HashMap<Object *,Object*>& CloneMap)
{
	Object* pNewObject = NULL;
	if (pObject)
	{
		auto uiIndex = CloneMap.find(pObject);

		if (uiIndex == CloneMap.end())
		{
			const Rtti& SrcRtti = pObject->GetType();
			if (pObject->IsHasFlag(OF_GCObject))
			{
				pNewObject = Object::GetInstance(SrcRtti.GetName());
			}
			else
			{
				pNewObject = Object::GetNoGCInstance(SrcRtti.GetName());
			}
			CloneMap.insert({ pObject,pNewObject });
			const Rtti& DestRtti = pNewObject->GetType();

			for (uint i = 0 ; i < SrcRtti.GetPropertyNum() ; i++)
			{
				Property* pProperty = SrcRtti.GetProperty(i);
				if (pProperty->GetFlag() & Property::F_CLONE)
				{
					pProperty->CloneData(pObject, pNewObject, CloneMap);
				}
				else if(pProperty->GetFlag() & Property::F_COPY)
				{
					pProperty->CopyData(pObject,pNewObject);
				}
			}
		}
		else
		{
			pNewObject = uiIndex->second;
		}
	}
	return pNewObject;
}

void Object::_CloneObject(Object* pObjectSrc, Object* pObjectDest, HashMap<Object *,Object*>& CloneMap)
{
	if (!pObjectSrc)
	{
		return ;
	}
	auto uiIndex = CloneMap.find(pObjectSrc);

	if (uiIndex == CloneMap.end())
	{
		const Rtti& SrcRtti = pObjectSrc->GetType();
		const Rtti& DestRtti = pObjectDest->GetType();
		for (uint i = 0 ; i < SrcRtti.GetPropertyNum() ; i++)
		{
			Property * pProperty = SrcRtti.GetProperty(i);
			if (pProperty->GetFlag() & Property::F_CLONE)
			{
				pProperty->CloneData(pObjectSrc, pObjectDest, CloneMap);
			}
			else if(pProperty->GetFlag() & Property::F_COPY)
			{
				pProperty->CopyData(pObjectSrc, pObjectDest);
			}
		}
	}
	else
	{
		JY_ASSERT(0); 
	}
}

Object * Object::GetNoGCInstance(const String& sRttiName, Object* host)
{
	auto i = ms_ClassFactory.find(sRttiName);
	if (i == ms_ClassFactory.end())
		return NULL;

	Object * pObject = i->second(host);
	return pObject;
}

Object * Object::GetInstance(const String& sRttiName, Object* host)
{
	auto i = ms_ClassFactory.find(sRttiName);
	if(i == ms_ClassFactory.end())
		return NULL;

	Object * pObject = i->second(host);
	//VSResourceManager::AddGCObject(pObject);
	return pObject;
}

Object * Object::GetInstance(const Rtti& Rtti, Object* host)
{
	Object* pObject = (Rtti.m_CreateFun)(host);
	//VSResourceManager::AddGCObject(pObject);
	return pObject;
}

Object::Object()
{
	m_uiFlag = 0;
	m_bSerialize = true;
	GetObjectManager().AddObject(this);
}

Object::Object(const Object & object)
{
	m_uiFlag = object.m_uiFlag;
	m_bSerialize = object.m_bSerialize;
	GetObjectManager().AddObject(this);
}

Object& Object::operator =(const Object &object)
{
	m_uiFlag = object.m_uiFlag;
	m_bSerialize = object.m_bSerialize;
	return *this;
}

Object::~Object()
{
	GetObjectManager().DeleteObject(this);
}

bool Object::IsSameType(const Object *pObject) const
{
	return pObject && GetType().IsSameType(pObject->GetType());
}

bool Object::IsDerived(const Object *pObject)const
{
	return pObject && GetType().IsDerived(pObject->GetType());
}

std::string Object::GetObjectIDString() const
{
	char buff[1024];
	int len = sprintf(buff, "0x%llx", GetObjectID());
	return std::string(buff, len);
}

bool Object::IsSameType(const Rtti &Type)const
{
	return GetType().IsSameType(Type);
}

bool Object::IsDerived(const Rtti &Type)const
{
	return GetType().IsDerived(Type);	
}

bool Object::PostClone(Object * pObjectSrc)
{
	return true;
}

void Object::ValueChange(String & Name)
{

}

void Object::LoadedEvent(ResourceProxyBase* pResourceProxy, void* Data)
{

}

//
// FastObjectManager
//
FastObjectManager::FastObjectManager()
{
	m_uiObjectNum = 0;
	ObjectHashTree.reserve(1000);
}

FastObjectManager::~FastObjectManager()
{

}

void FastObjectManager::AddObject(Object * p)
{
	JY_ASSERT(p);
	ObjectHashTree.insert(p);
	m_uiObjectNum++;
}

void FastObjectManager::DeleteObject(Object * p)
{
	JY_ASSERT(p);
	ObjectHashTree.erase(p);
	m_uiObjectNum--;
}

bool FastObjectManager::IsClear()
{
	return m_uiObjectNum == 0;
}

uint FastObjectManager::GetObjectNum()
{
	return m_uiObjectNum;
}


NS_JYE_END
