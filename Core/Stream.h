
#pragma once

#include "Rtti.h"
#include "Pointer.h"
#include "Type.h"

NS_JYE_BEGIN

class Object;

#define CHECK_INVOKE_OBJECT_SERIALIZE(po, invoke)	\
	Object* pReObj = (Object*)(po);	\
	if (!pReObj || (pReObj && pReObj->IsSerialize()))		\
		invoke;

class CORE_API Stream
{
public:
	enum ArchiveType // Archive Type
	{
		AT_SAVE,
		AT_LOAD,
		AT_LINK,
		AT_REGISTER,
		AT_SIZE,
		AT_POSTLOAD,
		AT_LATER_POSTLOAD,
		AT_OBJECT_COLLECT_GC,
		AT_CLEAR_OBJECT_PROPERTY_GC,
		AT_LOAD_OBJECT_COLLECT_GC,
	}; 
	
	template<class T>
	void Archive(T& Io)
	{
		if (m_uiStreamFlag == AT_LOAD)
		{
			if (TIsEObjectPointerType<T>::Value)
			{
				Object*& Temp = *(Object**)(void*)&Io;
				ReadObjectGUID(Temp);
			}
			else if (TIsEObjectType<T>::Value)
			{
				Object* Key = (Object*)&Io;
				Object* Value = nullptr;
				ReadObjectGUID(Value);
				m_pmVSTypeLoadMap.insert({ Key, Value });
			}
			else if (TIsEObjectSmartPointerType<T>::Value)
			{
				Pointer<Object>& Temp = *(Pointer<Object>*)(void*)&Io;
				ReadObjectGUID(Temp);
			}
			else if (TIsStringType<T>::Value)
			{
				String& Temp = *(String*)(void*)&Io;
				ReadString(Temp);
			}
			else
			{
				Read((void*)&Io, sizeof(T));
			}
		}
		else if (m_uiStreamFlag == AT_SAVE)
		{
			if (TIsEObjectPointerType<T>::Value)
			{
				Object*& Temp = *(Object**)(void*)&Io;
				if (!Temp || (Temp && Temp->IsSerialize()))
					WriteObjectGUID(Temp);
			}
			else if (TIsEObjectSmartPointerType<T>::Value)
			{
				Pointer<Object>& Temp = *(Pointer<Object>*)(void*)&Io;
				if (!Temp || (Temp && Temp->IsSerialize()))
					WriteObjectGUID(Temp);
			}
			else if (TIsStringType<T>::Value)
			{
				String& Temp = *(String*)(void*)&Io;
				WriteString(Temp);
			}
			else if (TIsEObjectType<T>::Value)
			{
				Object* Temp = (Object*)&Io;
				if (!Temp || (Temp && Temp->IsSerialize()))
					WriteObjectGUID(Temp);
			}
			else
			{
				Write((void*)&Io, sizeof(T));
			}
		}
		else if (m_uiStreamFlag == AT_REGISTER || m_uiStreamFlag == AT_POSTLOAD || m_uiStreamFlag == AT_LATER_POSTLOAD)
		{
			if (TIsEObjectPointerType<T>::Value)
			{
				Object*& Temp = *(Object**)(void*)&Io;
				if (!Temp || (Temp && Temp->IsSerialize()))
					ArchiveAll(Temp);
			}
			else if (TIsEObjectSmartPointerType<T>::Value)
			{
				Pointer<Object>& Temp = *(Pointer<Object>*)(void*)&Io;
				if (!Temp || (Temp && Temp->IsSerialize()))
					ArchiveAll(Temp);
			}
			else if (TIsEObjectType<T>::Value)
			{
				Object* Temp = (Object*)&Io;
				if (!Temp || (Temp && Temp->IsSerialize()))
					ArchiveAll(Temp);
			}
		}
		else if (m_uiStreamFlag == AT_SIZE)
		{
			if (TIsEObjectPointerType<T>::Value || TIsEObjectSmartPointerType<T>::Value)
			{
				Object*& Temp = *(Object**)(void*)&Io;
				if (!Temp || (Temp && Temp->IsSerialize()))
					m_uiArchivePropertySize += 4;	// guid, link
			}
			else if (TIsEObjectType<T>::Value)
			{
				Object* Temp = (Object*)&Io;
				if (!Temp || (Temp && Temp->IsSerialize()))
					m_uiArchivePropertySize += 4;	// guid, link
			}
			else if (TIsStringType<T>::Value)
			{
				String& Temp = *(String*)(void*)&Io;
				m_uiArchivePropertySize += GetStrDistUse(Temp);
			}
			else
			{
				m_uiArchivePropertySize += sizeof(T);
			}
		}
		else if (m_uiStreamFlag == AT_OBJECT_COLLECT_GC || m_uiStreamFlag == AT_LOAD_OBJECT_COLLECT_GC)
		{
			if (TIsEObjectPointerType<T>::Value)
			{
				Object*& Temp = *(Object**)(void*)&Io;

				if (Temp)
				{
					if (Temp->IsHasFlag(Object::OF_PendingKill))
					{
						Temp = nullptr;
						return;
					}
					ArchiveAll(Temp);
				}
			}
			else if (TIsEObjectSmartPointerType<T>::Value)
			{
				Pointer<Object>& Temp = *(Pointer<Object>*)(void*)&Io;
				if (Temp)
				{
					if (Temp->IsHasFlag(Object::OF_PendingKill))
					{
						Temp = nullptr;
						return;
					}
					ArchiveAll(Temp);
				}
			}
			else if (TIsEObjectType<T>::Value)
			{
				Object* Temp = (Object*)&Io;
				ArchiveAll(Temp);
			}
		}
		else if (m_uiStreamFlag == AT_CLEAR_OBJECT_PROPERTY_GC)
		{
			if (TIsEObjectPointerType<T>::Value)
			{
				Object*& Temp = *(Object**)(void*)&Io;
				Temp = nullptr;
			}
			else if (TIsEObjectSmartPointerType<T>::Value)
			{
				Pointer<Object>& Temp = *(Pointer<Object>*)(void*)&Io;
				Object* LocalTemp = Temp;
				Temp = nullptr;
				if (LocalTemp)
				{
					if (LocalTemp->IsHasFlag(Object::OF_REACH))
					{
						LocalTemp->ClearFlag(Object::OF_PendingKill);
					}
				}
			}
			else if (TIsEObjectType<T>::Value)
			{
				Object* Temp = (Object*)&Io;
				ArchiveAll(Temp);
			}
		}
		else if (m_uiStreamFlag == AT_LINK)
		{
			if (TIsEObjectPointerType<T>::Value)
			{
				Object*& Temp = *(Object**)(void*)&Io;
				LinkObjectPtr(Temp);
			}
			else if (TIsEObjectSmartPointerType<T>::Value)
			{
				Pointer<Object>& Temp = *(Pointer<Object>*)(void*)&Io;
				LinkObjectPtr(Temp);
			}
			else if (TIsEObjectType<T>::Value)
			{
				Object* Key = (Object*)&Io;
				Object* Value = nullptr;
				Value = GetVSTypeMapValue(Key);
				LinkObjectPtr(Value);
				JY_ASSERT(Value);
				Object::CloneObject(Value, Key);
			}
		}
	}
	template<class T>
	void Archive(Vector<T>& Io)
	{
		if (m_uiStreamFlag == AT_LOAD)
		{
			uint uiNum = 0;
			Archive(uiNum);
			Io.resize(uiNum);
			if (TIsNoNeedLoop<T>::Value && uiNum)
			{
				Read(Io.data(), sizeof(T) * uiNum);
			}
			else
			{
				for (uint i = 0; i < uiNum; i++)
				{
					Archive(Io[i]);
				}
			}
		}
		else if (m_uiStreamFlag == AT_SAVE)
		{
			uint uiNum = Io.size();
			if (TIsNoNeedLoop<T>::Value && uiNum)
			{
				Archive(uiNum);
				Write(Io.data(), sizeof(T) * uiNum);
			}
			else
			{
				int realnum = 0;
				for (uint i = 0; i < uiNum; i++)
				{
					if (TIsEObjectPointerType<T>::Value || TIsEObjectSmartPointerType<T>::Value)
					{
						Object* pReObj = (Object*)(Io[i]);
						if (!pReObj || (pReObj && pReObj->IsSerialize()))
							realnum++;
					}
				}
				Archive(realnum);

				for (uint i = 0; i < uiNum; i++)
				{
					if (TIsEObjectPointerType<T>::Value || TIsEObjectSmartPointerType<T>::Value)
					{
						Object* pReObj = (Object*)(Io[i]);
						if (!pReObj || (pReObj && pReObj->IsSerialize()))
							Archive(Io[i]);
					}
				}
			}
		}
		else if (m_uiStreamFlag == AT_REGISTER || m_uiStreamFlag == AT_LINK
			|| m_uiStreamFlag == AT_POSTLOAD || m_uiStreamFlag == AT_LATER_POSTLOAD 
			|| m_uiStreamFlag == AT_OBJECT_COLLECT_GC || m_uiStreamFlag == AT_LOAD_OBJECT_COLLECT_GC
			|| m_uiStreamFlag == AT_CLEAR_OBJECT_PROPERTY_GC)
		{
			uint uiNum = Io.size();
			for (uint i = 0; i < uiNum; i++)
			{
				if (TIsEObjectPointerType<T>::Value || TIsEObjectSmartPointerType<T>::Value)
				{
					Object* pReObj = (Object*)(Io[i]);
					if (m_uiStreamFlag == AT_REGISTER)
					{
						if (!pReObj || (pReObj && pReObj->IsSerialize()))
							Archive(Io[i]);
					}
					else
					{
						Archive(Io[i]);
					}
				}
			}
		}
		else if (m_uiStreamFlag == AT_SIZE)
		{
			uint uiNum = Io.size();
			if (TIsNoNeedLoop<T>::Value && uiNum)
			{
				Archive(uiNum);
				m_uiArchivePropertySize += sizeof(T) * uiNum;
			}
			else
			{
				int realnum = 0;
				for (uint i = 0; i < uiNum; i++)
				{
					if (TIsEObjectPointerType<T>::Value || TIsEObjectSmartPointerType<T>::Value)
					{
						Object* pReObj = (Object*)(Io[i]);
						if (!pReObj || (pReObj && pReObj->IsSerialize()))
							realnum++;
					}
				}
				Archive(realnum);

				for (uint i = 0; i < uiNum; i++)
				{
					if (TIsEObjectPointerType<T>::Value || TIsEObjectSmartPointerType<T>::Value)
					{
						Object* pReObj = (Object*)(Io[i]);
						if (!pReObj || (pReObj && pReObj->IsSerialize()))
							Archive(Io[i]);
					}
				}
			}
		}
	}

