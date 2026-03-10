#include "EventLoop.h"
#include "channel.h"
#include "epoll.h"

#include <vector>

EventLoop::EventLoop()
{
    ev = new epoll();
}

EventLoop::~EventLoop()
{
    delete ev;
}

void EventLoop::updateChannel(channel *ch)
{
    ev->updateChannel(ch);
}

void EventLoop::beginLoop()
{
    while (1)
    {
        std::vector<channel *> chs = ev->poll();
        for (channel *ch : chs)
        {
            ch->handleEventByCallBack();
        }
    }
}
