#include <set>
#include "Timer.h"
#include "base/EventLoop.h"
#include "base/channel.h"
#include <sys/timerfd.h>
#include <sys/socket.h>
class channel;
class EventLoop;
class TimeQueue
{
private:
    std::set<std::pair<TimeStamp, Timer *>> timers;
    channel *ch;
    int timefd;
    EventLoop *loop;
    std::vector<std::pair<TimeStamp, Timer *>> active_timers;

public:
    TimeQueue(EventLoop *_loop);
    void createFd();
    void ReadFd();
    void handleRead();
    void ResetTimers();
    bool Insert(Timer *timer);
    void AddTimer(Timer *timer);
    void ResetTimerFd(Timer *timer);
    ~TimeQueue();
};

TimeQueue::TimeQueue(EventLoop *_loop) : loop(_loop)
{
    createFd();
    ch = new channel(loop, timefd, false, false);
    ch->setReadCallBack(std::bind(&TimeQueue::handleRead, this));
    ch->enAbleToReading();
}

void TimeQueue::createFd()
{
    timefd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if (timefd < 0)
    {
        printf("TimerQueue::CreateTimerfd error\n");
    }
}
void TimeQueue::ReadFd()
{
    char buffer[64];
    recv(timefd, buffer, sizeof(buffer), 0);
    return;
}
void TimeQueue::handleRead()
{
    ReadFd();
    active_timers.clear();
    auto end = timers.lower_bound(std::make_pair(TimeStamp::getNowTime(), reinterpret_cast<Timer *>(UINTPTR_MAX)));
    active_timers.insert(active_timers.end(), timers.begin(), end);
    timers.erase(timers.begin(), end);
    for (auto &p : active_timers)
    {
        p.second->run();
    }
    ResetTimers();
}
void TimeQueue::ResetTimers()
{
    for (auto &p : timers)
    {
        if (p.second->IsRepeat())
        {
            auto timer = p.second;
            timer->restart(TimeStamp::getNowTime());
            Insert(timer);
        }
        else
        {
            delete p.second;
        }
    }
}
bool TimeQueue::Insert(Timer *timer)
{
    bool reset_instantly = false;
    if (timers.empty() || (timer->getExpirationTime() < timers.begin()->first))
    {
        reset_instantly = true;
    }
    timers.insert({timer->getExpirationTime(), timer});
    return reset_instantly;
}
void TimeQueue::AddTimer(Timer *timer)
{
    if (Insert(timer))
    {
        ResetTimerFd(timer);
    }
}
TimeQueue::~TimeQueue()
{
}
void TimeQueue::ResetTimerFd(Timer *timer)
{
}