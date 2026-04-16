#pragma once
#include "channel.h"
#include "epoll.h"
#include "Thread_Poll/ThreadPoll.h"
#include <vector>
#include <functional>
#include <memory>
#include <mutex>
#include "./time/TimeStamp.h"
class epoll;
class TimeStamp;
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
    void RunEvery(double interval, std::function<void()> &cb);   // 在当前时间每隔一段时间run一次
    void RunAt(TimeStamp *timestamp, std::function<void()> &cb); // 在指定时间戳run一次
    void RUnAfter(double wait_time, std::function<void()> &cb);  // 在当前时间之后的时间run一次
};
