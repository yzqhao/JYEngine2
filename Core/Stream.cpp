
#include "Object.h"
#include "Property.h"
#include "Interface/IFile.h"
#include "Interface/IFileSystem.h"
#include "StringHash.h"

#include <functional>

NS_JYE_BEGIN

uint Stream::ms_uiCurVersion = 0;

uint Stream::GetStrDistUse(const String &Str)
{
	return sizeof(int) + (uint)Str.length() * sizeof(char);

}

uint Stream::GetStrDistUse(const char * pCh)
{
	return sizeof(int) + (uint)strlen(pCh) * sizeof(char);
}

Stream::Stream()
{
	m_pVObjectArray.clear();
	m_pmLoadMap.clear();
	m_pmVSTypeLoadMap.clear();
	m_pcCurBufPtr	= nullptr;
	m_pcBuffer		= nullptr;
	m_uiBufferSize	= 0;
	m_bLoadUseGC = false;
}

Stream::~Stream()
{
	m_pVObjectArray.clear();
	m_pcCurBufPtr	= nullptr;
}

bool Stream::Read(void * pvBuffer,uint uiSize)
{
	if(!pvBuffer || !m_pcBuffer)
		return false;

	if(uint(m_pcCurBufPtr + uiSize - m_pcBuffer) > m_uiBufferSize)
		return false;

	memcpy(pvBuffer,m_pcCurBufPtr,uiSize);

	m_pcCurBufPtr +=uiSize;
	return true;
}

bool Stream::Write(const void * pvBuffer,uint uiSize)
{
	if(!pvBuffer || !m_pcBuffer)
		return false;

	if(uint(m_pcCurBufPtr + uiSize - m_pcBuffer) > m_uiBufferSize)
		return false;

	memcpy(m_pcCurBufPtr,pvBuffer,uiSize);

	m_pcCurBufPtr +=uiSize;
	return true;
}

bool Stream::ReadString(String& str)
{
	uint uiBufferSize = 0;
	uint uiStrLen = 0;
	if(!Read(&uiBufferSize,sizeof(uint)))
		return false;
	if (!uiBufferSize)
	{
		return true;
	}
	char * pCh = 0;
	pCh= _NEW char[uiBufferSize];
	if(!Read(pCh,uiBufferSize))
	{
		SAFE_DELETE_ARRAY(pCh);
		return false;
	}
	uiStrLen = uiBufferSize;
	str.resize(uiStrLen);
	memcpy(const_cast<char*>(str.data()), pCh, uiBufferSize);

	SAFE_DELETE_ARRAY(pCh);

	return true;
}

bool Stream::WriteString(const String & String)
{
	uint uiBufferLen = String.length() * sizeof(char);
	if(!Write(&uiBufferLen,sizeof(uint)))
		return false;
	if(!Write(String.data(),uiBufferLen))
		return false;
	return true;
}

bool Stream::RegisterReachableObject(Object * pObject)
{
	if (pObject->IsHasFlag(Object::OF_REACH))
	{
		return false;
	}
	pObject->SetFlag(Object::OF_REACH);
	pObject->ClearFlag(Object::OF_UNREACH);

	return true;
}

bool Stream::RegisterPostLoadObject(Object * pObject)
{
	for(uint i = 0 ; i < (uint)m_pPostLoadObject.size(); i++)
	{
		if(m_pPostLoadObject[i] == pObject)
		{
			return false;
		}
	}
	m_pPostLoadObject.push_back((Object *)pObject);	
	return true;
}

bool Stream::RegisterObject(Object * pObject)
{
	JY_ASSERT(pObject);
	if(!pObject)
		return false;

	for(uint i = 0 ; i < (uint)m_pVObjectArray.size(); i++)
	{
		if(m_pVObjectArray[i] == pObject)
		{
			return false;
		}
	}
	m_pVObjectArray.push_back((Object *)pObject);

	return true;
}

Object *Stream::GetVSTypeMapValue(Object *pKey)const
{
	auto i = m_pmVSTypeLoadMap.find(pKey);
	if(i == m_pmVSTypeLoadMap.end())
		return nullptr;

	return i->second;
}

