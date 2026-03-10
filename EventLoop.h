#pragma once
class epoll;
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
};
