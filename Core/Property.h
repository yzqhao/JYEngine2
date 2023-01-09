
#pragma once

#include "Core.h"
#include "Type.h"
#include "Stream.h"

NS_JYE_BEGIN

class Object;
class Rtti;
class Function;

template <class T> inline bool operator < (const T& v1, const T& v2) { return false; }
template <class T> inline bool operator > (const T& v1, const T& v2) { return false; }

template<typename T>
void Copy(T& Dest, T& Src, HashMap<Object*, Object*>& CloneMap)
{
	if (TIsEObjectPointerType<T>::Value)
	{
		Object*& TempSrc = *(Object**)(void*)&Src;
		Object*& TempDest = *(Object**)(void*)&Dest;
		TempDest = Object::_CloneCreateObject(TempSrc, CloneMap);
	}
	else if (TIsEObjectType<T>::Value)
	{
		Object* TempSrc = (Object*)&Src;
		Object* TempDest = (Object*)&Dest;
		Object::_CloneObject(TempSrc, TempDest, CloneMap);
	}
	else if (TIsEObjectSmartPointerType<T>::Value)
	{
		Pointer<Object>& TempSrc = *(Pointer<Object>*)(void*)(&Src);
		Pointer<Object>& TempDest = *(Pointer<Object>*)(void*)(&Dest);
		TempDest = Object::_CloneCreateObject(TempSrc, CloneMap);
	}
	else
	{
		Dest = Src;
	}
}

template<typename T>
void Copy(Vector<T>& Dest, Vector<T>& Src, HashMap<Object*, Object*>& CloneMap)
{
	Dest.clear();
	Dest.resize(Src.size());
	for (uint i = 0; i < Src.size(); i++)
	{
		Copy(Dest[i], Src[i], CloneMap);
	}
}

template<class Key, class Value>
void Copy(HashMap<Key, Value>& Dest, HashMap<Key, Value>& Src, HashMap<Object*, Object*>& CloneMap)
{
	Dest.clear();
	for (auto& it : Src)
	{
		Key* sk = (Key*)std::addressof(it.first);
		Value* sv = (Value*)std::addressof(it.second);
		Key dk;
		Value dv ;
		Copy(dk, *sk, CloneMap);
		Copy(dv, *sv, CloneMap);
		Dest.insert({dk, dv});
	}
}

class CORE_API Property
{
public:
	typedef void (*GetPropertyFunction)(Object* self, void* value);
	typedef void (*SetPropertyFunction)(Object* self, void* value);
public:
	enum	//Property Type
	{
		PT_VALUE,
		PT_ENUM,
		PT_DATA,
		PT_ARRAY,
		PT_MAP,
		PT_MAX
	};
	enum	// Flag 
	{
		F_NONE = 0X00,
		F_SAVE_LOAD = 0X01,	// 序列化与反序列化
		F_CLONE = 0X02,		// 克隆 深拷贝
		F_COPY = 0X04,		// 复制 浅拷贝
		F_SAVE_LOAD_CLONE = 0X03,
		F_SAVE_LOAD_COPY = 0X05,
		F_REFLECT_NAME = 0X08,	// 反射
		F_NO_USE_GC = 0X10,		// GC
		F_MAX
	};

	Property() { }
	Property(Rtti& Owner, const String& Name, uint uiElementOffset, uint uiFlag)
		: m_pRttiOwner(&Owner), m_Name(Name), m_uiElementOffset(uiElementOffset), m_uiFlag(uiFlag), m_pGet(nullptr), m_pSet(nullptr)
	{
	}
	Property(Function& Owner, const String& Name, uint uiElementOffset, uint uiFlag)
		:m_pFunctionOwner(&Owner), m_Name(Name), m_uiElementOffset(uiElementOffset), m_uiFlag(uiFlag), m_pGet(nullptr), m_pSet(nullptr)
	{
	}
	virtual ~Property() { }

