#pragma once

#include "Timer.h"

#include <set>
#include <utility>
#include <vector>

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
