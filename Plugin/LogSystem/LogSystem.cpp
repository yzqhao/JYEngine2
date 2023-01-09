#include "LogSystem.h"
#include "Core/Interface/ITimeSystem.h"
#include "Math/Math.h"

#include <iostream>

#ifdef PLATFORM_WIN32
#include <tchar.h>
#endif

NS_JYE_BEGIN

static const int s_maxTagLength = 20;

namespace 
{
	void SetTag(char* outString, const std::string& tag)
	{
		outString[0] = '[';
		outString[s_maxTagLength - 1] = ']';
	}

}

LogSystem::LogSystem()
{
	init();
}

LogSystem::~LogSystem()
{
}

void LogSystem::init()
{
#ifdef PLATFORM_WIN32
	AllocConsole();
	_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
	_mutex = std::move(std::make_unique<std::mutex>());
}

void LogSystem::Print(LogType t, char* str)
{
#ifdef PLATFORM_ANDROID
	switch (t) {
	case LogType::LT_LOG:
		__android_log_print(ANDROID_LOG_INFO, "JYELOG", "%s", str);
		break;
	case LogType::LT_WARNING:
#ifdef NDEBUG
		LOGE("[JYELOG]W %s", str);
#else
		__android_log_print(ANDROID_LOG_WARN, "JYELOG", "%s", str);
#endif
		break;
	case LogType::LT_ERROR:
#ifdef NDEBUG
		LOGE("[JYELOG]E %s", str);
#else
		__android_log_print(ANDROID_LOG_ERROR, "JYELOG", "%s", str);
#endif
		break;
	}
#endif

#if PLATFORM_IOS || PLATFORM_MAC
	switch (t) {
	case LogType::LT_LOG:
		break;
	case LogType::LT_WARNING:
		LOGE("[JYELOG]W %s", str);
		break;
	case LogType::LT_ERROR:
		LOGE("[JYELOG]E %s", str);
		break;
	default:
		break;
	}
#endif

#ifdef PLATFORM_WIN32
	switch (t)
	{
	case LT_LOG:
		SetConsoleTextAttribute(_hConsole, 2);
		break;
	case LT_WARNING:
		SetConsoleTextAttribute(_hConsole, 14);
		break;
	case LT_ERROR:
		SetConsoleTextAttribute(_hConsole, 12);
		break;
	default:
		SetConsoleTextAttribute(_hConsole, 7);
		break;
	}
#endif
	std::cout << str << std::endl;
#ifdef PLATFORM_WIN32
	SetConsoleTextAttribute(_hConsole, 7);
#endif
	
}

void LogSystem::WriteLog(LogType lt, const char*file, int nLine, char* szLogContent)
{
	if (m_isImmediate)
	{
		_mutex->lock();
		Print(lt, szLogContent);
		_mutex->unlock();
	}
	else
	{
		// TODO
		JY_ASSERT(false);
	}

#if RELEASE 
	std::string fn;
	fn = szEngineLog;
	fn += ".";
	fn += szDate;
	std::ofstream fd;
	try{
		fd.open(fn.c_str(), std::ios::app);
		fd << szTime << " : " << tag << " " << szLogContent << std::endl;
		fd.close();
	}catch (...){
		printf("open log file exception!\n");
		return;
	}
#endif
}

void VSLog(const char* file, int nLine, const char* format, ...)
{
#ifdef PLATFORM_WIN32
	char* pArgs = (char*)&format + sizeof(format);
	static TCHAR s_sLogBuffer[1024];
	_vstprintf_s(s_sLogBuffer, 1024, format, pArgs);
	OutputDebugString(s_sLogBuffer);
#endif
}


NS_JYE_END
