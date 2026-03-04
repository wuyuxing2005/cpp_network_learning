#pragma once
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include "errif.h"
inline in_addr_t shifen_to_network(const char *ip)
{
    in_addr_t addr;
    errif(inet_pton(AF_INET, ip, &addr) == -1, "inet_pton error");
    return addr;
}

inline std::string network_to_shifen(in_addr_t ip)
{
    struct in_addr addr{ip};
    char buf[INET_ADDRSTRLEN];
    errif(inet_ntop(AF_INET, &addr, buf, sizeof(buf)) == NULL, "inet_ntop error");
    return std::string(buf);
}