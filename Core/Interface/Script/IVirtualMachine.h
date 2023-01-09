#pragma once

#include "../../Core.h"

NS_JYE_BEGIN

class CORE_API IVirtualMachine
{
public:
	IVirtualMachine(uint index);
	virtual ~IVirtualMachine();

	virtual void Initialize() = 0;
	virtual void Exit() = 0;
	virtual void CallScript(const std::string& script_file_path) = 0;
	virtual void DoLoadScript(const std::string& script_file_path) = 0;
	virtual void CallScriptFunc(const std::string& script_function) = 0;
	virtual void* GetResult() = 0;

	uint GetIndex() const { return m_MachineIndex; }

public:

	const uint m_MachineIndex;
};

NS_JYE_END