#include "base/mysocket.h"
#include <iostream>
#include <cstring>
#include <sys/epoll.h>
#include <unistd.h>
#include <memory>
#include "base/Connection.h"
#define MAX_BUFFER_SIZE 10
// 为clientIO阻塞版本
int main()
{
    std::unique_ptr<mysocket> socket = std::make_unique<mysocket>();
    std::unique_ptr<sock_addr> sockaddr = std::make_unique<sock_addr>("127.0.0.1", 9999);
    socket->connect(sockaddr.get());
    std::unique_ptr<Connection> conn = std::make_unique<Connection>(nullptr, std::move(socket));
    std::unique_ptr<Buffer> inputbuffer = std::make_unique<Buffer>();
    conn->state_ = Connection::State::Connected; // 预设为Connected
    while (inputbuffer->inputLine())
    {
        conn->setSendBuffer(inputbuffer->getString());
        conn->send0();
        if (conn->state_ == Connection::State::Connected)
        {
            conn->recv0();
            std::cout << "Client recv :" << conn->getReadBuffer() << '\n';
        }
        else
        {
            std::cout << "error connection" << '\n';
            break;
        }
    }
    return 0;
}
