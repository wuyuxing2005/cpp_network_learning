#include "base/EventLoop.h"

EventLoop::EventLoop()
{
    ep = std::make_unique<epoll>();
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
    }
}
