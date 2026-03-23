#pragma once

#include "mysocket.h"
#include "sock_addr.h"
#include "EventLoop.h"
#include "channel.h"
#include <memory>
#include <functional>
class mysocket;
class sock_addr;
class EventLoop;
class channel;
class Server;
class Acceptor
{
private:
    EventLoop *loop;
    std::unique_ptr<mysocket> socket;
    std::unique_ptr<sock_addr> sc_addr;
    std::unique_ptr<channel> ch;
    std::function<void(std::unique_ptr<mysocket>)> CallBack;

public:
    Acceptor(EventLoop *_loop);
    ~Acceptor();
    void accpetNewConnection();
    void setConnectionCallBack(std::function<void(std::unique_ptr<mysocket>)> _CallBack);
};
