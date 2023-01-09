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
	FORCEINLINE bool isJustDepends()//只加载依赖项
	{
		return m_isJustDepends;
	}

private:
	virtual bool _isLoadResource() override;//是否加载

	bool m_isFinalizer;
	bool m_isJustDepends;//当这个source的resource已经加载，设置这个是只加载依赖项
};
DECLARE_Ptr(ISharedSource);
TYPE_MARCO(ISharedSource);

NS_JYE_END