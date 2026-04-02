#include "src/http/HttpServer.h"
#include "src/base/DebugLog.h"
#include <iostream>
#include <string>
#include <unistd.h>

const std::string html = " <font color=\"red\">This is html!</font> ";
void HttpResponseCallback(const HttpRequest &request, HttpResponse *response)
{
    if (request.method() != HttpRequest::Method::kGet)
    {
        response->SetStatusCode(HttpResponse::HttpStatusCode::k400BadRequest);
        response->SetStatusMessage("Bad Request");
        response->SetCloseConnection(true);
    }
    else
    {
        std::string url = request.url();
        if (url == "/")
        {
            response->SetStatusCode(HttpResponse::HttpStatusCode::k200K);
            response->SetBody(html);
            response->SetContentType("text/html");
        }
        else if (url == "/hello")
        {
            response->SetStatusCode(HttpResponse::HttpStatusCode::k200K);
            response->SetBody("Hello World!\n");
            response->SetContentType("text/plain");
        }
        else if (url == "/favicon.ico")
        {
            response->SetStatusCode(HttpResponse::HttpStatusCode::k200K);
        }
        else
        {
            response->SetStatusCode(HttpResponse::HttpStatusCode::k404NotFound);
            response->SetStatusMessage("Not Found");
            response->SetBody("Sorry Not Found\n");
            response->SetCloseConnection(true);
        }
    }
    return;
} // 需要放在main之前，因为会导致你bind的时候bind的是一个空的

namespace
{
bool ShouldEnableLog(int argc, char *argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "--log" || arg == "-l" || arg == "--log=1")
        {
            return true;
        }
        if (arg == "--no-log" || arg == "--log=0")
        {
            return false;
        }
    }
    return false;
}
}

int main(int argc, char *argv[])
{
    debuglog::SetEnabled(ShouldEnableLog(argc, argv));
    CPP_NETWORK_LOG << "[http-main] starting HTTP server pid=" << getpid() << '\n';
    HttpServer *Server = new HttpServer();
    Server->setHandleHttpServerCallBack(std::bind(&HttpResponseCallback, std::placeholders::_1, std::placeholders::_2));
    Server->start();
}
