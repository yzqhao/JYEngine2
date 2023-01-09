#pragma once

#include "../Core.h"
#include "System/Singleton.hpp"
#include "Script/IVirtualMachine.h"

NS_JYE_BEGIN

class CORE_API IScriptSystem
{
	SYSTEM_SINGLETON_DECLEAR(IScriptSystem);
private:
	typedef std::vector< IVirtualMachine* > MachineArray;
public:
	IScriptSystem();
	virtual ~IScriptSystem();

	const std::string& GetPackagePath() const { return m_packagePath; }
	void SetPackagePath(const std::string& path) { m_packagePath = path; }

	IVirtualMachine* GetVirtualMachineByIdx(int idx) const { return (m_pMachineArray)[idx]; }

	IVirtualMachine* VirtualMachineFactory();
	void RecycleBin(IVirtualMachine* vm);

protected:
	virtual IVirtualMachine* _DoVirtualMachineFactory(uint index) = 0;
	virtual void _DoRecycleBin(IVirtualMachine* vm) = 0;

private:
	std::string m_packagePath;
	MachineArray m_pMachineArray;
	volatile int m_VMCount;
};

NS_JYE_END