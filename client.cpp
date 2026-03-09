#include "mysocket.h"
#include <iostream>
#include <cstring>
#define MAX_BUFFER_SIZE 1024
void beginContact();
int main()
{
    mysocket *socket = new mysocket();
    // 此处要注意 。最好还是改成阻塞，如果你出现了
    // setnonblocking(socket->getFd());

    sock_addr *sockaddr = new sock_addr("127.0.0.1", 9999);
    socket->connect(sockaddr);

    std::string input;
    while (getline(std::cin, input))
    {
        send(socket->getFd(), input.c_str(), input.size(), 0);
        char buffer[MAX_BUFFER_SIZE];
        memset(buffer, '\0', sizeof(buffer));
        // sleep(1); 如果初始化socket为非阻塞，那么最好sleep一段时间，以便server正确回传hello
        ssize_t n = recv(socket->getFd(), buffer, sizeof(buffer) - 1, 0);
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            continue;
        }
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

    close(socket->getFd());
    return 0;
}