	template<class Key, class Value>
	void Archive(HashMap<Key, Value>& Io)
	{
		if (m_uiStreamFlag == AT_LOAD)
		{
			uint uiNum = Io.size();
			Archive(uiNum);
			for (uint i = 0; i < uiNum; ++i)
			{
				Key sk;
				Value sv;
				Archive(sk);
				Archive(sv);
				Io.insert({ sk, sv });
			}
		}
		else if (m_uiStreamFlag == AT_SAVE)
		{
			uint uiNum = Io.size();
			Archive(uiNum);
			for (auto& it : Io)
			{
				Archive(it.first);
				Archive(it.second);
			}
		}
		else if (m_uiStreamFlag == AT_REGISTER || m_uiStreamFlag == AT_LINK || m_uiStreamFlag == AT_POSTLOAD || m_uiStreamFlag == AT_LATER_POSTLOAD
			|| m_uiStreamFlag == AT_OBJECT_COLLECT_GC || m_uiStreamFlag == AT_LOAD_OBJECT_COLLECT_GC
			|| m_uiStreamFlag == AT_CLEAR_OBJECT_PROPERTY_GC)
		{
			uint uiNum = Io.size();
			for (auto& it : Io)
			{
				Archive(it.first);
				Archive(it.second);
			}
		}
		else if (m_uiStreamFlag == AT_SIZE)
		{
			uint uiNum = Io.size();
			Archive(uiNum);
			
			{
				for (auto& it : Io)
				{
					Archive(it.first);
					Archive(it.second);
				}
			}
		}
	}

