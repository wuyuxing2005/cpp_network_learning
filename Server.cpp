#include "Server.h"
#include "Connection.h"
Server ::Server(EventLoop *_loop)
{
    this->loop = _loop;
    apt = new Acceptor(_loop);
    apt->setConnectionCallBack(std::bind(&Server::newConnection, this, std::placeholders::_1));
}
Server::~Server()
{
}
void Server::start() // 在创建实例后手动开启
{
    std::cout << "Server Start Now" << std::endl;
    loop->beginLoop(); // 此处是开始寻找loop中的epoll中的已注册的发生事件的channel，并根据channel中的CallBack函数来进行具体的操作。也就是 **启动服务器的操作**
}
void Server::newConnection(mysocket *mysc)
{
    Connection *connection = new Connection(loop, mysc);
    connection->setDeleteConnectionCallBack(std::bind(&Server::deleteConnection, this, std::placeholders::_1));
    connections[mysc->getFd()] = connection;
}

void Server::deleteConnection(mysocket *mysc)
{
    Connection *con = connections[mysc->getFd()];
    delete con;
    connections.erase(mysc->getFd());
}
