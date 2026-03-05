#include "epoll.h"
epoll::epoll()
{
    ev_fds.resize(MAX_EVENTS);
    epfd = epoll_create1(0);
}
void epoll::epoll_add(int sock_fd, uint32_t events)
{
    epoll_event ev;
    ev.data.fd = sock_fd;
    ev.events = events;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sock_fd, &ev);
}
void epoll::epoll_del(int sock_fd, uint32_t events)
{
    epoll_event ev;
    ev.data.fd = sock_fd;
    ev.events = events;
    epoll_ctl(epfd, EPOLL_CTL_DEL, sock_fd, &ev);
}
void epoll::epoll_mod(int sock_fd, uint32_t events)
{
    epoll_event ev;
    ev.data.fd = sock_fd;
    ev.events = events;
    epoll_ctl(epfd, EPOLL_CTL_MOD, sock_fd, &ev);
}
std::vector<epoll_event> epoll::poll()
{
    epoll_wait(epfd, ev_fds.data(), MAX_EVENTS, -1);
    return this->ev_fds;
}
epoll::~epoll()
{
}