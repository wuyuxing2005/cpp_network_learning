#include <sys/epoll.h>
#include <vector>
#include <fcntl.h>
#define MAX_EVENTS 1024
class epoll
{
private:
    std::vector<epoll_event> ev_fds;//存放epoll_wait得到的所有的fd
    int epfd;

public:
    epoll();
    void epoll_add(int sock_fd, uint32_t events);
    void epoll_del(int sock_fd, uint32_t events);
    void epoll_mod(int sock_fd, uint32_t events);
    std::vector<epoll_event> poll();
    ~epoll();
};
