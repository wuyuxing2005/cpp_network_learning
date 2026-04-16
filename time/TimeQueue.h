#include <set>
#include "Timer.h"
#include "base/EventLoop.h"
#include "base/channel.h"
#include <sys/timerfd.h>
#include <sys/socket.h>
#include <cstring>
#include <assert.h>
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
    uint64_t howmany;
    ::read(timefd, &howmany, sizeof(howmany));
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
    for (auto &p : active_timers)
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
    if (timers.empty() || (timer->getExpirationTime() < timers.begin()->first)) // 当timers为零，此时timerfd应该和该任务的到期时间对齐。
                                                                                // 当timers有其他任务，但是新的任务的到期时间小于timers里面的第一个到期的任务的到期时间，此时同样要对其timerfd的时间进行重置，使其和新的任务的到期时间对齐。
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
    struct itimerspec new_;
    struct itimerspec old_;
    memset(&new_, '\0', sizeof(new_));
    memset(&old_, '\0', sizeof(old_));
    int64_t micro_seconds_dif = timer->getExpirationTime().microseconds() - TimeStamp::getNowTime().microseconds();
    if (micro_seconds_dif < 100) // 当新的任务的到期时间和当前时间的差小于100微秒时，timerfd的时间设置为100微秒，以保证timerfd能够及时触发。
    {
        micro_seconds_dif = 100;
    }

    new_.it_value.tv_sec = static_cast<time_t>(
        micro_seconds_dif / kMicrosecond2Second);
    new_.it_value.tv_nsec = static_cast<long>((micro_seconds_dif % kMicrosecond2Second) * 1000);
    int ret = ::timerfd_settime(timefd, 0, &new_, &old_); // 此处设置linux timerfd的时间
    assert(ret != -1);
    (void)ret;
}