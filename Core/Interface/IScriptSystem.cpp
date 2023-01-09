#include "IScriptSystem.h"
#include "System/platform/ThreadSafeOprator.h"

NS_JYE_BEGIN

SYSTEM_SINGLETON_IMPLEMENT(IScriptSystem);

IScriptSystem::IScriptSystem()
	: m_VMCount(0)
{
	SYSTEM_SINGLETON_INITIALIZE;
}

IScriptSystem::~IScriptSystem()
{
	SYSTEM_SINGLETON_DESTROY;

	for (auto& it : m_pMachineArray)
	{
		RecycleBin(it);
	}
}

IVirtualMachine* IScriptSystem::VirtualMachineFactory()
{
	IVirtualMachine* vm = _DoVirtualMachineFactory(m_VMCount);
	vm->Initialize();
	m_pMachineArray.push_back(vm);
	Increment(&m_VMCount);
	return vm;
}

void IScriptSystem::RecycleBin(IVirtualMachine* vm)
{
	vm->Exit();
	uint index = vm->GetIndex();
	(m_pMachineArray)[index] = NULL;
	_DoRecycleBin(vm);
}

NS_JYE_END