const Object* Stream::GetLoadMapValue(uint uiKey)const
{
	auto i = m_pmLoadMap.find(uiKey);
	if(i == m_pmLoadMap.end())
		return nullptr;
	
	return i->second;
}

uint Stream::GetSaveMapValue(Object * Key)const
{
	auto i = m_pmSaveMap.find(Key);
	if (i == m_pmSaveMap.end())
		return false;

	return i->second;
}

Object* Stream::GetObjectByRtti(const Rtti &Rtti)
{
	Object * pObject = nullptr;
	for(uint i = 0 ; i < m_pVObjectArray.size() ; i++)
	{
		if((m_pVObjectArray[i]->GetType()).IsSameType(Rtti))
		{
			pObject = m_pVObjectArray[i];
			break;
		}
	}
	if (pObject == nullptr)
	{
		for (uint i = 0; i < m_pVObjectArray.size(); i++)
		{
			if ((m_pVObjectArray[i]->GetType()).IsDerived(Rtti))
			{
				pObject = m_pVObjectArray[i];
				break;
			}
		}
	}
	for (uint i = 0; i < m_pVObjectArray.size(); i++)
	{
		Object * p = m_pVObjectArray[i];
		JY_ASSERT(p != nullptr);
		if (p)
		{
			p->ClearFlag(Object::OF_REACH);
			p->SetFlag(Object::OF_UNREACH);
		}
	}
	if (pObject)
	{
		Stream GCCollectStream;
		GCCollectStream.SetStreamFlag(Stream::AT_LOAD_OBJECT_COLLECT_GC);
		GCCollectStream.ArchiveAll(pObject);

		Vector<Object *> CanGCObject;
		for (uint i = 0; i < m_pVObjectArray.size();)
		{
			Object * p = m_pVObjectArray[i];
			if (p->IsHasFlag(Object::OF_UNREACH))
			{
				CanGCObject.push_back(p);
				m_pVObjectArray.erase(m_pVObjectArray.begin() + i);
			}
			else
			{
				i++;
			}
		}

		//VSResourceManager::AddCanGCObject(CanGCObject);
	}
	else
	{
		GetObjectFailed();
	}
	if (pObject)
	{
		m_pPostLoadObject.clear();
		m_uiStreamFlag = AT_POSTLOAD;
		ArchiveAll(pObject);
		m_pPostLoadObject.clear();
		m_uiStreamFlag = AT_LATER_POSTLOAD;
		ArchiveAll(pObject);
	}
	return pObject;
}

void Stream::GetObjectFailed()
{
	if (m_pVObjectArray.size())
	{
		//VSResourceManager::AddCanGCObject(m_pVObjectArray);
		m_pVObjectArray.clear();
	}
}

