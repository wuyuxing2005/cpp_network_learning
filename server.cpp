#include <cerrno>
#include <cstring>
#include <iostream>
#include <unordered_map>

#include "epoll.h"
#include "errif.h"
#include "mysocket.h"
#define MAX_BUFFER_SIZE 1024
std::unordered_map<int, uint16_t> ports;
void handleReadEvent(int fd);
int main()
{
    epoll my_epoll;

    mysocket server_socket;
    sock_addr server_addr("127.0.0.1", 9999);

    errif(setnonblocking(server_socket.getFd()) == -1, "setnonblocking listen fd");
    errif(server_socket.bind(&server_addr) == -1, "bind");
    errif(server_socket.listen() == -1, "listen");

    int socket_fd = server_socket.getFd();
    channel *ch = new channel(&my_epoll, socket_fd);
    ch->enAbleToReading();
    std::cout << "Server listening on 127.0.0.1:9999" << std::endl;
    while (1)
    {
        std::vector<channel *> channels = my_epoll.poll();
        for (int i = 0; i < channels.size(); i += 1)
        {
            if (channels[i]->getFd() == socket_fd)
            {
                sock_addr sc_addr;
                int client_fd = server_socket.accept(&sc_addr);
                setnonblocking(client_fd);
                channel *client_ch = new channel(&my_epoll, client_fd);
                client_ch->enAbleToReading();
                uint16_t port = ntohs(sc_addr.getAddr().sin_port);
                ports[client_fd] = port;
                std::cout << "Client connected. ip: " << network_to_shifen(sc_addr.getAddr().sin_addr.s_addr) << " port: " << port << std::endl;
            }
            else if (channels[i]->getRevents() & EPOLLIN)
            {
                handleReadEvent(channels[i]->getFd());
            }
        }
    }
    delete ch;
    return 0;
}
void handleReadEvent(int fd)
{
    char buffer[MAX_BUFFER_SIZE];
    while (1)
    {
        memset(buffer, '\0', sizeof(buffer));
        int s = recv(fd, buffer, MAX_BUFFER_SIZE, 0);
        if (s == 0)
        {
            std::cout << "client " << ports[fd] << " quit" << std::endl;
            close(fd);
            break;
        }
        else if (s > 0)
        {
            std::cout << "server recv : " << buffer << " from " << ports[fd] << std::endl;
            memset(buffer, '\0', sizeof(buffer));
            const char message[] = "Hello!";
            send(fd, message, strlen(message), 0);
        }
        else if (s == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        {
            break;
        }
        else if (s == -1 && (errno == EINTR))
        {
            continue;
        }
        else
        {
            break;
        }
    }
}
