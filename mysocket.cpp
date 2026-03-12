#include "mysocket.h"
#include "errif.h"

mysocket::mysocket()
{
    fd = socket(AF_INET, SOCK_STREAM, 0);
    errif(fd == -1, "socket create error");
    // 此处不直接初始化非阻塞的原因是对于clientdua端
}
mysocket::mysocket(int _fd) : fd(_fd)
{
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
mysocket::~mysocket()
{
    if (fd >= 0)
    {
        close(fd);
    }
}

int mysocket::setnonblocking()
{
    int flags = fcntl(this->fd, F_GETFL);
    if (flags == -1)
    {
        return -1;
    }
    return fcntl(this->fd, F_SETFL, flags | O_NONBLOCK);
}
