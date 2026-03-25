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
void Server::newConnection(std::unique_ptr<mysocket> mysc) // acceptor中的回调函数
{
    int random = mysc->getFd() % subReactors.size();
    std::shared_ptr<Connection> connection = std::make_shared<Connection>(subReactors[random].get(), std::move(mysc));

    connection->state_ = Connection::State::Connected;
    connection->setDeleteConnectionCallBack(std::bind(&Server::deleteConnection, this, std::placeholders::_1));
    connection->setFunctionCallBack(Connect_Callback);
    {
        std::lock_guard<std::mutex> guard(connections_mtx); // main reactor 创建连接，sub reactor 可能删除连接，这里只保护 map 修改
        connections[connection->getsocket()->getFd()] = connection;
    }
    connection->registerCallBack(); // 设置channel回调函数并注册进epoll
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
    EventLoop *loop_ = con->getLoop();
    loop_->PushFuncInToDoList(std::bind(&Connection::connectionDestructor, con)); // 因为是拷贝操作，con就是一份额外的connection 的shared_ptr的计数。
    //在该函数未返回之前实际上con的shared_ptr引用数为3，因为此时还存在一个临时变量con。当该函数返回时就只剩俩了
}
void Server::setConnect(std::function<void(Connection *)> _Connect_Callback)
{
    Connect_Callback = _Connect_Callback;
}
