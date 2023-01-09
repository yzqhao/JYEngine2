
#pragma once

#include "Core/Interface/ITimeSystem.h"

#include <chrono>

NS_JYE_BEGIN

class TimeSystem : public ITimeSystem
{
public:
    explicit TimeSystem();
    ~TimeSystem();

    // 程序启动计时
    double GetGamePlayTime();

	void UpdateFPS();
	double GetFPS() { return m_fFPS; }
	double GetDetTime() { return m_fDetTime; }
	unsigned GetTotalFrame() { return m_iTotalFrameCount; }
    
private:

    std::chrono::system_clock::time_point	_beginTime;

	int m_iFrameCount{};
	unsigned m_iTotalFrameCount{};
	double m_fFPS{};
	double m_fTime{};
	double m_fLastFPSTime{};
	double m_fDetTime{};
	double m_fLastTime{};
};

NS_JYE_END
