#pragma once
#include "base/Server.h"
#include "HttpContext.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include <memory>

inline constexpr double kAutoCloseTimeoutSeconds = 100.0;

class HttpServer
{
private:
    Server *server;
    EventLoop *loop;
    std::function<void(const HttpRequest &, HttpResponse *)> make_response_callback_;

public:
    HttpServer();
    ~HttpServer();
    void setHandleHttpServerCallBack(std::function<void(const HttpRequest &, HttpResponse *)> cb);
    void start();
    void HttpOnMessage(Connection *conn);
    void OnRequest(Connection *conn, HttpRequest *request);
    void ScheduleAutoClose(Connection *conn);
    void ActiveCloseConn(std::weak_ptr<Connection> conn);
};
