#pragma once

#include <sys/epoll.h>
#include <vector>
#include <unistd.h>
#include "channel.h"
#define MAX_EVENTS 1024

class channel;

class epoll
{
private:
    std::vector<epoll_event> evs; // 存放所有有事件发生的fd
    int epfd;

public:
    epoll();
    void updateChannel(channel *ch);
    void deleteChannel(channel *ch);
    std::vector<channel *> poll();
    ~epoll();
};
