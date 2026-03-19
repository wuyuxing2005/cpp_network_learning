#pragma once
#include "EventLoop.h"
#include "mysocket.h"
#include "channel.h"
#include <cstring>
#include <iostream>
#include <memory>
#include <mutex>
#include "Buffer.h"
#define MAX_BUFFER_SIZE 10
class channel;
class EventLoop;
class mysocket;
class Connection
    : public std::enable_shared_from_this<Connection>
{

private:
    EventLoop *loop;
    mysocket *mysc;
    channel *ch;

    std::function<void(int)> deleteCallBack;
    std::function<void(Connection *)> functionCallBack;
    Buffer *readBuffer;
    Buffer *sendBuffer;
    std::mutex conn_mtx;

public:
    enum State
    {
        Invalid = 1,
        Connected,
        Closed,
        Failed,
    };
    Connection(EventLoop *_loop, mysocket *_mysc);
    void registerCallBack();
    void setDeleteConnectionCallBack(std::function<void(int)> CallBack);
    // void echo();
    std::string getReadBuffer();
    State state_{State::Invalid};
    void setSendBuffer(std::string s);
    void recv0();
    void send0();
    void handleFunctionCallBack();
    void setFunctionCallBack(std::function<void(Connection *)> functionCallBack);
    void close0();
    mysocket *getsocket();
    ~Connection();
};
