#include "EventLoop.h"
EventLoop::EventLoop()
{
    ep = new epoll();
    thread_pool = new Thread_pool<std::function<void()>>(10, 10);
}

EventLoop::~EventLoop()
{
    delete ep;
}

void EventLoop::updateChannel(channel *ch)
{
    ep->updateChannel(ch);
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
void EventLoop::addInPoll(std::function<void()>* CallBack)
{
    thread_pool->append(CallBack);
}
