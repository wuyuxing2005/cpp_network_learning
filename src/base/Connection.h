#pragma once
#include "EventLoop.h"
#include "mysocket.h"
#include "channel.h"
#include <cstring>
#include <iostream>
#include <memory>
#include <mutex>
#include "Buffer.h"
#include "http/HttpContext.h"
#define MAX_BUFFER_SIZE 1000
class channel;
class EventLoop;
class mysocket;
class Connection
    : public std::enable_shared_from_this<Connection>
{

private:
    EventLoop *loop;
    std::unique_ptr<mysocket> mysc;
    std::unique_ptr<channel> ch;
    std::unique_ptr<Buffer> readBuffer;
    std::unique_ptr<Buffer> sendBuffer;
    std::size_t send_offset_{0};
    bool close_after_write_{false};
    std::function<void(int)> deleteCallBack;
    std::function<void(Connection *)> functionCallBack;
    std::mutex conn_mtx;
    std::unique_ptr<HttpContext> http_context;

public:
    enum State
    {
        Invalid = 1,
        Connected,
        Closed,
        Failed,
    };
    Connection(EventLoop *_loop, std::unique_ptr<mysocket> _mysc);
    void registerCallBack();
    void setDeleteConnectionCallBack(std::function<void(int)> CallBack);
    // void echo();
    std::string getReadBuffer();
    State state_{State::Invalid};
    void setSendBuffer(std::string s);
    void recv0();
    void send0();
    void noBlockedRecv();
    bool noBlockedSend();
    void handleFunctionCallBack();
    void handleWriteCallBack();
    EventLoop *getLoop();
    void setFunctionCallBack(std::function<void(Connection *)> functionCallBack);
    void setCloseAfterWrite(bool close_after_write);
    void close0();
    void connectionDestructor();
    mysocket *getsocket();
    HttpContext *context();
    ~Connection();
};
