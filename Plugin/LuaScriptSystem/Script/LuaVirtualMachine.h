
#pragma once

#include "../private/Define.h"
#include "Core/Interface/Script/IVirtualMachine.h"

#include <sol/sol.hpp>

NS_JYE_BEGIN

class LUA_SCRIPT_SYSTEM_API LuaVirtualMachine : public IVirtualMachine
{
public:
	LuaVirtualMachine(uint index);
	virtual ~LuaVirtualMachine();

	virtual void Initialize() override;
	virtual void Exit() override;
	virtual void CallScript(const std::string& script_file_path) override;
	virtual void DoLoadScript(const std::string& script_file_path) override;
	virtual void CallScriptFunc(const std::string& script_function) override;
	virtual void* GetResult() override;

private:
	sol::state m_solState;

	sol::table m_res;
};

NS_JYE_END