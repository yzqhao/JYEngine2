
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
	// ���ֳ�����Ҫ��������Ϊnode�������л�scene������node��Ҫscene�����л����֧�ֲ���scene��Ϣ��
	// ��node���沿�������Ҫscene��Ϣ��������������һ��load֮����ȥ�ӳ�load�Ĵ���
	virtual void PostLateLoad(Stream* pStream) { }	

	virtual void LoadedEvent(ResourceProxyBase* pResourceProxy, void* Data = nullptr);
	
protected:
	static HashMap<String, FactoryFunction> ms_ClassFactory;	// ֻ��Object�����
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
	bool m_bSerialize;		// ����һ�ָ��������л���ʽ������֧��ָ�����ض��������л��ģ����������Է���vector���������ǲ��ܷ���map����

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