bool Stream::GetObjectArrayByRtti(const Rtti &Rtti, Vector<Object*> &ObjectArray, bool IsDerivedFrom)
{
	ObjectArray.clear();
	for(uint i = 0 ; i < m_pVObjectArray.size() ; i++)
	{
		if ((m_pVObjectArray[i]->GetType()).IsSameType(Rtti) || ((m_pVObjectArray[i]->GetType()).IsDerived(Rtti) && IsDerivedFrom))
		{
			ObjectArray.push_back(m_pVObjectArray[i]);
		}
	}
	if (ObjectArray.size() > 0)
	{
		for (uint i = 0; i < m_pVObjectArray.size(); i++)
		{
			Object* p = m_pVObjectArray[i];
			JY_ASSERT(p != nullptr);
			if (p)
			{
				p->ClearFlag(Object::OF_REACH);
				p->SetFlag(Object::OF_UNREACH);
			}
		}

		Stream GCCollectStream;
		GCCollectStream.SetStreamFlag(Stream::AT_LOAD_OBJECT_COLLECT_GC);
		for (uint i = 0; i < ObjectArray.size(); i++)
		{
			GCCollectStream.ArchiveAll(ObjectArray[i]);
		}
		
		Vector<Object *> CanGCObject;
		for (uint i = 0; i < m_pVObjectArray.size();)
		{
			Object * p = m_pVObjectArray[i];
			if (p->IsHasFlag(Object::OF_UNREACH))
			{
				CanGCObject.push_back(p);
				m_pVObjectArray.erase(m_pVObjectArray.begin() + i);
			}
			else
			{
				i++;
			}
		}

		//VSResourceManager::AddCanGCObject(CanGCObject);

		m_uiStreamFlag = AT_POSTLOAD;
		for (uint i = 0; i < ObjectArray.size(); i++)
		{
			ArchiveAll(ObjectArray[i]);
		}

		m_uiStreamFlag = AT_LATER_POSTLOAD;
		for (uint i = 0; i < ObjectArray.size(); i++)
		{
			ArchiveAll(ObjectArray[i]);
		}

		return true;
	}
	else
	{
		GetObjectFailed();
		return false;
	}
}
/*******************************************************************************
									new version
*******************************************************************************/
bool Stream::ArchiveAll(Object * pObject)
{
	if (!pObject)
	{
		return false;
	}
	
	if (m_uiStreamFlag == AT_REGISTER)
	{
		if(RegisterObject(pObject))
		{
			const Rtti& Rtti = pObject->GetType();
			for (uint j = 0 ; j < Rtti.GetPropertyNum() ; j++)
			{
				Property * pProperty = Rtti.GetProperty(j);
				if (pProperty->GetFlag() & Property::F_SAVE_LOAD)
				{
					pProperty->Archive(*this, pObject);
				}
			}
		}
	}
	else if (m_uiStreamFlag == AT_POSTLOAD)
	{
		if (RegisterPostLoadObject(pObject))
		{
			const Rtti& Rtti = pObject->GetType();
			for (uint j = 0 ; j < Rtti.GetPropertyNum() ; j++)
			{
				Property * pProperty = Rtti.GetProperty(j);
				if (pProperty->GetFlag() & Property::F_SAVE_LOAD)
				{
					pProperty->Archive(*this, pObject);
				}
			}
			pObject->PostLoad(this);
		}
	}
	else if (m_uiStreamFlag == AT_LATER_POSTLOAD)
	{
		if (RegisterPostLoadObject(pObject))
		{
			const Rtti& Rtti = pObject->GetType();
			for (uint j = 0; j < Rtti.GetPropertyNum(); j++)
			{
				Property* pProperty = Rtti.GetProperty(j);
				if (pProperty->GetFlag() & Property::F_SAVE_LOAD)
				{
					pProperty->Archive(*this, pObject);
				}
			}
			pObject->PostLateLoad(this);
		}
	}
	else if (m_uiStreamFlag == AT_OBJECT_COLLECT_GC)
	{
		if (RegisterReachableObject(pObject))
		{
			const Rtti& Rtti = pObject->GetType();
			for (uint j = 0; j < Rtti.GetPropertyNum(); j++)
			{
				Property * pProperty = Rtti.GetProperty(j);
				if (!(pProperty->GetFlag() & Property::F_NO_USE_GC))
				{
					pProperty->Archive(*this, pObject);
				}
			}
		}
	}
	else if (m_uiStreamFlag == AT_LOAD_OBJECT_COLLECT_GC)
	{
		if (RegisterReachableObject(pObject))
		{
			const Rtti& Rtti = pObject->GetType();
			for (uint j = 0; j < Rtti.GetPropertyNum(); j++)
			{
				Property * pProperty = Rtti.GetProperty(j);			
				pProperty->Archive(*this, pObject);
			}
		}
	}
	else if (m_uiStreamFlag == AT_CLEAR_OBJECT_PROPERTY_GC)
	{
		const Rtti& Rtti = pObject->GetType();
		for (uint j = 0; j < Rtti.GetPropertyNum(); j++)
		{
			Property * pProperty = Rtti.GetProperty(j);
			if (!(pProperty->GetFlag() & Property::F_NO_USE_GC))
			{
				pProperty->Archive(*this, pObject);
			}
		}
	}
	return true;
}

