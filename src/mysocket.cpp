#include "base/mysocket.h"
#include "base/errif.h"

mysocket::mysocket()
{
    fd = socket(AF_INET, SOCK_STREAM, 0);
    isBlocked = true;
}
mysocket::mysocket(int _fd) : fd(_fd)
{
    isBlocked = true;
}
int mysocket::bind(sock_addr *sc_addr)
{
    return ::bind(fd, (sockaddr *)&(sc_addr->getAddr()), sizeof(sc_addr->getAddr()));
}
int mysocket::listen()
{
    return ::listen(fd, SOMAXCONN);
}
int mysocket::accept(sock_addr *sc_addr)
{
    socklen_t len = sizeof(sc_addr->getAddr());
    return ::accept(fd, (sockaddr *)&sc_addr->getAddr(), &len);
}
int mysocket::connect(sock_addr *sc_addr)
{
    socklen_t len = sizeof(sc_addr->getAddr());
    return ::connect(fd, (sockaddr *)&sc_addr->getAddr(), len);
}
int mysocket::getFd()
{
    return this->fd;
}
bool mysocket::getIsBlocked()
{
    return isBlocked;
}
mysocket::~mysocket()
{
    if (fd >= 0)
    {
        close(fd);
    }
}
void mysocket::setBlock(bool _isblocked)
{
    isBlocked = _isblocked;
}
int mysocket::setnonblocking()
{
    this->isBlocked = false;
    int flags = fcntl(this->fd, F_GETFL);
    if (flags == -1)
    {
        return -1;
    }
    return fcntl(this->fd, F_SETFL, flags | O_NONBLOCK);
}
