#include "HttpResponse.h"
#include <utility>

namespace
{
std::string DefaultStatusMessage(HttpResponse::HttpStatusCode status_code)
{
    switch (status_code)
    {
    case HttpResponse::HttpStatusCode::k100Continue:
        return "Continue";
    case HttpResponse::HttpStatusCode::k200K:
        return "OK";
    case HttpResponse::HttpStatusCode::k400BadRequest:
        return "Bad Request";
    case HttpResponse::HttpStatusCode::k403Forbidden:
        return "Forbidden";
    case HttpResponse::HttpStatusCode::k404NotFound:
        return "Not Found";
    case HttpResponse::HttpStatusCode::k500internalServerError:
        return "Internal Server Error";
    default:
        return "Unknown";
    }
}
}

HttpResponse::HttpResponse(bool close_connection)
{
    this->close_connection_ = close_connection;
}
HttpResponse::~HttpResponse()
{
}

void HttpResponse::SetBody(const std::string &body)
{
    this->body_ = body;
}
void HttpResponse::SetCloseConnection(bool close_connection)
{
    this->close_connection_ = close_connection;
}
void HttpResponse::SetStatusCode(HttpStatusCode status_code)
{
    this->status_code_ = status_code;
}

void HttpResponse::AddHeader(const std::string &key, const std::string &value)
{
    headers_[key] = value;
}
void HttpResponse::SetContentType(const std::string &content_type)
{
    AddHeader("Content-Type", content_type);
}
void HttpResponse::SetStatusMessage(const std::string &status_message)
{
    this->status_message_ = status_message;
}
std::string HttpResponse::message()
{
    if (status_message_.empty())
    {
        status_message_ = DefaultStatusMessage(status_code_);
    }
    if (!headers_.count("Content-Length"))
    {
        headers_["Content-Length"] = std::to_string(body_.size());
    }
    headers_["Connection"] = close_connection_ ? "close" : "keep-alive";

    std::string message;
    message += ("HTTP/1.1 " +
                std::to_string(status_code_) + " " +
                status_message_ + "\r\n");
    for (auto &p : headers_)
    {
        message += (p.first + ':' + ' ' + p.second + "\r\n");
    }
    message += "\r\n";
    message += body_;
    return message;
}
bool HttpResponse::IsCloseConnection()
{
    return close_connection_;
}
