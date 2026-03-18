#include "src/base/Server.h"
int main()
{
    EventLoop *loop = new EventLoop();
    Server *server = new Server(loop);
    server->setConnect([](Connection *connect) {

    });              // 我们希望是Server来控制服务器连接的业务逻辑
    server->start(); // loop->beginloop();
    delete loop;
    delete server;
    return 0;
}
