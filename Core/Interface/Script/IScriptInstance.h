#pragma once

#include "IFunctionInvoker.h"

NS_JYE_BEGIN

class Object;

class CORE_API IScriptInstance
{
private:
	struct UserFunction
	{
		IFunctionInvoker* m_pInvoker;
		UserFunction(IFunctionInvoker* inv)
			: m_pInvoker(inv) {}
	};
	typedef std::map< uint, UserFunction* > FuncMap;
protected:
	IScriptInstance();
	virtual ~IScriptInstance();

public:
	virtual void SetMember(const char* name) = 0;
	virtual void SetMember(const char* name, bool obj) = 0;
	virtual void SetMember(const char* name, short obj) = 0;
	virtual void SetMember(const char* name, uint16 obj) = 0;
	virtual void SetMember(const char* name, int obj) = 0;
	virtual void SetMember(const char* name, uint obj) = 0;
	virtual void SetMember(const char* name, float obj) = 0;
	virtual void SetMember(const char* name, int64 obj) = 0;
	virtual void SetMember(const char* name, uint64 obj) = 0;
	virtual void SetMember(const char* name, double obj) = 0;
	virtual void SetMember(const char* name, const char* obj, uint len) = 0;
	virtual void SetMember(const char* name, Object* obj) = 0;
	virtual void SetMember(const char* name, Object* obj) = 0;

	FORCEINLINE uint GetScriptHash() const;
	FORCEINLINE bool isFunctionExist(const std::string& name);
	FORCEINLINE uint GetMachineIndex() const;
	FORCEINLINE const std::string& GetClassName();

private:
	std::string		m_ClassName;
	FuncMap			m_FunctionMap;
};

FORCEINLINE const std::string& IScriptInstance::GetClassName()
{
	return m_ClassName;
}

FORCEINLINE bool IScriptInstance::isFunctionExist(const std::string& name)
{
	return false;// _isFunctionExist(name);
}

NS_JYE_END