	FORCEINLINE Rtti* GetRtti() const { return m_pRttiOwner; }
	FORCEINLINE Function* GetFunction() const { return m_pFunctionOwner; }
	FORCEINLINE const String& GetName() const { return m_Name; }
	FORCEINLINE void SetOwner(Rtti& Owner) { m_pRttiOwner = &Owner; }
	FORCEINLINE void SetOwner(Function& Function) { m_pFunctionOwner = &Function; }
	FORCEINLINE uint GetFlag() const { return m_uiFlag; }
	FORCEINLINE void SetFlag(uint uiFlag) { m_uiFlag = uiFlag; }

	virtual void* GetValueAddress(void* pObj) const
	{
		return (void*)(((unsigned char*)pObj) + m_uiElementOffset);
	}

	virtual bool Archive(Stream& stream, void* pObj) = 0;

	virtual bool Clone(Property* p)
	{
		if ((p->GetRtti() == GetRtti() && GetRtti()) || (p->GetFunction() == GetFunction() && GetFunction()))
		{
			m_Name = p->m_Name;
			m_uiFlag = p->m_uiFlag;
			m_uiElementOffset = p->m_uiElementOffset;
			return true;
		}

		return false;
	}

	virtual uint GetPropertyType() const = 0;
	virtual Property* GetInstance() = 0;
	virtual void CloneData(void* pSrcObj, void* pDestObj, HashMap<Object*, Object*>& CloneMap) = 0;
	virtual void CopyData(void* pSrcObj, void* pDestObj) = 0;

	void Get(Object* cla, void* res)
	{
		m_pGet(cla, res);
	}
	void Set(Object* cla, void* member)
	{
		m_pSet(cla, member);
	}
	GetPropertyFunction GetGetFunction() { return m_pGet; }
	SetPropertyFunction GetSetFunction() { return m_pSet; }

	template<typename GF, typename SF>
	void Member(GF get, SF set)
	{
		m_pGet = reinterpret_cast<GetPropertyFunction>(get);
		m_pSet = reinterpret_cast<SetPropertyFunction>(set);
	}

protected:
	Rtti* m_pRttiOwner;
	Function* m_pFunctionOwner;
	String m_Name;
	uint m_uiFlag;
	uint m_uiElementOffset;

	GetPropertyFunction m_pGet;
	SetPropertyFunction m_pSet;
};

template<typename T>
class EnumProperty : public Property
{
public:
	EnumProperty()
	{
		JY_ASSERT(sizeof(T) == 4);
	}
	EnumProperty(Rtti& Owner, const String& Name, const String& EnumName, uint uiElementOffset, uint uiFlag)
		: Property(Owner, Name, uiElementOffset, uiFlag)
	{
		JY_ASSERT(sizeof(T) == 4);
		m_EnumName = EnumName;
	}
	EnumProperty(Function& Owner, const String& Name, const String& EnumName, uint uiElementOffset, uint uiFlag)
		: Property(Owner, Name, uiElementOffset, uiFlag)
	{
		JY_ASSERT(sizeof(T) == 4);
		m_EnumName = EnumName;
	}
	virtual ~EnumProperty()
	{

	}
	virtual bool Clone(Property* p)
	{
		EnumProperty* Temp = (EnumProperty*)p;
		if (!Property::Clone(Temp))
			return false;
		m_EnumName = Temp->m_EnumName;
		return true;
	}
	virtual bool GetValue(void* pObj, uint& pDataDest) const
	{
		pDataDest = *(uint*)(((unsigned char*)pObj) + m_uiElementOffset);
		return true;
	}
	virtual bool GetValue(const void* pObj, uint& pDataDest) const
	{
		pDataDest = *(const uint*)(((const char*)pObj) + m_uiElementOffset);
		return true;
	}
	virtual uint& Value(void* pObj) const
	{
		return *(uint*)(((const char*)pObj) + m_uiElementOffset);
	}
	String& GetEnumName()
	{
		return m_EnumName;
	}

	virtual uint GetPropertyType() const
	{
		return PT_ENUM;
	}
	virtual Property* GetInstance()
	{
		return _NEW EnumProperty();
	}
	virtual void CloneData(void* pSrcObj, void* pDestObj, HashMap<Object*, Object*>& CloneMap)
	{
		Value(pDestObj) = Value(pSrcObj);
	}
	virtual void CopyData(void* pSrcObj, void* pDestObj)
	{
		Value(pDestObj) = Value(pSrcObj);
	}
	virtual bool Archive(Stream& stream,void* pObj)
	{
		stream.Archive(Value(pObj));
		return true;
	}
protected:
	String m_EnumName;
}; 

