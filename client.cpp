#include "mysocket.h"
#include <iostream>
#include <cstring>
#include "Buffer.h"
#include <sys/epoll.h>
#define MAX_BUFFER_SIZE 1024

int main()
{
    mysocket *socket = new mysocket();
    // 此处要注意 。最好还是改成阻塞，如果你出现了第一次client发送信息接收不到hello的问题，就是该非阻塞socket的问题，需要加sleep
    socket->setnonblocking();
    sock_addr *sockaddr = new sock_addr("127.0.0.1", 9999);
    socket->connect(sockaddr);
    std::string input;
    int epfd = epoll_create1(0);
    epoll_event ev, evs[1];
    ev.data.fd = socket->getFd();
    ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(epfd, EPOLL_CTL_ADD, socket->getFd(), &ev);
    while (getline(std::cin, input))
    {
        send(socket->getFd(), input.c_str(), input.size(), 0);
        epoll_wait(epfd, evs, 1, -1); // 一定要设置成-1；表示一直wait阻塞等待
        char buffer[MAX_BUFFER_SIZE];
        while (1)
        {
            memset(buffer, '\0', sizeof(buffer));
            ssize_t s = recv(socket->getFd(), buffer, sizeof(buffer), 0);
            if (s > 0)
            {
                std::cout << "Receive message from server " << buffer << std::endl;
            }
            else if (s == 0)
            {
                std::cout << "connection close" << std::endl;
                close(socket->getFd());
                return 0;
            }
            else if (s < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
            {
                break; // 读完了
            }
            else
            {
                strerror(errno);
                return 0;
            }
        }
    }
    close(socket->getFd());
    return 0;
}