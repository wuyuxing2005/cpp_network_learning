#include "base/Connection.h"

namespace
{
std::string PreviewBuffer(const std::string &buffer, std::size_t max_len = 80)
{
    std::string preview;
    preview.reserve(std::min(buffer.size(), max_len));
    for (char ch : buffer)
    {
        if (preview.size() >= max_len)
        {
            break;
        }
        if (ch == '\r')
        {
            preview += "\\r";
        }
        else if (ch == '\n')
        {
            preview += "\\n";
        }
        else
        {
            preview += ch;
        }
    }
    if (buffer.size() > max_len)
    {
        preview += "...";
    }
    return preview;
}
}

Connection::Connection(EventLoop *_loop, std::unique_ptr<mysocket> _mysc) // 负责连接socket（channel）的处理
{
    this->loop = _loop;
    mysc = std::move(_mysc);
    ch = std::make_unique<channel>(loop, mysc->getFd(), true, true);
    readBuffer = std::make_unique<Buffer>();
    sendBuffer = std::make_unique<Buffer>();
    http_context = std::make_unique<HttpContext>();
}

Connection::~Connection()
{
}
void Connection::connectionDestructor()
{
    if (ch != nullptr && ch->getInepoll())
    {
        loop->deleteChannel(ch.get()); // 这里使用我们人为的delete函数是为了删除ch在epoll中的注册,具体的内存上的析构处理仍然在Connection中
    }
}
void Connection::registerCallBack()
{
    ch->Tie(shared_from_this());
    ch->setReadCallBack(std::bind(&Connection::handleFunctionCallBack, this));
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
EventLoop *Connection::getLoop()
{
    return this->loop;
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
            const std::string current_buffer = readBuffer->getString();
            std::cout << "[conn-peer-close] fd=" << mysc->getFd()
                      << " buffered_bytes=" << current_buffer.size()
                      << " preview=\"" << PreviewBuffer(current_buffer) << "\""
                      << std::endl;
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
    int size = sendBuffer->getSize();
    if (size <= 0)
    {
        return;
    }
    const char *buffer = sendBuffer->getChar_c();
    int bytes_send = 0;
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

    noBlockedRecv();
}
void Connection::send0()
{
    noBlockedSend();
    sendBuffer->clear_s(); // 清空sendBuffer
}
void Connection::close0() // 关闭Connection对应的socket，只是逻辑关闭，但是Connection实例的析构还需要析构函数
{
    deleteCallBack(mysc->getFd());
}
mysocket *Connection::getsocket()
{
    return mysc.get();
}
HttpContext *Connection::context()
{
    return http_context.get();
}
