#pragma once

#include "../../Core.h"

NS_JYE_BEGIN

class CORE_API IFunctionInvoker
{
private:
	uint		m_ID;
protected:
	IFunctionInvoker() : m_ID(nullhandle) {}
	virtual ~IFunctionInvoker() {}

public:

	FORCEINLINE void SetFunctionID(uint id)
	{
		m_ID = id;
	}

	FORCEINLINE uint GetFunctionID()
	{
		return m_ID;
	}

	virtual int GetResultCnt() = 0;
	virtual void* GetResult(int idx) = 0;

protected:
};
NS_JYE_END