template<typename T, typename NumType>
class DataProperty : public Property
{
public:
	DataProperty(Rtti& Owner, const String& Name, uint uiElementOffset, uint uiDataNum, bool bDynamicCreate)
		:Property(Owner, Name, uiElementOffset, F_SAVE_LOAD_CLONE | F_NO_USE_GC)
	{
		m_uiDataNum = uiDataNum;
		m_bDynamicCreate = bDynamicCreate;
	}
	DataProperty(Function& Owner, const String& Name, uint uiElementOffset, uint uiDataNum, bool bDynamicCreate)
		:Property(Owner, Name, uiElementOffset, F_SAVE_LOAD_CLONE | F_NO_USE_GC)
	{
		m_uiDataNum = uiDataNum;
		m_bDynamicCreate = bDynamicCreate;
	}
	DataProperty()
	{

	}
	DataProperty(Rtti& Owner, const String& Name, uint uiElementOffset, uint uiNumElementOffset)
		:Property(Owner, Name, uiElementOffset, F_SAVE_LOAD_CLONE | F_NO_USE_GC)
	{
		m_uiDataNum = 0;
		m_bDynamicCreate = true;
		m_uiNumElementOffset = uiNumElementOffset;
	}
	virtual uint GetPropertyType() const { return PT_DATA; }
	virtual void CopyData(void* pSrcObj, void* pDestObj) { JY_ASSERT(0); }
	virtual void CloneData(void* pSrcObj, void* pDestObj, HashMap<Object*, Object*>& CloneMap)
	{
		T* SrcValueAddres = *(T**)GetValueAddress(pSrcObj);

		if (m_uiDataNum > 0)
		{
			T** Temp = (T**)GetValueAddress(pDestObj);
			if (m_bDynamicCreate)
			{
				*Temp = _NEW T[m_uiDataNum];
				memcpy((void*)(*Temp), (void*)SrcValueAddres, m_uiDataNum * sizeof(T));
			}
			else
			{
				memcpy((void*)(*Temp), (void*)SrcValueAddres, m_uiDataNum * sizeof(T));
			}
		}
		else
		{
			T** Temp = (T**)GetValueAddress(pDestObj);

			void* SrcNumOffset = (void*)(((unsigned char*)pSrcObj) + m_uiNumElementOffset);
			void* DestNumOffset = (void*)(((unsigned char*)pDestObj) + m_uiNumElementOffset);
			*(NumType*)DestNumOffset = *(NumType*)SrcNumOffset;
			NumType uiNum = *(NumType*)SrcNumOffset;

			*Temp = _NEW T[uiNum];
			memcpy((void*)(*Temp), (void*)SrcValueAddres, uiNum * sizeof(T));
		}
	}
	virtual Property* GetInstance()
	{
		return _NEW DataProperty<T, NumType>();
	}
	virtual bool Clone(Property* p)
	{
		DataProperty<T, NumType>* Temp = (DataProperty<T, NumType> *)p;
		if (!Property::Clone(Temp))
			return false;
		m_bDynamicCreate = Temp->m_bDynamicCreate;
		m_uiDataNum = Temp->m_uiDataNum;
		m_uiNumElementOffset = Temp->m_uiNumElementOffset;
		return true;
	}
	virtual bool Archive(Stream& stream, void* pObj)
	{
		uint uiStreamFlag = stream.GetStreamFlag();
		if (uiStreamFlag == Stream::AT_SAVE)
		{
			T* ValueAddres = *(T**)GetValueAddress(pObj);

			if (m_uiDataNum > 0)
			{
				stream.Write(ValueAddres, m_uiDataNum * sizeof(T));
			}
			else
			{
				void * NumOffset = (void*)(((unsigned char*)pObj) + m_uiNumElementOffset);
				NumType uiNum = *(NumType*)NumOffset;
				stream.Write(NumOffset,sizeof(NumType));
				stream.Write(ValueAddres, uiNum * sizeof(T));
			}
		}
		else if (uiStreamFlag == Stream::AT_LOAD)
		{
			T** Temp = (T**)GetValueAddress(pObj);
			if (m_uiDataNum > 0)
			{
				if (m_bDynamicCreate)
				{
					*Temp = _NEW T[m_uiDataNum];
					stream.Read((void *)(*Temp),m_uiDataNum * sizeof(T));
				}
				else
				{
					stream.Read((void *)(*Temp),m_uiDataNum * sizeof(T));
				}
			}
			else
			{
				void * NumOffset = (void*)(((unsigned char*)pObj) + m_uiNumElementOffset);
				stream.Read(NumOffset,sizeof(NumType));
				NumType uiNum = *(NumType*)NumOffset;
				if (uiNum)
				{
					*Temp = _NEW T[uiNum];
					stream.Read((void *)(*Temp), uiNum * sizeof(T));
				}
			}
		}
		else if (uiStreamFlag == Stream::AT_SIZE)
		{
			if (m_uiDataNum > 0)
			{
				stream.AddBufferSize(m_uiDataNum * sizeof(T));
			}
			else
			{
				void * NumOffset = (void*)(((unsigned char*)pObj) + m_uiNumElementOffset);
				NumType uiNum = *(NumType*)NumOffset;
				stream.AddBufferSize(sizeof(NumType));
				stream.AddBufferSize(uiNum * sizeof(T));
			}
		}
		return true;
	}
protected:
	bool m_bDynamicCreate;
	uint m_uiDataNum;
	uint m_uiNumElementOffset;
};

