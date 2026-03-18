#include "base/channel.h"
#include "base/EventLoop.h"

#include <sys/epoll.h>

channel::channel(EventLoop *_loop, int _fd, bool _useThreadPoll, bool _useET) : loop(_loop), fd(_fd), inepoll(false), events(0), revents(0), useThreadPoll(_useThreadPoll), useET(_useET)
{
}

int channel::getFd()
{
    return fd;
}

bool channel::getInepoll()
{
    return inepoll;
}

uint32_t channel::getEvents()
{
    return events;
}

EventLoop *channel::getEvLoop()
{
    return loop;
}

void channel::setInEpoll()
{
    inepoll = true;
}

void channel::enAbleToReading() // 把自己注册到epoll中
{
    // 说明自己想要进行读操作，后面会有别enAbleToXXX，此处会修改为别的
    if (useET)
    {
        events = EPOLLIN | EPOLLET;
    }
    else
    {
        events = EPOLLIN;
    }
    loop->updateChannel(this);
}

void channel::setRevents(uint32_t _revents)
{
    revents = _revents;
}
uint32_t channel::getRevents()
{
    return revents;
}
channel::~channel()
{
}
void channel::handleEventByCallBack()
{
    // std::function<void()> callback = CallBack; // 先拷贝回调，避免后续对象销毁导致悬空引用。
    CallBack(); // 此处不需要再进行addthreadpoll，因为此时loop中不再存在线程池，loop直接监听即可
}
void channel::setCallBack(std::function<void()> callback)
{
    this->CallBack = callback;
}
