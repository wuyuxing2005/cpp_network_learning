#pragma once
#include "channel.h"
#include "epoll.h"
#include "Thread_Poll/ThreadPoll.h"
#include <vector>
#include <functional>
#include <memory>
#include <mutex>
class epoll;
class channel;
class EventLoop
{
private:
    std::unique_ptr<epoll> ep;
    std::vector<std::function<void()>> to_do_list_;
    std::mutex mutex_;

public:
    EventLoop();
    ~EventLoop();
    void beginLoop();
    void updateChannel(channel *ch);
    void deleteChannel(channel *ch);
    void DoToDolist();
    void PushFuncInToDoList(std::function<void()> cb);
};