template<typename T>
class ValueBaseProperty : public Property
{
public:
	ValueBaseProperty()
	{

	}
	ValueBaseProperty(Rtti& Owner, const String& Name, uint uiElementOffset, uint uiFlag, bool Range = false, T HighValue = T(), T LowValue = T(), T fStep = T())
		:Property(Owner, Name, uiElementOffset, uiFlag)
	{
		m_LowValue = LowValue;
		m_HightValue = HighValue;
		m_fStep = fStep;
		m_bRange = Range;
	}
	ValueBaseProperty(Function& Owner, const String& Name, uint uiElementOffset, uint uiFlag, bool Range = false, T HighValue = T(), T LowValue = T(), T fStep = T())
		:Property(Owner, Name, uiElementOffset, uiFlag)
	{
		m_LowValue = LowValue;
		m_HightValue = HighValue;
		m_fStep = fStep;
		m_bRange = Range;
	}
	virtual ~ValueBaseProperty()
	{

	}
	virtual bool Clone(Property* p)
	{
		ValueBaseProperty<T>* Temp = (ValueBaseProperty<T> *)p;
		if (!Property::Clone(Temp))
			return false;
		m_LowValue = Temp->m_LowValue;
		m_HightValue = Temp->m_HightValue;
		return true;
	}
protected:
	T m_LowValue;
	T m_HightValue;
	T m_fStep;
	bool m_bRange;
};

template<typename T>
class ValueProperty : public ValueBaseProperty<T>
{
public:
	ValueProperty()
	{

	}
	ValueProperty(Rtti& Owner, const String& Name, uint uiElementOffset, uint uiFlag, bool Range = false, T HighValue = T(), T LowValue = T(), T fStep = T())
		:ValueBaseProperty<T>(Owner, Name, uiElementOffset, uiFlag, Range, HighValue, LowValue, fStep)
	{

	}
	ValueProperty(Function& Owner, const String& Name, uint uiElementOffset, uint uiFlag, bool Range = false, T HighValue = T(), T LowValue = T(), T fStep = T())
		: ValueBaseProperty<T>(Owner, Name, uiElementOffset, uiFlag, Range, HighValue, LowValue, fStep)
	{

	}
	virtual ~ValueProperty()
	{
		
	}
	virtual uint GetPropertyType() const
	{
		return Property::PT_VALUE;
	}

