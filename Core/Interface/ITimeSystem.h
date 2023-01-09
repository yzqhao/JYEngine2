
#pragma once

#include "../Core.h"
#include "../../System/Singleton.hpp"

#include <chrono>

NS_JYE_BEGIN

class CORE_API ITimeSystem
{
	SYSTEM_SINGLETON_DECLEAR(ITimeSystem);
public:
    explicit ITimeSystem();
    ~ITimeSystem();

    // 程序启动计时
    virtual double GetGamePlayTime() = 0;

	virtual void UpdateFPS() = 0;
	virtual double GetFPS() = 0;
	virtual double GetDetTime() = 0;
	virtual unsigned GetTotalFrame() = 0;
};

NS_JYE_END
