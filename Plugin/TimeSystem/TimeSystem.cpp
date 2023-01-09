
#include "TimeSystem.h"


NS_JYE_BEGIN

TimeSystem::TimeSystem()
{
	_beginTime = std::chrono::system_clock::now();
}

TimeSystem::~TimeSystem()
{
}

double TimeSystem::GetGamePlayTime()
{
	auto nowtime = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = nowtime - _beginTime;
	return elapsed_seconds.count();
}

void TimeSystem::UpdateFPS()
{
	m_fTime = GetGamePlayTime();
	m_fDetTime = m_fTime - m_fLastTime;
	m_fLastTime = m_fTime;
	if (m_fTime - m_fLastFPSTime > 1.0f)
	{
		m_fLastFPSTime = m_fTime;
		m_fFPS = m_iFrameCount;
		m_iFrameCount = 0;
	}
	else
	{
		m_iFrameCount++;
	}
	m_iTotalFrameCount++;
}

NS_JYE_END
