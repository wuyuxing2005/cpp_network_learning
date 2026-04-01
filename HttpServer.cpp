#include "src/http/HttpServer.h"
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
}//需要放在main之前，因为会导致你bind的时候bind的是一个空的
int main()
{
    HttpServer *Server = new HttpServer();
    Server->setHandleHttpServerCallBack(std::bind(&HttpResponseCallback, std::placeholders::_1, std::placeholders::_2));
    Server->start();
}

// const std::string html = " <font color=\"red\">This is html!</font> ";
// void HttpResponseCallback(const HttpRequest &request, HttpResponse *response)
// {
//     if (request.method() != HttpRequest::Method::kGet)
//     {
//         response->SetStatusCode(HttpResponse::HttpStatusCode::k400BadRequest);
//         response->SetStatusMessage("Bad Request");
//         response->SetCloseConnection(true);
//     }

//     {
//         std::string url = request.url();
//         if (url == "/")
//         {
//             response->SetStatusCode(HttpResponse::HttpStatusCode::k200K);
//             response->SetBody(html);
//             response->SetContentType("text/html");
//         }
//         else if (url == "/hello")
//         {
//             response->SetStatusCode(HttpResponse::HttpStatusCode::k200K);
//             response->SetBody("hello world\n");
//             response->SetContentType("text/plain");
//         }
//         else if (url == "/favicon.ico")
//         {
//             response->SetStatusCode(HttpResponse::HttpStatusCode::k200K);
//         }
//         else
//         {
//             response->SetStatusCode(HttpResponse::HttpStatusCode::k404NotFound);
//             response->SetStatusMessage("Not Found");
//             response->SetBody("Sorry Not Found\n");
//             response->SetCloseConnection(true);
//         }
//     }
//     return;
// }