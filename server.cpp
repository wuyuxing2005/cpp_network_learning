#include "src/base/Server.h"
#include "src/base/DebugLog.h"

#include <string>

namespace
{
bool ShouldEnableLog(int argc, char *argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "--log" || arg == "-l" || arg == "--log=1")
        {
            return true;
        }
        if (arg == "--no-log" || arg == "--log=0")
        {
            return false;
        }
    }
    return false;
}
}

int main(int argc, char *argv[])
{
    debuglog::SetEnabled(ShouldEnableLog(argc, argv));
    EventLoop *loop = new EventLoop();
    Server *server = new Server(loop);
    server->setConnect(
        [](Connection *connect)
        {
            connect->recv0();
            if (connect->state_ == Connection::State::Connected)
            {
                CPP_NETWORK_LOG << "Recv from client : " << connect->getsocket()->getFd() << " : " << connect->getReadBuffer() << '\n';
                connect->setSendBuffer(connect->getReadBuffer());
                connect->send0();
            }
            else
            {
                connect->close0();
                CPP_NETWORK_LOG << "Conenction close" << '\n';
            }
        });          // 我们希望是Server来控制服务器连接的业务逻辑
    server->start(); // loop->beginloop();
    delete loop;
    delete server;
    return 0;
}
