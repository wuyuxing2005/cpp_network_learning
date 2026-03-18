#pragma once
#include "EventLoop.h"
#include "channel.h"
#include "mysocket.h"
#include "sock_addr.h"
#include <iostream>
#include "set_noblocking.h"
#include <cstring>
#include <unordered_map>
#include <memory>
#include <mutex>
#include "Acceptor.h"
#include "Connection.h"
#include "Thread_Poll/ThreadPoll.h"
/*
注意到当前Server类已经与event检测，注册，分发完全分离了，只关注于服务器socket处理与服务器逻辑（echo client信息与new connection建立）
*/
class Connection;
class Server
{
private:
    EventLoop *MainReactor;
    Acceptor *apt;
    std::unordered_map<int, std::shared_ptr<Connection>> connections;
    std::vector<EventLoop *> subReactors;
    std::mutex connections_mtx;
    Thread_pool<std::function<void()>> *thread_pool;
    std::function<void(Connection *)> Connect_Callback;

public:
    Server(EventLoop *_loop);
    void start();
    void newConnection(mysocket *mysc);
    void deleteConnection(int fd);
    void setConnect(std::function<void(Connection *)> _Connect_Callback);
    ~Server();
};
