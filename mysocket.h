#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "sock_addr.h"
#include "set_noblocking.h"
class mysocket
{
private:
    int fd;

public:
    mysocket();
    int bind(sock_addr *sc_addr);
    int listen();
    int connect(sock_addr *sock_addr);
    int accept(sock_addr *sc_addr);
    int getFd();
    ~mysocket();
};