	virtual bool GetValue(void* pObj, T& pDataDest) const
	{
		pDataDest = *(T*)(((unsigned char*)pObj) + ValueBaseProperty<T>::m_uiElementOffset);
		return true;
	}

	virtual bool GetValue(const void* pObj, T& pDataDest) const
	{
		pDataDest = *(const T*)(((const char*)pObj) + ValueBaseProperty<T>::m_uiElementOffset);
		return true;
	}

	virtual T& Value(void* pObj) const
	{
		return *(T*)(((const char*)pObj) + ValueBaseProperty<T>::m_uiElementOffset);
	}

	virtual void CloneData(void* pSrcObj, void* pDestObj, HashMap<Object*, Object*>& CloneMap)
	{
		Copy(Value(pDestObj), Value(pSrcObj), CloneMap);
	}
	virtual void CopyData(void* pSrcObj, void* pDestObj)
	{
		Value(pDestObj) = Value(pSrcObj);
	}
	virtual Property* GetInstance()
	{
		return _NEW ValueProperty<T>();
	}

	virtual bool Archive(Stream& stream, void* pObj)
	{
		stream.Archive(Value(pObj));
		return true;
	}	
};

template<typename ArrayType, typename T>
class ArrayProperty : public ValueBaseProperty<T>
{
public:
	ArrayProperty()
	{

	}
	ArrayProperty(Rtti& Owner, const String& Name, uint uiElementOffset, uint uiFlag, bool Range = false, T HighValue = T(), T LowValue = T(), T fStep = T())
		:ValueBaseProperty<T>(Owner, Name, uiElementOffset, uiFlag, Range, HighValue, LowValue, fStep)
	{

	}
	ArrayProperty(Function& Owner, const String& Name, uint uiElementOffset, uint uiFlag, bool Range = false, T HighValue = T(), T LowValue = T(), T fStep = T())
		:ValueBaseProperty<T>(Owner, Name, uiElementOffset, uiFlag, Range, HighValue, LowValue, fStep)
	{

	}
	virtual ~ArrayProperty()
	{

	}
	virtual uint GetPropertyType() const
	{
		return Property::PT_ARRAY;
	}
	inline ArrayType& GetContainer(void* pObj) const
	{
		return (*(ArrayType*)(((unsigned char*)pObj) + ValueBaseProperty<T>::m_uiElementOffset));
	}
	inline bool push_back(void* pObj, uint uiIndex, T& pDataSrc)
	{
		GetContainer(pObj).push_back(pDataSrc);
		return true;
	}
	inline bool Erase(void* pObj, uint i)
	{
		GetContainer(pObj).Erase(i);
	}
	virtual bool GetValue(void* pObj, uint uiIndex, T& pDataDest) const
	{
		pDataDest = (GetContainer(pObj)[uiIndex]);
		return true;
	}
	virtual void CloneData(void* pSrcObj, void* pDestObj, HashMap<Object*, Object*>& CloneMap)
	{
		Copy(GetContainer(pDestObj), GetContainer(pSrcObj), CloneMap);
	}
	virtual void CopyData(void* pSrcObj, void* pDestObj)
	{
		GetContainer(pDestObj) = GetContainer(pSrcObj);
	}
	virtual Property* GetInstance()
	{
		return _NEW ArrayProperty<ArrayType, T>();
	}
	virtual bool Archive(Stream& stream,void* pObj)
	{
		stream.Archive(GetContainer(pObj));
		return true;
	}
};

