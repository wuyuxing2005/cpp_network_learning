#include "base/EventLoop.h"

EventLoop::EventLoop() : ep(std::make_unique<epoll>()), timequeue(this)
{
}

EventLoop::~EventLoop()
{
}

void EventLoop::updateChannel(channel *ch)
{
    ep->updateChannel(ch);
}

void EventLoop::deleteChannel(channel *ch)
{
    ep->deleteChannel(ch);
}

void EventLoop::beginLoop()
{
    while (1)
    {
        std::vector<channel *> chs = ep->poll();
        for (channel *ch : chs)
        {
            ch->handleEventByCallBack();
        }
        DoToDolist();
    }
}
void EventLoop::DoToDolist() // 目前是用来在handleEVent之后来进行回收处理
{
    std::vector<std::function<void()>> functions;
    {
        std::lock_guard<std::mutex> guard(mutex_);
        functions.swap(to_do_list_); // 直接清空to_do_list里面的数据并提取出来到functions中
    }
    for (auto &func : functions)
    {
        func(); // 调用ConnectionDestructor
    }
}
void EventLoop::PushFuncInToDoList(std::function<void()> cb)
{
    std::lock_guard<std::mutex> guard(mutex_);
    to_do_list_.emplace_back(std::move(cb));
}

void EventLoop::RunAfter(double wait_time, std::function<void()> &cb)
{
    TimeStamp timestamp = TimeStamp::AddTime(TimeStamp::getNowTime(), wait_time);
    RunAt(&timestamp, cb);
}
void EventLoop::RunAt(TimeStamp *timestamp, std::function<void()> &cb)
{
    Timer *timer = new Timer(0.0, *timestamp, cb);
    timequeue.AddTimer(timer);
}
void EventLoop::RunEvery(double wait_time, std::function<void()> &cb)
{
    TimeStamp timestamp = TimeStamp::AddTime(TimeStamp::getNowTime(), wait_time);
    Timer *timer = new Timer(wait_time, timestamp, cb);
    timequeue.AddTimer(timer);
}
