
#pragma once

#include "private/Define.h"
#include "Core/Interface/IScriptSystem.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include <sol/sol.hpp>

NS_JYE_BEGIN

class LUA_SCRIPT_SYSTEM_API LuaScriptSystem : public IScriptSystem
{
public:
	LuaScriptSystem();
	virtual ~LuaScriptSystem();

protected:
	virtual IVirtualMachine* _DoVirtualMachineFactory(uint index) override;
	virtual void _DoRecycleBin(IVirtualMachine* vm) override;

private:

};

NS_JYE_END