template<typename MapType, typename KEY, typename VALUE>
class MapProperty : public ValueBaseProperty<VALUE>
{
public:
	MapProperty(Rtti& Owner, const String& Name, uint uiElementOffset, uint uiFlag, bool Range = false, VALUE HighValue = VALUE(), VALUE LowValue = VALUE(), VALUE fStep = VALUE())
		:ValueBaseProperty<VALUE>(Owner, Name, uiElementOffset, uiFlag, Range, HighValue, LowValue, fStep)
	{
	}
	MapProperty(Function& Owner, const String& Name, uint uiElementOffset, uint uiFlag, bool Range = false, VALUE HighValue = VALUE(), VALUE LowValue = VALUE(), VALUE fStep = VALUE())
		:ValueBaseProperty<VALUE>(Owner, Name, uiElementOffset, uiFlag, Range, HighValue, LowValue, fStep)
	{

	}
	MapProperty()
	{
	}
	virtual ~MapProperty()
	{
	}
	virtual uint GetPropertyType() const
	{
		return Property::PT_MAP;
	}
	inline MapType& GetContainer(void* pObj) const
	{
		return (*(MapType*)(((unsigned char*)pObj) + ValueBaseProperty<VALUE>::m_uiElementOffset));
	}
	inline bool push_back(void* pObj, uint uiIndex, std::pair<KEY, VALUE>& pDataSrc)
	{
		GetContainer(pObj).insert(pDataSrc);
		return true;
	}
	inline bool Erase(void* pObj, uint i)
	{
		GetContainer(pObj).Erase(i);
	}
	virtual bool GetValue(void* pObj, const KEY& key, VALUE& pDataDest) const
	{
		pDataDest = (GetContainer(pObj)[key]);
		return true;
	}
	virtual void CloneData(void* pSrcObj, void* pDestObj, HashMap<Object*, Object*>& CloneMap)
	{
		Copy(GetContainer(pDestObj), GetContainer(pSrcObj), CloneMap);
	}
	virtual void CopyData(void* pSrcObj, void* pDestObj)
	{
		GetContainer(pDestObj) = GetContainer(pSrcObj);
	}
	virtual Property* GetInstance()
	{
		return _NEW MapProperty<MapType, KEY, VALUE>();
	}
	virtual bool Archive(Stream& Stream, void* pObj)
	{
		Stream.Archive(GetContainer(pObj));
		return true;
	}
};

template<class T, class NumType>
struct DataPropertyCreator
{
	Property* CreateProperty(const String& Name, Rtti& Owner, uint Offset, uint NumOffset)
	{
		return _NEW DataProperty<T, NumType>(Owner, Name, Offset, NumOffset);
	}
	Property* CreateFunctionProperty(const String& Name, Function& Owner, uint Offset, uint NumOffset)
	{
		return _NEW DataProperty<T, NumType>(Owner, Name, Offset, NumOffset);
	}
	Property* CreateProperty(const String& Name, Rtti& Owner, uint Offset, uint uiDataNum, bool bDynamicCreate)
	{
		return _NEW DataProperty<T, NumType>(Owner, Name, Offset, uiDataNum, bDynamicCreate);
	}
};

template<class T>
struct AutoPropertyCreator
{
	Property* CreateProperty(const String& Name, Rtti& Owner, uint Offset, uint uiFlag)
	{
		if (!(TIsEObjectPointerType<T>::Value || TIsEObjectSmartPointerType<T>::Value || TIsCustomType<T>::Value || TIsEObjectType<T>::Value))
		{
			uiFlag |= Property::F_NO_USE_GC;
		}
		return _NEW ValueProperty<T>(Owner, Name, Offset, uiFlag);
	}
	Property* CreateFunctionProperty(const String& Name, Function& Owner, uint Offset, uint uiFlag)
	{
		return _NEW ValueProperty<T>(Owner, Name, Offset, uiFlag);
	}
	Property* CreateProperty(const String& Name, Rtti& Owner, uint Offset, T HighValue, T LowValue, float fStep, uint uiFlag)
	{
		if (!(TIsEObjectPointerType<T>::Value || TIsEObjectSmartPointerType<T>::Value || TIsCustomType<T>::Value || TIsEObjectType<T>::Value))
		{
			uiFlag |= Property::F_NO_USE_GC;
		}
		return _NEW ValueProperty<T>(Owner, Name, Offset, uiFlag, true, HighValue, LowValue, fStep);
	}
};

