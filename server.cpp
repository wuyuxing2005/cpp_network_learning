#include "Server.h"
int main()
{
    EventLoop *loop = new EventLoop();
    Server *server = new Server(loop);
    server->start(); // loop->beginloop();
    return 0;
}