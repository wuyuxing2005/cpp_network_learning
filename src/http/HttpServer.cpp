#include "HttpServer.h"
#include "base/DebugLog.h"

namespace
{
    std::string PreviewRequest(const std::string &buffer, std::size_t max_len = 120)
    {
        std::string preview;
        preview.reserve(max_len);
        for (char ch : buffer)
        {
            if (preview.size() >= max_len)
            {
                break;
            }
            if (ch == '\r')
            {
                preview += "\\r";
            }
            else if (ch == '\n')
            {
                preview += "\\n";
            }
            else
            {
                preview += ch;
            }
        }
        if (buffer.size() > max_len)
        {
            preview += "...";
        }
        return preview;
    }
}

HttpServer::HttpServer()
{
    loop = new EventLoop();
    server = new Server(loop);
    server->setConnect(std::bind(&HttpServer::HttpOnMessage, this, std::placeholders::_1));
}

HttpServer::~HttpServer()
{
}
void HttpServer::setHandleHttpServerCallBack(std::function<void(const HttpRequest &, HttpResponse *)> cb)
{
    make_response_callback_ = cb;
}
void HttpServer::start()
{
    server->start();
}
void HttpServer::HttpOnMessage(Connection *conn)
{
    conn->recv0();
    if (conn->state_ != Connection::State::Connected)
    {
        CPP_NETWORK_LOG << "[http-close] fd=" << conn->getsocket()->getFd()
                        << " reason=peer closed"
                        << " buffered_bytes=" << conn->getReadBuffer().size()
                        << '\n';
        conn->close0();
        return;
    }
    conn->UpdateTimeStamp(TimeStamp::getNowTime());
    ScheduleAutoClose(conn);

    HttpContext *ctx = conn->context();
    bool is_complete = ctx->ParaseRequest(conn->getReadBuffer().c_str(), conn->getReadBuffer().size());
    if (!is_complete)
    {
        CPP_NETWORK_LOG << "[http-close] fd=" << conn->getsocket()->getFd()
                        << " reason=bad request"
                        << " raw=\"" << PreviewRequest(conn->getReadBuffer()) << "\""
                        << '\n';
        conn->setSendBuffer("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->setCloseAfterWrite(true);
        conn->send0();
    }
    else
    {
        CPP_NETWORK_LOG << "[http-request] fd=" << conn->getsocket()->getFd()
                        << " method=" << ctx->request()->GetMethodString()
                        << " url=" << ctx->request()->url()
                        << '\n';
        OnRequest(conn, ctx->request());
    }
}
void HttpServer::OnRequest(Connection *conn, HttpRequest *request)
{
    HttpResponse response(false);
    make_response_callback_(*request, &response); // 创建response
    CPP_NETWORK_LOG << "[http-response] fd=" << conn->getsocket()->getFd()
                    << " method=" << request->GetMethodString()
                    << " url=" << request->url()
                    << " connection=" << (response.IsCloseConnection() ? "close" : "keep-alive")
                    << '\n';
    conn->setSendBuffer(response.message());
    conn->setCloseAfterWrite(response.IsCloseConnection());
    conn->send0();
    if (response.IsCloseConnection())
    {
        CPP_NETWORK_LOG << "[http-close] fd=" << conn->getsocket()->getFd()
                        << " reason=response requested close"
                        << " method=" << request->GetMethodString()
                        << " url=" << request->url()
                        << '\n';
        return;
    }
    conn->context()->ResetContextStatus();
}
void HttpServer::ScheduleAutoClose(Connection *conn)
{
    std::weak_ptr<Connection> weak_conn = conn->weak_from_this();
    std::function<void()> cb = std::bind(&HttpServer::ActiveCloseConn, this, weak_conn);
    conn->getLoop()->RunAfter(kAutoCloseTimeoutSeconds, cb);
}
void HttpServer::ActiveCloseConn(std::weak_ptr<Connection> weak_conn)
{
    std::shared_ptr<Connection> conn = weak_conn.lock();
    if (!conn || conn->state_ != Connection::State::Connected)
    {
        return;
    }

    TimeStamp expire_time = TimeStamp::AddTime(conn->timestamp(), kAutoCloseTimeoutSeconds);
    if (expire_time < TimeStamp::getNowTime())
    {
        CPP_NETWORK_LOG << "[http-close] fd=" << conn->getsocket()->getFd()
                        << " reason=idle timeout"
                        << " timeout=" << kAutoCloseTimeoutSeconds
                        << '\n';
        conn->close0();
    }
}
