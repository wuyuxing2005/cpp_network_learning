#include "Timer.h"
Timer::Timer(double _interval, TimeStamp timestamp, std::function<void()> _CallBack)
    : interval(_interval), isRepeat(_interval > 0.0), CallBack(_CallBack), expiration(timestamp) {}
void Timer::run()
{
    CallBack();
}
TimeStamp Timer::getExpirationTime()
{
    return expiration;
}
bool Timer::IsRepeat()
{
    return isRepeat;
}
void Timer::restart(TimeStamp Now)
{
    expiration = TimeStamp(Now.microseconds() + interval);
}
Timer::~Timer()
{
}
