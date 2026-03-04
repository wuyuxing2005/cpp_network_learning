#pragma once
#include <fcntl.h>

inline int setnonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL);
    if (flags == -1)
    {
        return -1;
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}
