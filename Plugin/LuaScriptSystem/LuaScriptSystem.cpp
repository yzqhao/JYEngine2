#include "LuaScriptSystem.h"
#include "Core/Interface/ILogSystem.h"
#include "Core/Interface/IFileSystem.h"
#include "Script/LuaVirtualMachine.h"

NS_JYE_BEGIN

LuaScriptSystem::LuaScriptSystem()
{
}

LuaScriptSystem::~LuaScriptSystem()
{

}

IVirtualMachine* LuaScriptSystem::_DoVirtualMachineFactory(uint index)
{
	return _NEW LuaVirtualMachine(index);
}

void LuaScriptSystem::_DoRecycleBin(IVirtualMachine* vm)
{
	SAFE_DELETE(vm);
}

NS_JYE_END
