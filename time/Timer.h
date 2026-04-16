#pragma once
#include "TimeStamp.h"
#include <functional>
class Timer
{
private:
    Timer(const Timer &) = delete;
    Timer &operator=(const Timer &) = delete;
    Timer(Timer &&) = delete;
    Timer &operator=(Timer &&) = delete;
    double interval;                // 重复时间周期
    TimeStamp expiration;           // 重复时间的时间戳
    bool isRepeat;                  // 是否是重复Timer
    std::function<void()> CallBack; // 到达时间戳后采取的行为

public:
    Timer(double _interval, TimeStamp Now, std::function<void()> CallBack);
    void run();
    bool IsRepeat();
    TimeStamp getExpirationTime();
    void restart(TimeStamp Now);
    ~Timer();
};
