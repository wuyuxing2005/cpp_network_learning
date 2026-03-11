#pragma once
#include "EventLoop.h"
#include "channel.h"
#include "mysocket.h"
#include "sock_addr.h"
#include <iostream>
#include "set_noblocking.h"
#include <cstring>
#define MAX_BUFFER_SIZE 1024
/*
以注意到当前Server类已经与event检测，注册，分发完全分离了，只关注于服务器socket处理与服务器逻辑（echo client信息与new connection建立）

*/
class Server
{
private:
    EventLoop *loop;

public:
    Server(EventLoop *_loop);
    void start();
    void newConnection(mysocket *mysc);
    void handleReadingEvent(int client_fd);
    ~Server();
};
