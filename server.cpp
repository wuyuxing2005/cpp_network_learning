#include "Server.h"
int main()
{
    EventLoop *loop = new EventLoop();
    Server *server = new Server(loop);
    server->start();

    return 0;
}