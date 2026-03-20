#pragma once
#include "channel.h"
#include "epoll.h"
#include "Thread_Poll/ThreadPoll.h"
#include <vector>
#include <functional>
#include <memory>
class epoll;
class channel;
class EventLoop
{
private:
    std::unique_ptr<epoll> ep;
    // Thread_pool<std::function<void()>> *thread_pool;

public:
    EventLoop();
    ~EventLoop();
    void beginLoop();
    void updateChannel(channel *ch);
    void deleteChannel(channel *ch);
    // bool addInPoll(std::function<void()> CallBack);
    // 此时我的loop只需要进行epoll监听事件即可，线程池中的每一个线程都有一个loop，不需要再在loop中再创建线程池了
};
