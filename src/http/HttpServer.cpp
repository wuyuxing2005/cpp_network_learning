#include "HttpServer.h"

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
        std::cout << "[http-close] fd=" << conn->getsocket()->getFd()
                  << " reason=peer closed"
                  << " buffered_bytes=" << conn->getReadBuffer().size()
                  << '\n';
        conn->close0();
        return;
    }
    HttpContext *ctx = conn->context();
    bool is_complete = ctx->ParaseRequest(conn->getReadBuffer().c_str(), conn->getReadBuffer().size());
    if (!is_complete)
    {
        std::cout << "[http-close] fd=" << conn->getsocket()->getFd()
                  << " reason=bad request"
                  << " raw=\"" << PreviewRequest(conn->getReadBuffer()) << "\""
                  << '\n';
        conn->setSendBuffer("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->send0();
        conn->close0();
    }
    else
    {
        std::cout << "[http-request] fd=" << conn->getsocket()->getFd()
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
    std::cout << "[http-response] fd=" << conn->getsocket()->getFd()
              << " method=" << request->GetMethodString()
              << " url=" << request->url()
              << " connection=" << (response.IsCloseConnection() ? "close" : "keep-alive")
              << '\n';
    conn->setSendBuffer(response.message());
    conn->send0();
    if (response.IsCloseConnection())
    {
        std::cout << "[http-close] fd=" << conn->getsocket()->getFd()
                  << " reason=response requested close"
                  << " method=" << request->GetMethodString()
                  << " url=" << request->url()
                  << '\n';
        conn->close0();
        return;
    }
    conn->context()->ResetContextStatus();
}
