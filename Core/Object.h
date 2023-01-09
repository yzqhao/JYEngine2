
#pragma once

#include "Reference.h"
#include "Rtti.h"
#include "Pointer.h"
#include "Type.h"

NS_JYE_BEGIN

class FastObjectManager;
class ResourceProxyBase;

class CORE_API Object : public Reference
{
	friend class FastObjectManager;
	friend class Stream;
	DECLARE_RTTI;
	DECLARE_INITIAL_NO_CLASS_FACTORY;

	using FactoryFunction = Object*(*)(Object* host);
public:
	Object();
	Object(const Object& object);
	Object& operator =(const Object& object);
	virtual ~Object();
	
	static FastObjectManager& GetObjectManager();
	static Object* GetInstance(const String& sRttiName, Object* host = nullptr);
	static Object* GetInstance(const Rtti& Rtti, Object* host = nullptr);
	template<typename T>
	static T * GetInstance(Object* host = nullptr)
	{
		return (T *)GetInstance(T::ms_Type, host);
	}
	
	bool IsSameType(const Object *pObject)const;
	bool IsDerived(const Object *pObject)const;
	bool IsSameType(const Rtti &Type)const;
	bool IsDerived(const Rtti &Type)const;

	FORCEINLINE uint64 GetObjectID() const { return uint64(this); }
	std::string GetObjectIDString() const;

	virtual void BeforeSave(Stream* pStream) { }
	virtual void PostSave(Stream* pStream) { }
	virtual void PostLoad(Stream* pStream) { }
	// 部分场合需要。比如因为node不能序列化scene，所以node需要scene反序列化完成支持才能scene信息。
	// 而node下面部分组件需要scene信息做处理。所以增加一个load之后再去延迟load的处理。
	virtual void PostLateLoad(Stream* pStream) { }	

	virtual void LoadedEvent(ResourceProxyBase* pResourceProxy, void* Data = nullptr);
	
protected:
	static HashMap<String, FactoryFunction> ms_ClassFactory;	// 只有Object类持有
	static Object* GetNoGCInstance(const String& sRttiName, Object* host = nullptr);
	
//debug
public:

	static Object* _CloneCreateObject(Object * pObject, HashMap<Object *,Object*>& CloneMap);
	static void _CloneObject(Object* pObjectSrc ,Object* pObjectDest, HashMap<Object *,Object*>& CloneMap);

	static Object* CloneCreateObject(Object * pObject);
	static void CloneObject(Object * pObjectSrc,Object * pObjectDest);
	
	virtual bool PostClone(Object * pObjectSrc);
	virtual void ValueChange(String & Name);

public:
	enum //Object Flag
	{
		OF_REACH = 0x01,
		OF_UNREACH = 0x02,
		OF_PendingKill = 0x04,
		OF_GCObject = 0x08,
		OF_RootObject = 0x10,
		OF_MAX
	};
	inline void SetFlag(uint uiFlag) { m_uiFlag |= uiFlag; }
	inline void ClearFlag(uint uiFlag) { m_uiFlag &= ~uiFlag; }
	inline bool IsHasFlag(uint uiFlag) { return (m_uiFlag & uiFlag) != 0; }
	inline bool IsSerialize() { return m_bSerialize; }
	inline void SetSerialize(bool bser) { m_bSerialize = bser; }

	uint m_uiFlag;
	bool m_bSerialize;		// 引入一种更灵活的序列化方式，可以支持指定对特定对象不序列化的，这个对象可以放在vector容器，但是不能放在map容器

	inline void DecreRef()
	{
		Decrement((int*)&m_iReference);
		if (!m_iReference)
		{
			if (IsHasFlag(OF_GCObject))
			{
				SetFlag(OF_PendingKill);
			}
			else
			{
				_DELETE this;
			}
		}
	}
};
DECLARE_Ptr(Object);
TYPE_MARCO(Object);

class FastObjectManager
{
public:
	FastObjectManager();
	~FastObjectManager();
	void AddObject(Object * p);
	void DeleteObject(Object * p);
	bool IsClear();
	void PrepareForGC();
	uint GetObjectNum();
protected:
	HashSet<Object*> ObjectHashTree;
	uint m_uiObjectNum;
};

template <class T> T* StaticCast (Object* pkObj) { return (T*)pkObj; }
template <class T> const T* StaticCast (const Object* pkObj) { return (const T*)pkObj; }
template<class T> T* DynamicCast(Object * pObj) { return pObj && pObj->IsDerived(T::ms_Type)?(T*) pObj : nullptr; }
template<class T> const T * DynamicCast(const Object * pObj) { return pObj && pObj->IsDerived(T::ms_Type)?(const T*)pObj : nullptr; }

NS_JYE_END