bool Stream::LoadFromBuffer(unsigned char* pBuffer, uint uiSize)
{
	if (!pBuffer || !uiSize)
	{
		return false;
	}
	m_pVObjectArray.clear();
	m_pmLoadMap.clear();
	m_pcCurBufPtr = nullptr;
	m_uiBufferSize = 0;

	//都加载到内存中
	m_uiBufferSize = uiSize;
	m_pcBuffer = pBuffer;

	m_pcCurBufPtr = m_pcBuffer;

	Vector<ObjectTableType> ObjectTable;
	//加载版本号
	Read(&m_uiVersion,sizeof(uint));
	//加载物体个数
	uint iObjectNum = 0;
	Read(&iObjectNum,sizeof(uint));

	ObjectTable.resize(iObjectNum);
	//Object Table
	for(uint i = 0 ;i < iObjectNum ; i++)
	{
		Object* pObject = 0;
		//读取指针
		if(!Read(&ObjectTable[i].m_uiGUID, sizeof(uint))) return false;
		//读取RTTI
		if(!ReadString(ObjectTable[i].m_RttiName)) return false;
		if(!Read(&ObjectTable[i].m_uiObjectPropertySize, sizeof(uint))) return false;
		if(!Read(&ObjectTable[i].m_uiObjectPropertyNum, sizeof(uint))) return false;
	}

	//create object
	for(uint i = 0 ;i < iObjectNum ; i++)
	{
		Object * pObject = nullptr;
		if (m_bLoadUseGC)
		{
			pObject = Object::GetInstance(ObjectTable[i].m_RttiName);
		}
		else
		{
			pObject = Object::GetNoGCInstance(ObjectTable[i].m_RttiName);
		}
		//创建空对象
		
		JY_ASSERT(pObject);
		//创建加载映射表
		m_pmLoadMap.insert({ ObjectTable[i].m_uiGUID, pObject });

		RegisterObject(pObject);
	}

	//load object property table
	for (uint i = 0; i < iObjectNum; i++)
	{
		ObjectTable[i].m_ObjectPropertyTable.resize(ObjectTable[i].m_uiObjectPropertyNum);
		for (uint j = 0 ; j < ObjectTable[i].m_uiObjectPropertyNum ; j++)
		{
			Read(&ObjectTable[i].m_ObjectPropertyTable[j].m_uiNameID, sizeof(uint));
			Read(&ObjectTable[i].m_ObjectPropertyTable[j].m_uiOffset,sizeof(uint));
		}
		m_pcCurBufPtr += ObjectTable[i].m_uiObjectPropertySize;
	}

	m_uiStreamFlag = AT_LOAD;
	Map<Object*, Map<uint, Vector<uint>>> cacheLinkID;
	//load object property
	for (uint i = 0; i < m_pVObjectArray.size(); i++)
	{
		const Rtti& rtti = m_pVObjectArray[i]->GetType();
		uint uiTableID = i;
		for (uint j = 0; j < rtti.GetPropertyNum(); j++)
		{
			Property * pProperty = rtti.GetProperty(j);
			if (pProperty->GetFlag() & Property::F_SAVE_LOAD)
			{
				for (uint k = 0; k < ObjectTable[uiTableID].m_ObjectPropertyTable.size(); k++)
				{
					if (StringManager::GetInstance().GetOrCreateName(pProperty->GetName())->GetID() == ObjectTable[uiTableID].m_ObjectPropertyTable[k].m_uiNameID)
					{
						m_pcCurBufPtr = m_pcBuffer + ObjectTable[uiTableID].m_ObjectPropertyTable[k].m_uiOffset;
						pProperty->Archive(*this, m_pVObjectArray[i]);
						break;
					}
				}
			}
		}
	}
	
	//处理连接
	m_uiStreamFlag = AT_LINK;
	// 处理对象拷贝要注意一点(处理指针指向并不需要)，拷贝这个对象，如果这个对象中也有对象需要拷贝，那么子对象必须要预先处理完。因为在加载的时候注册对象是深度递归的，
	// 所以在m_pVObjectArray里面的Object也满足这个顺序，在处理对象拷贝时候，要从叶子节点到根节点，也就是只有它的儿子节点处理完毕，它才能进行处理，
	// 所以这次pProperty->Archive(*this, m_pVObjectArray[i])顺序实际上是从后往前，也就满足了从儿子节点到跟节点的遍历。
	for(int i = m_pVObjectArray.size() - 1; i >= 0 ; i--)
	{
		const Rtti& rtti = m_pVObjectArray[i]->GetType();
		for (uint j = 0 ; j < rtti.GetPropertyNum() ; j++)
		{	
			Property * pProperty = rtti.GetProperty(j);
			if (pProperty->GetFlag() & Property::F_SAVE_LOAD)
			{
				pProperty->Archive(*this, m_pVObjectArray[i]);
			}
		}
	}
	m_pcBuffer = nullptr;
	return true;
}

