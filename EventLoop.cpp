#include "EventLoop.h"
EventLoop::EventLoop()
{
    ep = new epoll();
    thread_pool = new Thread_pool<std::function<void()>>(10, 1000);
}

EventLoop::~EventLoop()
{
    delete ep;
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
    }
}
bool EventLoop::addInPoll(std::function<void()> CallBack)
{
    return thread_pool->append(std::move(CallBack));
}
