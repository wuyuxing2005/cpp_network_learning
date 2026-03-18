#include "base/epoll.h"

epoll::epoll()
{
    evs.resize(MAX_EVENTS);
    epfd = epoll_create1(0);
}
// void epoll::epoll_add(int sock_fd, uint32_t events)
// {
//     epoll_event ev;
//     ev.data.fd = sock_fd;
//     ev.events = events;
//     epoll_ctl(epfd, EPOLL_CTL_ADD, sock_fd, &ev);
// }
// void epoll::epoll_del(int sock_fd, uint32_t events)
// {
//     epoll_event ev;
//     ev.data.fd = sock_fd;
//     ev.events = events;
//     epoll_ctl(epfd, EPOLL_CTL_DEL, sock_fd, &ev);
// }
// void epoll::epoll_mod(int sock_fd, uint32_t events)
// {
//     epoll_event ev;
//     ev.data.fd = sock_fd;
//     ev.events = events;
//     epoll_ctl(epfd, EPOLL_CTL_MOD, sock_fd, &ev);
// }
std::vector<channel *> epoll::poll() // 获得当前所有有事件发生的文件描述符
{
    std::vector<channel *> chs;
    int nfds = epoll_wait(epfd, evs.data(), MAX_EVENTS, -1);
    if (nfds <= 0)
    {
        return chs;
    }
    for (int i = 0; i < nfds; i += 1)
    {
        channel *ch = (channel *)evs[i].data.ptr;
        ch->setRevents(evs[i].events);
        chs.push_back(ch);
    }
    return chs;
}

void epoll::updateChannel(channel *channel)
{
    epoll_event ev;
    ev.data.ptr = channel;
    ev.events = channel->getEvents();
    if (!channel->getInepoll())
    {
        epoll_ctl(epfd, EPOLL_CTL_ADD, channel->getFd(), &ev);
        channel->setInEpoll();
    }
    else
    {
        epoll_ctl(epfd, EPOLL_CTL_MOD, channel->getFd(), &ev);
    }
    return;
}

void epoll::deleteChannel(channel *channel)
{
    if (channel == nullptr || !channel->getInepoll())
    {
        return;
    }
    epoll_ctl(epfd, EPOLL_CTL_DEL, channel->getFd(), nullptr);
}

epoll::~epoll()
{
    close(epfd);
}