	uint GetStreamFlag() const { return m_uiStreamFlag; }
	void SetStreamFlag(uint uiStreamFlag) { m_uiStreamFlag = uiStreamFlag; }
	void AddBufferSize(uint uiSize) { m_uiArchivePropertySize += uiSize; }

	struct ObjectPropertyTable
	{
		ObjectPropertyTable() {}
		ObjectPropertyTable(const String& name, uint nameID) 
			: m_PropertyName(name), m_uiNameID(nameID) {}

		String m_PropertyName;
		uint m_uiNameID{};
		uint m_uiOffset{};
		uint m_uiSize{};
	};

	struct ObjectTableType
	{
		uint m_uiGUID{};
		String m_RttiName;
		uint m_uiOffset{};
		uint m_uiObjectPropertySize{};
		uint m_uiObjectPropertyTableSize{};
		uint m_uiObjectPropertyNum{};
		Vector<ObjectPropertyTable> m_ObjectPropertyTable;
	};
	bool m_bLoadUseGC;
protected:
	uint m_uiStreamFlag;
	uint m_uiArchivePropertySize;
public:
	virtual bool Load(const char* const pcFileName);
	virtual bool Save(const char* const pcFileName);

	virtual bool SaveBuffer();
	virtual bool LoadFromBuffer(unsigned char* pBuffer, uint uiSize);

