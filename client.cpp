#include "base/Connection.h"
#include "base/mysocket.h"
#include <iostream>
#include <cstring>
#include <cerrno>
#include "base/Buffer.h"
#include <sys/epoll.h>
#include <unistd.h>
#define MAX_BUFFER_SIZE 1000
// 为IO非阻塞版本
int main()
{
    std::unique_ptr<mysocket> socket = std::make_unique<mysocket>();
    std::unique_ptr<sock_addr> sockaddr = std::make_unique<sock_addr>("127.0.0.1", 9999);
    if (socket->connect(sockaddr.get()) == -1)
    {
        std::cout << "connect error: " << strerror(errno) << '\n';
        return 1;
    }
    if (socket->setnonblocking() == -1)
    {
        std::cout << "setnonblocking error: " << strerror(errno) << '\n';
        return 1;
    }

    std::unique_ptr<Connection> conn = std::make_unique<Connection>(nullptr, std::move(socket));
    conn->state_ = Connection::State::Connected;
    Buffer sendbuffer; // 仅仅用于写入你想发送给server的信息
    int epfd = epoll_create1(0);
    if (epfd == -1)
    {
        std::cout << "epoll_create1 error: " << strerror(errno) << '\n';
        return 1;
    }

    int sockfd = conn->getsocket()->getFd();
    epoll_event ev, evs[1];
    ev.data.fd = sockfd;
    ev.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev) == -1)
    {
        std::cout << "epoll_ctl error: " << strerror(errno) << '\n';
        close(epfd);
        return 1;
    }

    while (sendbuffer.inputLine())
    {
        conn->setSendBuffer(sendbuffer.getString());
        conn->send0();
        if (conn->state_ != Connection::State::Connected)
        {
            std::cout << "error connection" << '\n';
            break;
        }
        epoll_wait(epfd, evs, 1, -1); // 一定要设置成-1；表示一直wait阻塞等待
        conn->recv0();
        std::cout << "Client recv from Server :" << conn->getReadBuffer() << '\n';
    }
    close(epfd);
    return 0;
}
