#include "Server.h"
Server ::Server(EventLoop *_loop)
{
    this->loop = _loop;
    sock_addr *sc_addr = new sock_addr("127.0.0.1", 9999);
    mysocket *mysc = new mysocket();
    mysc->bind(sc_addr);
    mysc->listen();
    mysc->setnonblocking();
    channel *ch = new channel(this->loop, mysc->getFd());           // 监听channel
    ch->setCallBack(std::bind(&Server::newConnection, this, mysc)); // 绑定监听channel 与建立连接操作函数
    ch->enAbleToReading();                                          // 此处不要忘了写！   此处是把监听channel加入到loop的epoll中，上面的setCallback的操作中的enAbleToReading()是将client的连接channel注册到loop中。
}
Server::~Server()
{
}
void Server::start() // 在创建实例后手动开启
{
    std::cout << "Server Start Now" << std::endl;
    loop->beginLoop(); // 此处是开始寻找loop中的epoll中的已注册的发生事件的channel，并根据channel中的CallBack函数来进行具体的操作。也就是 **启动服务器的操作**
}
void Server::newConnection(mysocket *mysc) // 监听socket进行新连接的建立，以新建的channel为单位，注册到loop对应的epoll中。
{
    sock_addr *sc_addr = new sock_addr();
    int client_fd = mysc->accept(sc_addr);
    setnonblocking(client_fd);
    std::cout << "Accept From " << "Port : " << sc_addr->getAddr().sin_port << " ip: " << network_to_shifen(sc_addr->getAddr().sin_addr.s_addr) << std::endl;
    channel *client_ch = new channel(this->loop, client_fd);
    client_ch->setCallBack(std::bind(&Server::handleReadingEvent, this, client_fd));
    client_ch->enAbleToReading(); // 此处是把自己注册到epoll，先设置想要监听的事件（读事件），调用ch所对应的loop的updateChannel函数，把自己（channel）传入后让loop里面的epoll*ev进行注册到epoll的操作
}
void Server::handleReadingEvent(int client_fd) // 实际进行业务操作（read，write）(开始通信)
{
    char buffer[MAX_BUFFER_SIZE];
    while (1)
    {
        memset(buffer, '\0', MAX_BUFFER_SIZE);
        ssize_t bytes_read = recv(client_fd, buffer, MAX_BUFFER_SIZE, 0);
        if (bytes_read > 0) // 正常读取数据
        {
            std::cout << "Server recv message " << buffer << std::endl;
            const char *message = "Hello";
            send(client_fd, message, strlen(message), 0);
            continue; // continue的原因是不知道当前是否读取完毕了，因此需要继续循环看是否读完了，对应于ifelse第三种情况
        }
        else if (bytes_read == 0) // 连接终止了
        {
            std::cout << "Client quit" << std::endl;
            close(client_fd);
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