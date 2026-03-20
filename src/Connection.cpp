#include "base/Connection.h"
Connection::Connection(EventLoop *_loop, mysocket *_mysc) // 负责连接socket（channel）的处理
{
    this->loop = _loop;
    mysc = _mysc;
    ch = new channel(loop, mysc->getFd(), true, true);
    readBuffer = new Buffer();
    sendBuffer = new Buffer();
}

Connection::~Connection()
{
    if (ch != nullptr && ch->getInepoll())
    {
        loop->deleteChannel(ch);
    }
    delete ch;
    delete mysc;
    delete readBuffer;
    delete sendBuffer;
}

void Connection::registerCallBack()
{
    std::weak_ptr<Connection> weak_self = shared_from_this();
    ch->setCallBack([weak_self]() { // 看你的当前connection是否被delete，如果被delette就不调用echo了。
                                    // 原来的是std::bind(&Connection::echo, this)，但是如果this被delete会报错
        auto self = weak_self.lock();
        if (self)
        {
            self->handleFunctionCallBack();
        }
    });
    ch->enAbleToReading();
}
void Connection::handleFunctionCallBack()
{
    functionCallBack(this);
}
void Connection::setFunctionCallBack(std::function<void(Connection *)> _functionCallBack)
{
    functionCallBack = _functionCallBack; // server.cpp中设置的函数。注意是小写s的server.cpp
}

void Connection::setDeleteConnectionCallBack(std::function<void(int)> CallBack)
{
    this->deleteCallBack = CallBack;
}

void Connection::setSendBuffer(std::string s)
{
    sendBuffer->clear_s(); // 清空sendBuffer
    sendBuffer->append((char *)s.c_str(), s.size());
}

std::string Connection::getReadBuffer()
{
    return readBuffer->getString();
}
void Connection::BlockedRecv()
{
    char buffer[MAX_BUFFER_SIZE];
    memset(buffer, '\0', MAX_BUFFER_SIZE);
    ssize_t s = recv(mysc->getFd(), buffer, MAX_BUFFER_SIZE, 0);
    if (s > 0)
    {
        readBuffer->append(buffer, s);
    }
    else if (s == 0)
    {
        state_ = State::Closed;
    }
    else
    {
        std::cout << "error " << strerror(errno) << std::endl;
        state_ = State::Failed;
    }
}
void Connection::BlockedSend()
{
    ssize_t s = send(mysc->getFd(), sendBuffer->getChar_c(), sendBuffer->getSize(), 0);
    if (s == -1)
    {
        std::cout << "error " << strerror(errno) << std::endl;
        state_ = State::Failed;
    }
}
void Connection::noBlockedRecv()
{

    char buffer[MAX_BUFFER_SIZE];
    while (1)
    {
        memset(buffer, '\0', MAX_BUFFER_SIZE);
        ssize_t bytes_read = recv(mysc->getFd(), buffer, MAX_BUFFER_SIZE, 0);
        if (bytes_read > 0)
        {
            readBuffer->append(buffer, bytes_read);
            continue;
        }
        else if (bytes_read == 0)
        {
            state_ = State::Closed;
            break;
        }
        else if (bytes_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) // 读完了
        {
            break;
        }
        else
        {
            std::cout << "error " << strerror(errno) << std::endl;
            state_ = State::Failed;
            break;
        }
    }
}
void Connection::noBlockedSend()
{
    char buffer[sendBuffer->getSize()];
    memccpy(buffer, sendBuffer->getChar_c(), sendBuffer->getSize(), sendBuffer->getSize());
    int bytes_send = 0;
    int size = sendBuffer->getSize();
    while (bytes_send < size)
    {
        ssize_t s = send(mysc->getFd(), buffer + bytes_send, size - bytes_send, 0);
        if (s > 0)
        {
            bytes_send += s;
            continue;
        }
        else if (s == -1 && (errno == EWOULDBLOCK || errno == EAGAIN)) // 发完了
        {
            break;
        }
        else if (s == -1 && errno == EINTR) // 发送中断
        {
            continue;
        }
        else
        {
            std::cout << "error" << strerror(errno) << std::endl;
            break;
        }
    }
}
void Connection::recv0()
{
    readBuffer->clear_s(); // 清空readBuffer
    if (mysc->getIsBlocked())
    {
        BlockedRecv();
    }
    else
    {
        noBlockedRecv();
    }
}
void Connection::send0()
{
    if (mysc->getIsBlocked())
    {
        BlockedSend();
    }
    else
    {
        noBlockedSend();
    }
    sendBuffer->clear_s(); // 清空sendBuffer
}
void Connection::close0()
{
    deleteCallBack(mysc->getFd());
}
mysocket *Connection::getsocket()
{
    return mysc;
}