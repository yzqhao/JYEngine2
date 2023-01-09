#pragma once

#include "IAsyncSource.h"

NS_JYE_BEGIN

class ENGINE_API ISharedSource : public IAsyncSource
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	ISharedSource(ResourceProperty::ResourceType rt, bool isfinalizer);
	virtual ~ISharedSource();
	
	FORCEINLINE bool isFinalizer()
	{
		return m_isFinalizer;
	}

	FORCEINLINE void JustDepends()
	{
		m_isJustDepends = true;
	}
	FORCEINLINE bool isJustDepends()//ֻ����������
	{
		return m_isJustDepends;
	}

private:
	virtual bool _isLoadResource() override;//�Ƿ����

	bool m_isFinalizer;
	bool m_isJustDepends;//�����source��resource�Ѿ����أ����������ֻ����������
};
DECLARE_Ptr(ISharedSource);
TYPE_MARCO(ISharedSource);

NS_JYE_END