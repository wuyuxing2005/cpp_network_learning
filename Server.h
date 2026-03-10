#pragma once
#include "EventLoop.h"
#include "channel.h"
#include "mysocket.h"
#include "sock_addr.h"
#include <iostream>
#include "set_noblocking.h"
#include <cstring>
#define MAX_BUFFER_SIZE 1024
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