template<class T>
struct AutoPropertyCreator<Vector<T>>
{
	Property* CreateProperty(const String& Name, Rtti& Owner, uint Offset, uint uiFlag)
	{
		if (!(TIsEObjectPointerType<T>::Value || TIsEObjectSmartPointerType<T>::Value || TIsCustomType<T>::Value || TIsEObjectType<T>::Value || ValueBase<T>::NeedsConstructor))
		{
			uiFlag |= Property::F_NO_USE_GC;
		}
		return _NEW ArrayProperty<Vector<T>, T>(Owner, Name, Offset, uiFlag);
	}
	Property* CreateFunctionProperty(const String& Name, Function& Owner, uint Offset, uint uiFlag)
	{
		return _NEW ArrayProperty<Vector<T>, T>(Owner, Name, Offset, uiFlag);
	}
	Property* CreateProperty(const String& Name, Rtti& Owner, uint Offset, T HighValue, T LowValue, T fStep, uint uiFlag)
	{
		if (!(TIsEObjectPointerType<T>::Value || TIsEObjectSmartPointerType<T>::Value || TIsCustomType<T>::Value || TIsEObjectType<T>::Value || ValueBase<T>::NeedsConstructor))
		{
			uiFlag |= Property::F_NO_USE_GC;
		}
		return _NEW ArrayProperty<Vector<T>, T>(Owner, Name, Offset, uiFlag, HighValue, LowValue, fStep);
	}
};

template<class KEY, class VALUE>
struct AutoPropertyCreator<HashMap<KEY, VALUE>>
{
	Property* CreateProperty(const String& Name, Rtti& Owner, uint Offset, uint uiFlag)
	{
		return _NEW MapProperty<HashMap<KEY, VALUE>, KEY, VALUE>(Owner, Name, Offset, uiFlag);
	}
	Property* CreateFunctionProperty(const String& Name, Function& Owner, uint Offset, uint uiFlag)
	{
		return _NEW MapProperty<HashMap<KEY, VALUE>, KEY, VALUE>(Owner, Name, Offset, uiFlag);
	}
	Property* CreateProperty(const String& Name, Rtti& Owner, uint Offset, VALUE HighValue, VALUE LowValue, float fStep, uint uiFlag)
	{
		return _NEW MapProperty<HashMap<KEY, VALUE>, KEY, VALUE>(Owner, Name, Offset, uiFlag, HighValue, LowValue, fStep);
	}
};

template<class KEY, class VALUE>
struct AutoPropertyCreator<Map<KEY, VALUE>>
{
	Property* CreateProperty(const String& Name, Rtti& Owner, uint Offset, uint uiFlag)
	{
		return _NEW MapProperty<Map<KEY, VALUE>, KEY, VALUE>(Owner, Name, Offset, uiFlag);
	}
	Property* CreateFunctionProperty(const String& Name, Function& Owner, uint Offset, uint uiFlag)
	{
		return _NEW MapProperty<Map<KEY, VALUE>, KEY, VALUE>(Owner, Name, Offset, uiFlag);
	}
	Property* CreateProperty(const String& Name, Rtti& Owner, uint Offset, VALUE HighValue, VALUE LowValue, float fStep, uint uiFlag)
	{
		return _NEW MapProperty<Map<KEY, VALUE>, KEY, VALUE>(Owner, Name, Offset, uiFlag, HighValue, LowValue, fStep);
	}
};

class PropertyCreator
{
public:
	template<class ValueType>
	static AutoPropertyCreator<ValueType>& GetAutoPropertyCreator(ValueType& valueTypeDummyRef)
	{
		static AutoPropertyCreator<ValueType> apc;
		return apc;
	}
	template<class ValueType, class NumType>
	static DataPropertyCreator<ValueType, NumType>& GetAutoPropertyCreator(ValueType*& valueTypeDummyRef, NumType& valueNumTypeDummyRef)
	{
		static DataPropertyCreator<ValueType, NumType> apc;
		return apc;
	}
	template<class ValueType>
	static Property* CreateEnumProperty(ValueType& valueTypeDummyRef, const String& Name, const String& EnumName, Rtti& Owner, uint Offset, uint uiFlag)
	{
		return _NEW EnumProperty<ValueType>(Owner, Name, EnumName, Offset, uiFlag | Property::F_NO_USE_GC);
	}
};

