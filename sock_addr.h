#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include "ip_trans.h"
class sock_addr
{
private:
    char *ip;
    uint16_t port;
    sockaddr_in sc_addr;

public:
    sock_addr(const char *ip, uint16_t port);
    sock_addr();
    sockaddr_in &getAddr()
    {
        return sc_addr;
    }
    ~sock_addr();
};
