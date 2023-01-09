#pragma once

#include "../private/Define.h"
#include "Core/Object.h"
#include "Core/Propery/ResourceProperty.h"

NS_JYE_BEGIN

class ENGINE_API IMetadata : public Object
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	IMetadata();
	virtual ~IMetadata();

	virtual void GetIdentifier(String& hash) const = 0;		//��str�������ʾ����
	virtual void ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash) = 0;	//����ԭʼ��Դ
	virtual void ReleaseMetadate() = 0;		//������Ҫ�Ļ���
	virtual bool isRedirected() const;

	template< typename T >
	T* GetMetadata()
	{
		return static_cast<T*>(_ReturnMetadata());
	}

protected:
	virtual void* _ReturnMetadata() = 0;

};
DECLARE_Ptr(IMetadata);
TYPE_MARCO(IMetadata);

typedef Vector < IMetadata* > ISourceMetadataList;

NS_JYE_END