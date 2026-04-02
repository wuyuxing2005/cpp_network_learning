#include "base/Connection.h"
#include "base/mysocket.h"
#include <getopt.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <cerrno>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

namespace
{

    constexpr const char *kHost = "127.0.0.1";
    constexpr uint16_t kPort = 9999;
    constexpr const char *kMessage = "I'm client!";

    void oneClient(int msgs, int wait_seconds)
    {
        std::unique_ptr<mysocket> socket = std::make_unique<mysocket>();
        std::unique_ptr<sock_addr> addr = std::make_unique<sock_addr>(kHost, kPort);

        if (socket->connect(addr.get()) == -1)
        {
            std::cout << "connect error: " << strerror(errno) << '\n';
            return;
        }

        if (socket->setnonblocking() == -1)
        {
            std::cout << "setnonblocking error: " << strerror(errno) << '\n';
            return;
        }

        if (wait_seconds > 0)
        {
            sleep(wait_seconds);
        }

        const std::string message = kMessage;
        std::unique_ptr<Connection> conn = std::make_unique<Connection>(nullptr, std::move(socket));
        conn->state_ = Connection::State::Connected;

        int epfd = epoll_create1(0);
        if (epfd == -1)
        {
            std::cout << "epoll_create1 error: " << strerror(errno) << '\n';
            return;
        }

        int sockfd = conn->getsocket()->getFd();
        epoll_event ev{};
        epoll_event evs[1]{};
        ev.data.fd = sockfd;
        ev.events = EPOLLIN | EPOLLET;
        if (epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev) == -1)
        {
            std::cout << "epoll_ctl error: " << strerror(errno) << '\n';
            close(epfd);
            return;
        }

        for (int count = 0; count < msgs; ++count)
        {
            conn->setSendBuffer(message);
            conn->send0();
            if (conn->state_ != Connection::State::Connected)
            {
                std::cout << "error connection" << '\n';
                break;
            }

            while (true)
            {
                int ready = epoll_wait(epfd, evs, 1, -1);
                if (ready == -1)
                {
                    if (errno == EINTR)
                    {
                        continue;
                    }
                    std::cout << "epoll_wait error: " << strerror(errno) << '\n';
                    conn->state_ = Connection::State::Failed;
                    break;
                }
                if (ready == 0)
                {
                    continue;
                }

                conn->recv0();
                if (conn->state_ == Connection::State::Closed)
                {
                    std::cout << "server disconnected!" << '\n';
                    close(epfd);
                    return;
                }
                if (conn->state_ == Connection::State::Failed)
                {
                    std::cout << "socket read error" << '\n';
                    close(epfd);
                    return;
                }

                if (static_cast<int>(conn->getReadBuffer().size()) >= static_cast<int>(message.size()))
                {
                    break;
                }
            }

            if (conn->state_ != Connection::State::Connected)
            {
                break;
            }

            std::cout << "count: " << count << ", message from server: " << conn->getReadBuffer() << '\n';
        }

        close(epfd);
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
            std::cerr << "usage: ./test [-t threads] [-m messages] [-w wait_seconds]" << '\n';
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
