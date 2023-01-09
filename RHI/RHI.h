
#pragma once

#include "private/Define.h"
#include "RHIDefine.h"
#include "Core/TEnumAsByte.h"

NS_JYE_BEGIN

extern RHI_API bool GIsRHIInitialized;
extern RHI_API bool GUsingNullRHI;

struct InputUniformInfo
{
	std::string name;
	RHIDefine::VariablesType type;
	RHIDefine::ParameterSlot slot;
	unsigned short num;
	unsigned short regIndex;
	unsigned short regCount;
	std::vector<InputUniformInfo> subUniform;
	byte srv_uav;
};

struct InputAttributes
{
	std::string name;
	unsigned short id;
};

struct OutputUniformInfo
{
	std::string name;
	RHIDefine::VariablesType type;
	RHIDefine::ParameterSlot slot;
	handle uniHandle;
	unsigned short num;
	unsigned short regIndex;
	unsigned short regCount;
	std::vector<OutputUniformInfo> subInfos;
	byte srv_uav;
};

NS_JYE_END

#include "DynamicRHI.h"

NS_JYE_BEGIN

/** Initializes the RHI. */
extern RHI_API void RHIInit();

/** Shuts down the RHI. */
extern RHI_API void RHIExit();

NS_JYE_END