#pragma once

#include "private/Define.h"
#include "System/Singleton.hpp"
#include "System/DataStruct.h"
#include "Core/Interface/ILogSystem.h"

#include <mutex>

#ifdef PLATFORM_WIN32
#include <windows.h>
#endif

NS_JYE_BEGIN

class LogSystem : public ILogSystem
{
public:
	
	LogSystem();
	virtual ~LogSystem();

	void init();
	
protected:

	void Print(LogType t, char* str);

	virtual void WriteLog(LogType lt, const char* file, int nLine, char* szLogContent) override;

	std::unique_ptr<std::mutex> _mutex{};
#ifdef PLATFORM_WIN32
	HANDLE _hConsole;
#endif
};
	

NS_JYE_END