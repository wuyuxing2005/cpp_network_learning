#pragma once

#include <sys/epoll.h>

class epoll; // 前向声明，否则会出现双重依赖

class channel
{
private:
    epoll *ev;
    int fd;
    bool inepoll;
    uint32_t events;
    uint32_t revents;

public:
    channel(epoll *_ev, int _fd);
    int getFd();
    bool getInepoll();
    uint32_t getEvents();
    epoll *getEv();
    void setInEpoll();
    void enAbleToReading();
    void disbaleToReading();
    void setRevents(uint32_t _revents);
    uint32_t getRevents();
    ~channel();
};
