#include "base/channel.h"
#include "base/EventLoop.h"

#include <sys/epoll.h>

channel::channel(EventLoop *_loop, int _fd, bool _useThreadPoll, bool _useET) : loop(_loop), fd(_fd), inepoll(false), events(0), revents(0), useThreadPoll(_useThreadPoll), useET(_useET)
{
    tied_ = false;
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
    if (tied_) // 如果绑定了所对应的Connection，或者说需要注意其对应的Connection是否会提前delete
    {
        std::shared_ptr<void> guard = tie_.lock();
        if (guard) // 只有在handleevent之后Connection才会释放
        {
            CallBack();
        }
    }
    else
    {
        CallBack();
    }
} // 函数结束，guard指针被析构，相对应的connection被析构
void channel::setCallBack(std::function<void()> callback)
{
    this->CallBack = callback;
}

void channel::Tie(const std::shared_ptr<void> &owner)
{
    tie_ = owner;
    tied_ = true;
}