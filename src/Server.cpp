#include "base/Server.h"
#include "base/Connection.h"
#include <bits/std_thread.h>
Server ::Server(EventLoop *_loop)
{
    this->MainReactor = _loop;
    apt = std::make_unique<Acceptor>(_loop);
    apt->setConnectionCallBack(std::bind(&Server::newConnection, this, std::placeholders::_1));
    int i = 0;
    int size = static_cast<int>(std::thread::hardware_concurrency()); // 线程数量，也是subReactor数量
    if (size <= 0)
    {
        size = 1;
    }
    thread_pool = std::make_unique<Thread_pool<std::function<void()>>>(size, 11451);
    while (i < size)
    {
        subReactors.push_back(std::make_unique<EventLoop>());
        i += 1;
    }
    i = 0;
    while (i < size)
    {
        std::function<void()> cb = std::bind(&EventLoop::beginLoop, subReactors[i].get());
        thread_pool->append(cb);
        i += 1;
    }
}
Server::~Server()
{
}
void Server::start() // 在创建实例后手动开启
{
    std::cout << "Server Start Now" << std::endl;
    MainReactor->beginLoop(); // 此处是开始寻找loop中的epoll中的已注册的发生事件的channel，并根据channel中的CallBack函数来进行具体的操作。也就是 **启动服务器的操作**
}
void Server::newConnection(mysocket *mysc)
{
    int random = mysc->getFd() % subReactors.size();
    mysc->setBlock(false);
    std::shared_ptr<Connection> connection = std::make_shared<Connection>(subReactors[random].get(), mysc);
    connection->state_ = Connection::State::Connected;
    connection->setDeleteConnectionCallBack(std::bind(&Server::deleteConnection, this, std::placeholders::_1));
    connection->setFunctionCallBack(Connect_Callback);
    connection->registerCallBack();
    std::lock_guard<std::mutex> guard(connections_mtx);
    connections[mysc->getFd()] = connection;
}
void Server::deleteConnection(int fd)
{
    std::shared_ptr<Connection> con;
    {
        std::lock_guard<std::mutex> guard(connections_mtx);
        auto it = connections.find(fd);
        if (it == connections.end())
        {
            return;
        }
        con = it->second;
        connections.erase(it);
    }
}
void Server::setConnect(std::function<void(Connection *)> _Connect_Callback)
{
    Connect_Callback = _Connect_Callback;
}
