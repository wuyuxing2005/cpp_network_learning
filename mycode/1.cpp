#include <netdb.h>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>

int main()
{
    auto p = getservbyname("https", "tcp");
    if (!p)
    {
        std::cerr << "getservbyname failed\n";
        return 1;
    }

    auto k = gethostbyname("localhost");
    if (!k)
    {
        std::cerr << "gethostbyname failed\n";
        return 1;
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = p->s_port;
    address.sin_addr = *(in_addr *)k->h_addr_list[0];

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        return 1;
    }

    if (connect(sockfd, (sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("connect");
        close(sockfd);
        return 1;
    }

    char buffer[1024];
    ssize_t n = read(sockfd, buffer, sizeof(buffer) - 1);
    if (n < 0)
    {
        perror("read");
        close(sockfd);
        return 1;
    }
    buffer[n] = '\0';

    std::cout << buffer << std::endl;
    close(sockfd);
}
