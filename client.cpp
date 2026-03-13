#include "mysocket.h"
#include <iostream>
#include <cstring>
#define MAX_BUFFER_SIZE 1024

int main()
{
    mysocket *socket = new mysocket();
    // 此处要注意 。最好还是改成阻塞，如果你出现了第一次client发送信息接收不到hello的问题，就是该非阻塞socket的问题
    // socket->setnonblocking();
    sock_addr *sockaddr = new sock_addr("127.0.0.1", 9999);
    socket->connect(sockaddr);
    std::string input;
    while (getline(std::cin, input))
    {
        int s = send(socket->getFd(), input.c_str(), input.size(), 0);
        if (s == -1)
        {
            std::cout << "Nothing to send" << std::endl;
            break;
        }
        char buffer[MAX_BUFFER_SIZE];
        memset(buffer, '\0', sizeof(buffer));
        // sleep(1); 如果初始化socket为非阻塞，那么最好sleep一段时间，以便server正确回传hello
        // 否则你send之后立刻recv，第一次会recv不到东西，导致你的下一次send时，接着recv接收到的是上一次应该recv的信息，会出现错位，当最后一次你ctrl c时，此时连接socket客户端关闭，但是上一次的数据仍然想传进来，会导致报错erro Connection reset by peer
        ssize_t n = recv(socket->getFd(), buffer, sizeof(buffer) - 1, 0);
        if (n > 0)
        {
            std::cout << "Recieve from Server " << buffer << std::endl;
        }
        else if (n == 0)
        {
            std::cout << "Server closed connection" << std::endl;
            break;
        }
        else
        {
            perror("recv failed");
            break;
        }
    }
    delete socket;
    return 0;
}
