#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "sock_addr.h"
#include <fcntl.h>
class mysocket
{
private:
    int fd;
    bool isBlocked;

public:
    mysocket();
    mysocket(int _fd);
    int bind(sock_addr *sc_addr);
    int listen(int backlog = 16384);
    int connect(sock_addr *sock_addr);
    int accept(sock_addr *sc_addr);
    int getFd();
    int setnonblocking();
    void setBlock(bool _isBlocked);
    bool getIsBlocked();
    ~mysocket();
};
