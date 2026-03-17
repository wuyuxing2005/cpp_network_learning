#include "mysocket.h"
#include "sock_addr.h"

#include <getopt.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

namespace {

constexpr const char *kHost = "127.0.0.1";
constexpr uint16_t kPort = 9999;
constexpr const char *kMessage = "I'm client!";

void oneClient(int msgs, int wait_seconds)
{
    mysocket socket;
    sock_addr addr(kHost, kPort);

    if (socket.connect(&addr) == -1)
    {
        perror("socket connect error");
        return;
    }

    if (wait_seconds > 0)
    {
        sleep(wait_seconds);
    }

    const std::string message = kMessage;
    std::string read_buffer;
    read_buffer.reserve(message.size());

    for (int count = 0; count < msgs; ++count)
    {
        ssize_t write_bytes = send(socket.getFd(), message.data(), message.size(), 0);
        if (write_bytes == -1)
        {
            perror("socket write error");
            break;
        }

        read_buffer.clear();
        while (static_cast<int>(read_buffer.size()) < static_cast<int>(message.size()))
        {
            char buf[1024];
            ssize_t read_bytes = recv(socket.getFd(), buf, sizeof(buf), 0);
            if (read_bytes > 0)
            {
                read_buffer.append(buf, read_bytes);
                continue;
            }
            if (read_bytes == 0)
            {
                std::cout << "server disconnected!" << std::endl;
                return;
            }
            if (errno == EINTR)
            {
                continue;
            }
            perror("socket read error");
            return;
        }

        std::cout << "count: " << count << ", message from server: " << read_buffer << std::endl;
    }
}

} // namespace

int main(int argc, char *argv[])
{
    int threads = 100;
    int msgs = 100;
    int wait = 0;

    int opt;
    while ((opt = getopt(argc, argv, "t:m:w:")) != -1)
    {
        switch (opt)
        {
        case 't':
            threads = std::stoi(optarg);
            break;
        case 'm':
            msgs = std::stoi(optarg);
            break;
        case 'w':
            wait = std::stoi(optarg);
            break;
        default:
            std::cerr << "usage: ./test [-t threads] [-m messages] [-w wait_seconds]" << std::endl;
            return 1;
        }
    }

    std::vector<std::thread> workers;
    workers.reserve(threads);
    for (int i = 0; i < threads; ++i)
    {
        workers.emplace_back(oneClient, msgs, wait);
    }

    for (std::thread &worker : workers)
    {
        if (worker.joinable())
        {
            worker.join();
        }
    }

    return 0;
}
