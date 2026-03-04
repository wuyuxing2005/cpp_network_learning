#include "mysocket.h"
#include <iostream>
#include <cstring>
#define MAX_BUFFER_SIZE 1024
int main()
{
    mysocket *socket = new mysocket();
    setnonblocking(socket->getFd());
    sock_addr *sockaddr = new sock_addr("127.0.0.1", 9999);
    socket->connect(sockaddr);
    while (1)
    {
        std::string input;
        while (getline(std::cin, input))
        {
            send(socket->getFd(), input.c_str(), input.size(), 0);
            char buffer[MAX_BUFFER_SIZE];
            memset(buffer, 0, sizeof(buffer));
            ssize_t n = recv(socket->getFd(), buffer, sizeof(buffer) - 1, 0);
            if (n > 0)
            {
                buffer[n] = '\0';
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
    }
    close(socket->getFd());
    return 0;
}
