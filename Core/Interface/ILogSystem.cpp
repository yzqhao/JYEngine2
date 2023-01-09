
#include "ILogSystem.h"
#include "ITimeSystem.h"

#ifdef PLATFORM_WIN32
#include <windows.h>
#endif

NS_JYE_BEGIN

static char szEngineLog[MAX_LOG_LENGTH];

static const int s_maxTagLength = 20;

SYSTEM_SINGLETON_IMPLEMENT(ILogSystem);

ILogSystem::ILogSystem()
	: m_isImmediate(true)
{
	SYSTEM_SINGLETON_INITIALIZE;
}

ILogSystem::~ILogSystem()
{
	SYSTEM_SINGLETON_DESTROY;
}

void ILogSystem::Log(const char* file, int nLine, const char* format, ...)
{
	if (strlen(format) == 0)
		return;
	char* szBuffer = nullptr;
	if (!m_isImmediate)
	{
		szBuffer = new char[MAX_LOG_LENGTH];
		memset(szBuffer, 0, MAX_LOG_LENGTH);
	}
	else
	{
		szBuffer = szEngineLog;
		memset(szBuffer, 0, MAX_LOG_LENGTH);
	}

	va_list	l_va;
	va_start(l_va, format);
	vsnprintf(szBuffer, MAX_LOG_LENGTH, format, l_va);
	va_end(l_va);
	WriteLog(LT_LOG, file, nLine, szBuffer);
}

void ILogSystem::Warning(const char* file, int nLine, const char* format, ...)
{
	if (strlen(format) == 0)
		return;
	char* szBuffer = nullptr;
	if (!m_isImmediate)
		szBuffer = new char[MAX_LOG_LENGTH];
	else
		szBuffer = szEngineLog;
	memset(szBuffer, 0, MAX_LOG_LENGTH);

	va_list	l_va;
	va_start(l_va, format);
	vsnprintf(szBuffer, MAX_LOG_LENGTH, format, l_va);
	va_end(l_va);

	WriteLog(LT_WARNING, file, nLine, szBuffer);
}

void ILogSystem::Error(const char* file, int nLine, const char* format, ...)
{
	if (strlen(format) == 0)
		return;
	char* szBuffer = nullptr;
	if (!m_isImmediate)
		szBuffer = new char[MAX_LOG_LENGTH];
	else
		szBuffer = szEngineLog;
	memset(szBuffer, 0, MAX_LOG_LENGTH);

	va_list	l_va;
	va_start(l_va, format);
	vsnprintf(szBuffer, MAX_LOG_LENGTH, format, l_va);
	va_end(l_va);

	WriteLog(LT_ERROR, file, nLine, szBuffer);
}

void ILogSystem::LogTag(const std::string& tag, const char* file, int nLine, const char* format, ...)
{
	if (strlen(format) == 0)
		return;
	char* szBuffer = nullptr;
	if (!m_isImmediate)
	{
		szBuffer = new char[MAX_LOG_LENGTH];
		memset(szBuffer, 0, MAX_LOG_LENGTH);
	}
	else
	{
		szBuffer = szEngineLog;
		memset(szBuffer, 0, MAX_LOG_LENGTH);
	}

	snprintf(szBuffer, s_maxTagLength - 1, "[%17s]", tag.c_str());
	szBuffer[s_maxTagLength - 2] = ']';
	szBuffer[s_maxTagLength - 1] = ' ';

	va_list	l_va;
	va_start(l_va, format);
	vsnprintf(szBuffer + s_maxTagLength, MAX_LOG_LENGTH, format, l_va);
	va_end(l_va);
	WriteLog(LT_LOG, file, nLine, szBuffer);
}

void ILogSystem::WarningTag(const std::string& tag, const char* file, int nLine, const char* format, ...)
{
	if (strlen(format) == 0)
		return;
	char* szBuffer = nullptr;
	if (!m_isImmediate)
		szBuffer = new char[MAX_LOG_LENGTH];
	else
		szBuffer = szEngineLog;
	memset(szBuffer, 0, MAX_LOG_LENGTH);

	snprintf(szBuffer, s_maxTagLength - 1, "[%17s]", tag.c_str());
	szBuffer[s_maxTagLength - 2] = ']';
	szBuffer[s_maxTagLength - 1] = ' ';

	va_list	l_va;
	va_start(l_va, format);
	vsnprintf(szBuffer + s_maxTagLength, MAX_LOG_LENGTH, format, l_va);
	va_end(l_va);

	WriteLog(LT_WARNING, file, nLine, szBuffer);
}

void ILogSystem::ErrorTag(const std::string& tag, const char* file, int nLine, const char* format, ...)
{
	if (strlen(format) == 0)
		return;
	char* szBuffer = nullptr;
	if (!m_isImmediate)
		szBuffer = new char[MAX_LOG_LENGTH];
	else
		szBuffer = szEngineLog;
	memset(szBuffer, 0, MAX_LOG_LENGTH);

	snprintf(szBuffer, s_maxTagLength - 1, "[%17s]", tag.c_str());
	szBuffer[s_maxTagLength - 2] = ']';
	szBuffer[s_maxTagLength - 1] = ' ';

	va_list	l_va;
	va_start(l_va, format);
	vsnprintf(szBuffer + s_maxTagLength, MAX_LOG_LENGTH, format, l_va);
	va_end(l_va);

	WriteLog(LT_ERROR, file, nLine, szBuffer);
}

void ILogSystem::ScriptLog(const char* str)
{
	double time = ITimeSystem::Instance()->GetGamePlayTime();
	Log(nullptr, 0, "[%s %10.5f] %s", "LUA : ", time, str);
}

void ILogSystem::ScriptWarning(const char* str)
{
	double time = ITimeSystem::Instance()->GetGamePlayTime();
	Warning(nullptr, 0, "[%s %10.5f] %s", "LUA : ", time, str);
}

void ILogSystem::ScriptError(const char* str)
{
	double time = ITimeSystem::Instance()->GetGamePlayTime();
	Error(nullptr, 0, "[%s %10.5f] %s", "LUA : ", time, str);
}

NS_JYE_END