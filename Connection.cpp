#include "Connection.h"
Connection::Connection(EventLoop *_loop, mysocket *_mysc)
{
    this->loop = _loop;
    mysc = _mysc;
    channel *ch = new channel(loop, mysc->getFd());
    ch->setCallBack(std::bind(&Connection::echo, this));
    ch->enAbleToReading();
}

Connection::~Connection()
{
}
void Connection::echo()
{
    char buffer[MAX_BUFFER_SIZE];
    while (1) // 非阻塞状态需要一次读完
    {
        memset(buffer, '\0', MAX_BUFFER_SIZE);
        ssize_t bytes_read = recv(mysc->getFd(), buffer, MAX_BUFFER_SIZE, 0);
        if (bytes_read > 0) // 正常读取数据
        {
            std::cout << "Server recv message " << buffer << std::endl;
            const char *message = "Hello";
            send(mysc->getFd(), message, strlen(message), 0);
            continue; // continue的原因是不知道当前是否读取完毕了，因此需要继续循环看是否读完了，对应于ifelse第三种情况
        }
        else if (bytes_read == 0) // 连接终止了
        {
            std::cout << "Client quit" << std::endl;
            deleteCallBack(mysc);
            close(mysc->getFd());
            break;
        }
        else if (bytes_read < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) // 读完了
        {
            break;
        }
        else
        {
            std::cout << "erro " << strerror(errno) << std::endl;
            break;
        }
    }
}
void Connection::setDeleteConnectionCallBack(std::function<void(mysocket *mysc)> CallBack)
{
    this->deleteCallBack = CallBack;
}