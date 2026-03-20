#include "base/Acceptor.h"
#include "base/set_noblocking.h"
#include <iostream>
Acceptor::Acceptor(EventLoop *_loop) // 该类主要是抽象监听socket
{
    this->loop = _loop;
    socket = std::make_unique<mysocket>();
    sc_addr = std::make_unique<sock_addr>("127.0.0.1", 9999);
    socket->bind(sc_addr.get());
    socket->listen();
    ch = std::make_unique<channel>(_loop, socket->getFd(), false, false);
    ch->setCallBack(std::bind(&Acceptor::accpetNewConnection, this));
    ch->enAbleToReading(); // 注册进loop（epoll）
}

void Acceptor::accpetNewConnection() // 启动accpetor
{
    std::unique_ptr<sock_addr> sc_addr = std::make_unique<sock_addr>();
    int client_fd = socket->accept(sc_addr.get());
    setnonblocking(client_fd);
    std::cout << "Accept From " << "Port : " << sc_addr->getAddr().sin_port << " ip: " << network_to_shifen(sc_addr->getAddr().sin_addr.s_addr) << std::endl;
    std::unique_ptr<mysocket> client_socket = std::make_unique<mysocket>(client_fd);
    CallBack(client_socket.get()); // 使用Server中的newconnection
}

void Acceptor::setConnectionCallBack(std::function<void(mysocket *sc)> _CallBack) // 启动前需要初始化CallBack
{
    this->CallBack = _CallBack; // 将connextion的创建交给Server
    // 原因是因为Accpetor只负责监听与accpet，是负责一种机制，accpet之后获得的fd已经和监听fd（Accpetor）无关了，后续连接的操作应该由Server管理
}
Acceptor::~Acceptor()
{
}