bool Stream::Load(const char * const pcFileName)
{
	IFile* pFile = IFileSystem::Instance()->FileFactory(pcFileName);
	if (!pFile)
	{
		return false;
	}

	//打开文件
	if(!pFile->OpenFile(IFile::AT_READ))
	{
		SAFE_DELETE(pFile);
		return false;
	}

	//都加载到内存中
	uint uiBufferSize = pFile->GetSize();
	unsigned char * pBuffer = _NEW unsigned char[uiBufferSize];
	if(!pBuffer)
	{
		SAFE_DELETE(pFile);
		return false;
	}
	if(!pFile->ReadFile(pBuffer, uiBufferSize))
	{
		SAFE_DELETE(pFile);
		return false;
	}

	pFile->CloseFile(); 
	IFileSystem::Instance()->RecycleBin(pFile);

	if(!LoadFromBuffer(pBuffer,uiBufferSize))
	{
		SAFE_DELETE_ARRAY(pBuffer);
		return false;
	}
	SAFE_DELETE_ARRAY(pBuffer);

	return true;
}

bool Stream::SaveBuffer()
{
	uint iObjectNum = m_pVObjectArray.size();
	m_uiBufferSize = 0;

	//版本号
	m_uiBufferSize += sizeof(uint );
	//物体个数空间
	m_uiBufferSize += sizeof(uint);

	Vector<ObjectTableType> ObjectTable;
	ObjectTable.resize(iObjectNum);
	m_pmSaveMap.clear();
	// obejct table
	for(uint i = 0 ; i < m_pVObjectArray.size();i++)
	{
		//guid
		m_uiBufferSize += sizeof(uint);
		
		ObjectTable[i].m_uiGUID = i + 1;

		m_pmSaveMap.insert({ m_pVObjectArray[i], ObjectTable[i].m_uiGUID });

		//rtti name
		m_uiBufferSize += GetStrDistUse(m_pVObjectArray[i]->GetType().GetName());

		ObjectTable[i].m_RttiName = m_pVObjectArray[i]->GetType().GetName();

		// object property size
		m_uiBufferSize += sizeof(uint);

		// object property num
		m_uiBufferSize += sizeof(uint);
	}
	
	uint uiObjectContentAddr = m_uiBufferSize;

	m_uiStreamFlag = AT_SIZE;
	//object property size 
	for(uint i = 0 ; i < m_pVObjectArray.size();i++)
	{
		ObjectTable[i].m_uiOffset = m_uiBufferSize;

		Object* pProcessObj = m_pVObjectArray[i];
		const Rtti& rtti = pProcessObj->GetType();
		ObjectTable[i].m_ObjectPropertyTable.resize(rtti.GetPropertyNum());

		ObjectTable[i].m_uiObjectPropertyNum = 0;

		for (uint j = 0 ; j < rtti.GetPropertyNum() ; j++)
		{
			Property * pProperty = rtti.GetProperty(j);
			
			if (pProperty->GetFlag() & Property::F_SAVE_LOAD)
			{
				//name
				m_uiBufferSize += sizeof(uint);
				StringHash* sh = StringManager::GetInstance().GetOrCreateName(pProperty->GetName());
				ObjectTable[i].m_ObjectPropertyTable[j].m_PropertyName = pProperty->GetName();
				ObjectTable[i].m_ObjectPropertyTable[j].m_uiNameID = sh->GetID();
				//offset
				m_uiBufferSize += sizeof(uint);

				ObjectTable[i].m_uiObjectPropertyNum++;
			}
		}

		ObjectTable[i].m_uiObjectPropertyTableSize = m_uiBufferSize - ObjectTable[i].m_uiOffset;

		for (uint j = 0 ; j < rtti.GetPropertyNum() ; j++)
		{
			Property * pProperty = rtti.GetProperty(j);

			if (pProperty->GetFlag() & Property::F_SAVE_LOAD)
			{
				ObjectTable[i].m_ObjectPropertyTable[j].m_uiOffset = m_uiBufferSize;

				m_uiArchivePropertySize = 0;
				pProperty->Archive(*this,m_pVObjectArray[i]);
				ObjectTable[i].m_ObjectPropertyTable[j].m_uiSize = m_uiArchivePropertySize;

				m_uiBufferSize += m_uiArchivePropertySize;
			}
		}

		ObjectTable[i].m_uiObjectPropertySize = m_uiBufferSize - ObjectTable[i].m_uiObjectPropertyTableSize - ObjectTable[i].m_uiOffset;
	}

	for(uint i = 0 ; i < m_pVObjectArray.size();i++)
	{
		m_pVObjectArray[i]->BeforeSave(this);
	}
	//申请空间
	m_saveBuf.clear();
	m_saveBuf.resize(m_uiBufferSize);
	m_pcCurBufPtr = m_pcBuffer = &m_saveBuf[0];

	m_uiVersion = ms_uiCurVersion;
	//存储版本号
	Write(&m_uiVersion,sizeof(uint));

	//存储物体个数
	Write(&iObjectNum,sizeof(uint));

	// OBJECT TABLE
	for(uint i = 0 ; i < m_pVObjectArray.size();i++)
	{
		//guid
		if(!Write(&ObjectTable[i].m_uiGUID,sizeof(uint)))
		{
			m_saveBuf.clear();
			return false;
		}

		//rtti name
		if(!WriteString(ObjectTable[i].m_RttiName))
		{
			m_saveBuf.clear();
			return false;
		}

		Write(&ObjectTable[i].m_uiObjectPropertySize,sizeof(uint));

		Write(&ObjectTable[i].m_uiObjectPropertyNum,sizeof(uint));
	}

	//OBJECT PROPERTY 
	m_uiStreamFlag = AT_SAVE;
	for(uint i = 0 ; i < m_pVObjectArray.size();i++)
	{
		Object* pProcessObj = m_pVObjectArray[i];
		const Rtti& rtti = pProcessObj->GetType();
		for (uint j = 0 ; j < rtti.GetPropertyNum() ; j++)
		{
			Property* pProperty = rtti.GetProperty(j);

			if (pProperty->GetFlag() & Property::F_SAVE_LOAD)
			{
				Write(&ObjectTable[i].m_ObjectPropertyTable[j].m_uiNameID,sizeof(uint));
				Write(&ObjectTable[i].m_ObjectPropertyTable[j].m_uiOffset,sizeof(uint));
			}
		}

		for (uint j = 0 ; j < rtti.GetPropertyNum() ; j++)
		{
			Property * pProperty = rtti.GetProperty(j);
			if (pProperty->GetFlag() & Property::F_SAVE_LOAD)
			{
				pProperty->Archive(*this,m_pVObjectArray[i]);
			}
		}
	}

	for(uint i = 0 ; i < m_pVObjectArray.size();i++)
	{
		m_pVObjectArray[i]->PostSave(this);
	}
	
	return true;
}

bool Stream::Save(const char * const pcFileName)
{
	if (!SaveBuffer())
	{
		return false;
	}

	IFile* pFile = IFileSystem::Instance()->FileFactory(pcFileName);
	if (!pFile)
	{
		m_saveBuf.clear();
		return false;
	}
	//打开文件
	if(!pFile->OpenFile(IFile::AT_WRITE))
	{
		SAFE_DELETE(pFile);
		m_saveBuf.clear();
		return false;
	}

	if(!pFile->WriteFile(m_saveBuf.data(), m_uiBufferSize))
	{
		SAFE_DELETE(pFile);
		m_saveBuf.clear();
		return false;
	}

	pFile->CloseFile();
	IFileSystem::Instance()->RecycleBin(pFile);
	m_saveBuf.clear();
	return true;
}

NS_JYE_END
