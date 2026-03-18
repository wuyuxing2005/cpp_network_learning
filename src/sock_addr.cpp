#include "base/sock_addr.h"
sock_addr::sock_addr(const char *ip, uint16_t port) : port(port)
{
    this->ip = (char *)ip;
    this->sc_addr.sin_addr.s_addr = shifen_to_network(ip);
    this->sc_addr.sin_family = AF_INET;
    this->sc_addr.sin_port = htons(port);
}
sock_addr::sock_addr()
{
}
sock_addr::~sock_addr()
{
}
