#include "src/base/Server.h"
int main()
{
    EventLoop *loop = new EventLoop();
    Server *server = new Server(loop);
    server->setConnect(
        [](Connection *connect)
        {
            connect->recv0();
            if (connect->state_ == Connection::State::Connected)
            {
                std::cout << "Recv from client : " << connect->getsocket()->getFd() << " : " << connect->getReadBuffer() << std::endl;
                connect->setSendBuffer(connect->getReadBuffer());
                connect->send0();
            }
            else
            {
                connect->close0();
                std::cout << "Conenction close" << std::endl;
            }
        });          // 我们希望是Server来控制服务器连接的业务逻辑
    server->start(); // loop->beginloop();
    delete loop;
    delete server;
    return 0;
}
