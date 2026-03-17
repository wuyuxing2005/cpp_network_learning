#include "Connection.h"
Connection::Connection(EventLoop *_loop, mysocket *_mysc) // 负责连接socket（channel）的处理
{
    this->loop = _loop;
    mysc = _mysc;
    ch = new channel(loop, mysc->getFd(), true, true);
    readBuffer = new Buffer();
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
}

void Connection::registerCallBack()
{
    std::weak_ptr<Connection> weak_self = shared_from_this();
    ch->setCallBack([weak_self]() { // 看你的当前connection是否被delete，如果被delette就不调用echo了。
                                    // 原来的是std::bind(&Connection::echo, this)，但是如果this被delete会报错
        auto self = weak_self.lock();
        if (self)
        {
            self->echo();
        }
    });
    ch->enAbleToReading();
}

void Connection::echo()
{
    // std::lock_guard<std::mutex> guard(conn_mtx);
    char buffer[MAX_BUFFER_SIZE];
    while (1) // 非阻塞状态需要一次读完
    {
        memset(buffer, '\0', MAX_BUFFER_SIZE);
        ssize_t bytes_read = recv(mysc->getFd(), buffer, MAX_BUFFER_SIZE, 0);
        if (bytes_read > 0) // 正常读取数据
        {
            readBuffer->append(buffer, bytes_read);
            continue; // continue的原因是不知道当前是否读取完毕了，因此需要继续循环看是否读完了，对应于ifelse第三种情况
        }
        else if (bytes_read == 0) // 连接终止了
        {
            std::cout << "Client quit" << std::endl;
            deleteCallBack(mysc->getFd());
            break;
        }
        else if (bytes_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) // 读完了
        {
            std::cout << "Server recv message " << readBuffer->getString() << std::endl;
            send(mysc->getFd(), readBuffer->getChar_c(), readBuffer->getSize(), 0);
            readBuffer->clear_s();
            break;
        }
        else
        {
            std::cout << "erro " << strerror(errno) << std::endl;
            break;
        }
    }
}
void Connection::setDeleteConnectionCallBack(std::function<void(int)> CallBack)
{
    this->deleteCallBack = CallBack;
}