class Function
{
	using FunctionTemplatePtr = void(*)(Object* p, Function* pFun, void* para, void* ret);
public:
	enum	// Flag 
	{
		F_DEFAULT = 0X00,
		F_CLIENT = 0X01,
		F_SERVER = 0X02,
		F_REFLECT_UI = 0X04,
		F_MAX
	};
	Function()
	{
		m_pReturnProperty = nullptr;
		ObjectFun = nullptr;
	}
	~Function()
	{
		for (uint i = 0; i < m_PropertyArray.size(); i++)
		{
			if (m_PropertyArray[i])
			{
				SAFE_DELETE(m_PropertyArray[i])
			}
		}
		SAFE_DELETE(m_pReturnProperty);
	}
	Function(Rtti& Owner, const String& Name, uint uiFlag)
		:m_pOwner(&Owner)
	{
		m_Name = Name;
		m_uiFlag = uiFlag;
		m_pReturnProperty = nullptr;
		ObjectFun = nullptr;
	}
	inline Rtti* GetRtti()const
	{
		return m_pOwner;
	}
	inline const String& GetName()const
	{
		return m_Name;
	}
	virtual bool Clone(Function* f)
	{
		if ((f->GetRtti() == GetRtti() && GetRtti()))
		{
			m_Name = f->m_Name;
			m_uiFlag = f->m_uiFlag;
			for (uint i = 0; i < m_PropertyArray.size(); i++)
			{
				Property* p = f->m_PropertyArray[i]->GetInstance();
				m_PropertyArray.push_back(p);
				p->Clone(f->m_PropertyArray[i]);
			}
			return true;
		}
		return false;

	}
	virtual Function* GetInstance()
	{
		return _NEW Function();
	}
	void SetOwner(Rtti& Owner)
	{
		m_pOwner = &Owner;
	}
	uint GetFlag()const
	{
		return m_uiFlag;
	}
	void AddProperty(Property* P)
	{
		m_PropertyArray.push_back(P);
	}
	Property* GetProperty(uint i)
	{
		return m_PropertyArray[i];
	}
	uint GetPropertyNum()const
	{
		return m_PropertyArray.size();
	}
	void SetReturnType(Property* p)
	{
		m_pReturnProperty = p;
	}
	bool IsReturnVoid()
	{
		return !m_pReturnProperty;
	}
	void SetTotalSize(uint uiTotalSize)
	{
		m_uiTotalSize = uiTotalSize;
	}
	bool IsSame(Function* p)
	{
		if (m_pOwner != p->m_pOwner)
		{
			return false;
		}
		else if (m_Name != p->m_Name)
		{
			return false;
		}
		else if (m_uiTotalSize != p->m_uiTotalSize)
		{
			return false;
		}
		else if (m_PropertyArray.size() != p->m_PropertyArray.size())
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	FunctionTemplatePtr ObjectFun;
protected:
	Rtti* m_pOwner;
	String m_Name;
	uint m_uiFlag;
	Vector<Property*> m_PropertyArray;
	Property* m_pReturnProperty;
	uint m_uiTotalSize;
};

class MemberRegister
{
public:
	typedef void (*GetFunction)(Object* self, Vector<Object*>& res);
	typedef void (*SetFunction)(Object* self, Vector<Object*>& value);
public:
	MemberRegister() : m_rpRtti(nullptr) {}
	~MemberRegister() {}

	void Get(Object* cla, Vector<Object*>& res)
	{  
		m_pGet(cla, res);
	}
	void Set(Object* cla, Vector<Object*> member)
	{
		m_pSet(cla, member);
	}
	GetFunction GetGetFunction() { return m_pGet; }
	SetFunction GetSetFunction() { return m_pSet; }
	FORCEINLINE const String& GetName() const { return m_MemberName; }

	template<typename OBJ, typename GF, typename SF>
	void Member(GF get, SF set, const std::string& name)
	{
		m_MemberName = name;
		m_rpRtti = &OBJ::RTTI();
		m_pGet = reinterpret_cast<GetFunction>(get);
		m_pSet = reinterpret_cast<SetFunction>(set);
	}
private:
	std::string						m_MemberName;
	GetFunction						m_pGet;
	SetFunction						m_pSet;
	Rtti* m_rpRtti;
};

#include "PropertyMacro.h"

NS_JYE_END