	bool ArchiveAll(Object* pObject);
public:
	bool RegisterObject(Object* pObject);

	bool RegisterPostLoadObject(Object* pObject);

	bool RegisterReachableObject(Object* pObject);

	Stream();
	~Stream();
	bool Read(void* pvBuffer, uint uiSize);
	bool Write(const void* pvBuffer, uint uiSize);
	bool ReadString(String& str);
	bool WriteString(const String& str);

	static uint GetStrDistUse(const String& Str);
	static uint GetStrDistUse(const char* pCh);
	const Object* GetLoadMapValue(uint uiKey)const;
	uint GetSaveMapValue(Object* Key)const;
	Object* GetVSTypeMapValue(Object* pKey)const;
	virtual Object* GetObjectByRtti(const Rtti& Rtti);
	bool GetObjectArrayByRtti(const Rtti& Rtti, Vector<Object*>& ObjectArray, bool IsDerivedFrom = false);
	bool GetAllResourceObject(Vector<Object*>& ObjectArray);
	void GetObjectFailed();
	uint GetVersion() const { return m_uiVersion; }

	static uint GetCurVersion() { return ms_uiCurVersion; }
	
	template<class T>
	bool ReadObjectGUID(Pointer<T>& pointer);

	template<class T>
	bool WriteObjectGUID(const Pointer<T>& pointer);

	template<class T>
	bool LinkObjectPtr(Pointer<T>& pointer);

	template<class T>
	bool ReadObjectGUID(T*& pObject);

	template<class T>
	bool WriteObjectGUID(T* const& pObject);

	template<class T>
	bool LinkObjectPtr(T*& pObject);
protected:
	unsigned char* m_pcCurBufPtr;
	unsigned char* m_pcBuffer;
	Vector<unsigned char> m_saveBuf;
	uint m_uiBufferSize;
	Vector<Object*> m_pVObjectArray;
	Vector<Object*> m_pPostLoadObject;

	HashMap<uint, Object*> m_pmLoadMap;
	HashMap<Object*, uint> m_pmSaveMap;
	HashMap<Object*, Object*> m_pmVSTypeLoadMap;

	static uint ms_uiCurVersion;
	uint m_uiVersion;
};


template<class T>
bool Stream::ReadObjectGUID(Pointer<T>& Pointer)
{
	uint uiGUID = 0;
	if (!Read(&uiGUID, sizeof(uint)))
		return 0;
	USIZE_TYPE Temp = GUID32ToSizeType(uiGUID);
	Pointer.SetObject((T*)(Temp));
	return 1;
}

template<class T>
bool Stream::WriteObjectGUID(const Pointer<T>& Pointer)
{
	T* pP = Pointer;
	uint uiGUID = GetSaveMapValue(pP);
	if (!Write(&uiGUID, sizeof(uint)))
		return 0;
	return 1;
}

template<class T>
bool Stream::LinkObjectPtr(Pointer<T>& Pointer)
{
	uint uiGUID = SizeTypeToGUID32((USIZE_TYPE)Pointer.GetEObject());

	Pointer.SetObject(nullptr);

	Pointer = (T*)GetLoadMapValue(uiGUID);
	return 1;
}

template<class T>
bool Stream::ReadObjectGUID(T*& pObject)
{
	uint uiGUID = 0;
	if (!Read(&uiGUID, sizeof(uint)))
		return 0;
	USIZE_TYPE Temp = GUID32ToSizeType(uiGUID);
	pObject = (T*)(Temp);
	return 1;
}

template<class T>
bool Stream::WriteObjectGUID(T* const& pObject)
{
	uint uiGUID = GetSaveMapValue(pObject);
	if (!Write(&uiGUID, sizeof(uint)))
		return 0;
	return 1;
}

template<class T>
bool Stream::LinkObjectPtr(T*& pObject)
{
	uint uiGUID = SizeTypeToGUID32((USIZE_TYPE)pObject);
	pObject = (T*)GetLoadMapValue(uiGUID);
	return 1;
}
	
NS_JYE_END
