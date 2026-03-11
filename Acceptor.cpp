#include "Acceptor.h"
Acceptor::Acceptor(EventLoop *_loop)
{
    this->loop = _loop;
    socket = new mysocket();
    sc_addr = new sock_addr("127.0.0.1", 9999);
    socket->bind(sc_addr);
    socket->listen();
    ch = new channel(_loop, socket->getFd());
    ch->setCallBack(std::bind(&Acceptor::accpetNewConnection, this));
    /*
    此处你可能会担心，万一我还没来得及设置CallBack，此时监听channel就已经发生事件，此时调用的CallBack函数不就是空的吗。
    其实不用担心，因为只有当server端server->start()之后，也就是loop->beginloop()之后才会开始进行注册事件的处理。
    */
    ch->enAbleToReading(); // 注册进loop（epoll）
}

void Acceptor::accpetNewConnection() // 启动accpetor
{
    CallBack(this->socket);
}

void Acceptor::setConnectionCallBack(std::function<void(mysocket *sc)> _CallBack) // 启动前需要初始化CallBack
{
    this->CallBack = _CallBack;
}
Acceptor::~Acceptor()
{
    delete socket;
    delete sc_addr;
    delete ch;
}
