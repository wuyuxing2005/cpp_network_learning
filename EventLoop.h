#include "epoll.h"
class channel;
class EventLoop
{
private:
    epoll *ev;

public:
    EventLoop();
    ~EventLoop();
    void beginLoop();
    void updateChannel(channel *ch);
    void newConnect();
};
EventLoop::EventLoop()
{
    ev = new epoll();
}
EventLoop::~EventLoop()
{
}

void EventLoop::updateChannel(channel *ch)
{
    ev->updateChannel(ch);
}
void EventLoop::beginLoop()
{
    std::vector<channel *> chs = ev->poll();
    for (channel *ch : chs)
    {
        ch->handleEventByCallBack();
    }
}
