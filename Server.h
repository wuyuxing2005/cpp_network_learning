#include "EventLoop.h"
#include "channel.h"
#include "mysocket.h"
#include "sock_addr.h"
class Server
{
private:
    EventLoop *loop;

public:
    Server(EventLoop *loop);
    void newConnection(mysocket *my_socket);
    void enAbleReading(mysocket *my_socket);
    ~Server();
};

Server ::Server(EventLoop *loop)
{
    sock_addr *sc_addr = new sock_addr("127.0.0.1", 9999);
    mysocket *mysc = new mysocket();
    mysc->bind(sc_addr);
    mysc->listen();
    mysc->setnonblocking();
    EventLoop *loop = new EventLoop();
    channel *ch = new channel(loop, mysc->getFd());
    ch->setCallBack(std::bind(Server::newConnection, this, mysc));
    loop->beginLoop();
}
Server::~Server()
{
}
void Server::newConnection(channel *ch)
{
    sock_addr *sock_addr;
}
