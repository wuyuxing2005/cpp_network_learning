#pragma once
#include "channel.h"
#include "epoll.h"
#include "Thread_Poll/ThreadPoll.h"
#include <vector>
#include <functional>

class epoll;
class channel;
class EventLoop
{
private:
    epoll *ep;
    Thread_pool<std::function<void()>> *thread_pool;

public:
    EventLoop();
    ~EventLoop();
    void beginLoop();
    void updateChannel(channel *ch);
    void deleteChannel(channel *ch);
    bool addInPoll(std::function<void()> CallBack);
};
