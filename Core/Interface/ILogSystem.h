#pragma once

#include "../Core.h"
#include "System/Singleton.hpp"
#include "System/DataStruct.h"

#include <mutex>

NS_JYE_BEGIN

#define MAX_LOG_LENGTH		1024 * 4

class CORE_API ILogSystem
{
	SYSTEM_SINGLETON_DECLEAR(ILogSystem);
public:
	enum LogType
	{
		LT_LOG = 0,
		LT_WARNING,
		LT_ERROR,
	};
	
	ILogSystem();
	virtual ~ILogSystem();

	void Log(const char* file, int nLine, const char* format, ...);
	void Warning(const char* file, int nLine, const char* format, ...);
	void Error(const char* file, int nLine, const char* format, ...);

	void LogTag(const std::string& tag, const char* file, int nLine, const char* format, ...);
	void WarningTag(const std::string& tag, const char* file, int nLine, const char* format, ...);
	void ErrorTag(const std::string& tag, const char* file, int nLine, const char* format, ...);

	void ScriptLog(const char* str);
	void ScriptWarning(const char* str);
	void ScriptError(const char* str);

	void SetLogFile(uint filesize, const String& filename) { m_fileSize = filesize;  m_logFile = filename; }
	
protected:

	virtual void WriteLog(LogType lt, const char* file, int nLine, char* szLogContent) = 0;

	bool m_isImmediate{ false };
	uint m_fileSize{ };
	String m_logFile;
};

#if _DEBUG
#define JYLOG(f, ...) ILogSystem::Instance()->Log(__FILE__,__LINE__,f,##__VA_ARGS__)
#define JYLOGTAG(tag, f, ...) ILogSystem::Instance()->LogTag(tag.sTagName, __FILE__,__LINE__,f,##__VA_ARGS__)
#else
#define JYLOG(f, ...)
#define JYLOGTAG(tag, f, ...)
#endif

#define JYWARNING(f, ...) ILogSystem::Instance()->Warning(__FILE__,__LINE__,f,##__VA_ARGS__)
#define JYWARNINGTAG(tag, f, ...) ILogSystem::Instance()->WarningTag(tag.sTagName, __FILE__,__LINE__,f,##__VA_ARGS__)
#define JYERROR(f, ...) ILogSystem::Instance()->Error(__FILE__,__LINE__,f,##__VA_ARGS__)
#define JYERRORTAG(tag, f, ...) ILogSystem::Instance()->ErrorTag(tag.sTagName, __FILE__,__LINE__,f,##__VA_ARGS__)

NS_JYE_END