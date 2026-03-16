#pragma once
#include <cstdint>
#include <functional>

class EventLoop;

class channel // 实际上封装的是注册到epoll的ev
{
private:
    EventLoop *loop;
    int fd;
    bool inepoll;
    std::function<void()> CallBack; // 此处回调函数就是该channel对应的epoll（在EventLoop中）处理事件应该进行的操作
    // 之所以要使用的原因就是socket包括了监听socket与连接socket，server端的监听socket需要监听连接，连接socket则承担着send与recv的操作。在之前的Socket类仅仅是对于套接字fd，地址的封装，
    // 但是不包括不同Sokcet的本身的不同操作（指的是不同socket的操作，即回调函数内容）。这样可以做到更好的抽象
    uint32_t events;
    uint32_t revents;

public:
    channel(EventLoop *_loop, int _fd);
    int getFd();
    bool getInepoll();
    uint32_t getEvents();
    EventLoop *getEvLoop();
    void setInEpoll();
    void enAbleToReading();
    void disbaleToReading();
    void setRevents(uint32_t _revents);
    void setCallBack(std::function<void()> CallBack);
    void handleEventByCallBack();
    uint32_t getRevents();
    ~channel();
};
