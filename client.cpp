#pragma once
#include "base/mysocket.h"
#include <iostream>
#include <cstring>
#include "base/Buffer.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <memory>
#define MAX_BUFFER_SIZE 10
// 为clientIO阻塞版本
int main()
{
    mysocket *socket = new mysocket();
    sock_addr *sockaddr = new sock_addr("127.0.0.1", 9999);
    socket->connect(sockaddr);
    Buffer *sendbuffer = new Buffer(); // 仅仅用于写入你想发送给server的信息
    Buffer *readbuffer = new Buffer(); // 仅仅用于你想接收的从server的数据
    while (sendbuffer->inputLine())
    {
        send(socket->getFd(), sendbuffer->getChar_c(), sendbuffer->getSize(), 0);
        char buffer[MAX_BUFFER_SIZE];
        ssize_t byte_read = 0;
        while (1)
        {
            memset(buffer, '\0', sizeof(buffer));
            ssize_t s = recv(socket->getFd(), buffer, sizeof(buffer), 0);
            if (s > 0)
            {
                readbuffer->append(buffer, s);
                byte_read += s;
            }
            else if (s == 0)
            {
                std::cout << "connection close" << std::endl;
                close(socket->getFd());
                delete readbuffer;
                delete sendbuffer;
                return 0;
            }
            else
            {
                strerror(errno);
                return 0;
            }
            if (byte_read >= sendbuffer->getSize())
            {
                std::cout << "Recv from Server : " << readbuffer->getString() << std::endl;
                readbuffer->clear_s(); // 清除数据
                sendbuffer->clear_s(); // 清除数据
                break;
            }
        }
    }
    close(socket->getFd());
    return 0;
}
