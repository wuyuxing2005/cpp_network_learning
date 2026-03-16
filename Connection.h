#pragma once
#include "EventLoop.h"
#include "mysocket.h"
#include "channel.h"
#include <cstring>
#include <iostream>
#include "Buffer.h"
#define MAX_BUFFER_SIZE 10
class channel;
class EventLoop;
class mysocket;
class Connection
{
private:
    EventLoop *loop;
    mysocket *mysc;
    channel *ch;
    std::function<void(mysocket *mysc)> deleteCallBack;
    Buffer *readBuffer;

public:
    Connection(EventLoop *_loop, mysocket *_mysc);
    void setDeleteConnectionCallBack(std::function<void(mysocket *mysc)> CallBack);
    void echo();
    ~Connection();
};
