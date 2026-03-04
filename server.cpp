#include <cerrno>
#include <cstring>
#include <iostream>
#include <unordered_map>

#include "epoll.h"
#include "errif.h"
#include "mysocket.h"

#define MAX_BUFFER_SIZE 1024

int main()
{
    epoll my_epoll;
    std::unordered_map<int, uint16_t> ports;
    mysocket server_socket;
    sock_addr server_addr("127.0.0.1", 9999);

    errif(setnonblocking(server_socket.getFd()) == -1, "setnonblocking listen fd");
    errif(server_socket.bind(&server_addr) == -1, "bind");
    errif(server_socket.listen() == -1, "listen");

    int socket_fd = server_socket.getFd();
    my_epoll.epoll_add(socket_fd, EPOLLIN | EPOLLET);
    std::cout << "Server listening on 127.0.0.1:9999" << std::endl;

    while (1)
    {
        int eplen = my_epoll.wait();
        if (eplen == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }
            perror("epoll_wait");
            break;
        }

        std::vector<epoll_event> events = my_epoll.getEvnets();
        for (int i = 0; i < eplen; i += 1)
        {
            int fd = events[i].data.fd;
            if (fd == socket_fd)
            {
                while (1)
                {
                    sock_addr client_sc_addr;
                    int client_fd = server_socket.accept(&client_sc_addr);
                    if (client_fd == -1)
                    {
                        if (errno == EAGAIN || errno == EWOULDBLOCK)
                        {
                            break;
                        }
                        if (errno == EINTR)
                        {
                            continue;
                        }
                        perror("accept");
                        break;
                    }

                    if (setnonblocking(client_fd) == -1)
                    {
                        perror("setnonblocking client fd");
                        close(client_fd);
                        continue;
                    }

                    my_epoll.epoll_add(client_fd, EPOLLIN | EPOLLET);
                    uint16_t port = ntohs(client_sc_addr.getAddr().sin_port);
                    ports[client_fd] = port;
                    std::cout << "Client connected. ip: " << network_to_shifen(client_sc_addr.getAddr().sin_addr.s_addr) << " port: " << port << std::endl;
                }
            }
            else
            {
                char buffer[MAX_BUFFER_SIZE];
                while (1)
                {
                    memset(buffer, 0, sizeof(buffer));
                    ssize_t s = recv(fd, buffer, sizeof(buffer) - 1, 0);
                    if (s == 0)
                    {
                        my_epoll.epoll_del(fd, EPOLLIN | EPOLLET);
                        close(fd);
                        ports.erase(fd);
                        std::cout << "Client quit" << std::endl;
                        break;
                    }
                    else if (s > 0)
                    {
                        buffer[s] = '\0';
                        std::cout << "Server receive: " << buffer << " from Client: " << ports[fd] << std::endl;
                        const char message[] = "Hello!";
                        send(fd, message, strlen(message), 0);
                    }
                    else if (errno == EAGAIN || errno == EWOULDBLOCK)
                    {
                        break;
                    }
                    else if (errno == EINTR)
                    {
                        continue;
                    }
                    else
                    {
                        perror("recv");
                        my_epoll.epoll_del(fd, EPOLLIN | EPOLLET);
                        close(fd);
                        ports.erase(fd);
                        break;
                    }
                }
            }
        }
    }
}
