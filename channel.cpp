#include "channel.h"

#include "epoll.h"

channel::channel(epoll *_ev, int _fd) : ev(_ev), fd(_fd), inepoll(false), events(0), revents(0)
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

epoll *channel::getEv()
{
    return ev;
}

void channel::setInEpoll()
{
    inepoll = true;
}

void channel::enAbleToReading() // 把自己注册到epoll中
{
    events = EPOLLIN | EPOLLET; // 说明自己想要进行读操作，后面会有别enAbleToXXX，此处会修改为别的
    ev->updateChannel(this);
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
