#include "mysocket.h"
#include <iostream>
#include "set_noblocking.h"
#include <cstring>
#define MAX_BUFFER_SIZE 1024
int main()
{
    mysocket *socket = new mysocket();
    setnonblocking(socket->getFd());
    sock_addr *sc_addr = new sock_addr("127.0.0.1", 9999);
    socket->connect(sc_addr);
    std::string input;
    while (1)
    {
        while (getline(std::cin, input))
        {
            if (input == "quit" || "Quit" || "QUIT")
            {
                close(socket->getFd());
                return 0;
            }
            send(socket->getFd(), input.data(), input.size(), 0);
            char buffer[MAX_BUFFER_SIZE];
            buffer[MAX_BUFFER_SIZE - 1] = '\0';
            sleep(1);
            ssize_t s = recv(socket->getFd(), buffer, MAX_BUFFER_SIZE, 0);
            std::cout << "recv from server :" << buffer << std::endl;
            input.clear();
        }
    }
    close(socket->getFd());
    return 0;
}