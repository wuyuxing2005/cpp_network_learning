#pragma once
#include "mysocket.h"
#include "sock_addr.h"
#include "EventLoop.h"
#include "channel.h"
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
    mysocket *socket;
    sock_addr *sc_addr;
    channel *ch;
    std::function<void(mysocket *sc)> CallBack;

public:
    Acceptor(EventLoop *_loop);
    ~Acceptor();
    void accpetNewConnection();
    void setConnectionCallBack(std::function<void(mysocket *sc)> _CallBack);
};
