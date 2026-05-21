#include "asyncLogger.h"
void asyncLogger::start()
{
    running = true;
    thread_ = std::thread(&asyncLogger::run, this);
}
void asyncLogger ::run()
{
    while (running)
    {
        sem_.wait();
        lock_.lock();
        Buffers_1.swap(Buffers_0);
        lock_.unlock();
        for (auto &buffer : Buffers_1)
        {
            fwrite(buffer.getChar_c(), 1, buffer.getSize(), fp);
        }
        fflush(fp);
        Buffers_1.clear();
    }
}
void asyncLogger::append(Buffer &buffer)
{
    lock_.lock();
    Buffers_0.push_back(buffer); // 生产者只写 Buffers_0
    lock_.unlock();
    sem_.post();
    /*
    需要注意一个问题：你现在用 sem_，如果短时间 append 了很多次，sem_ 会累计很多次通知。
    后台线程第一次醒来可能已经把所有数据都 swap 走了，后面还会因为多余的 sem_.wait() 立即返回几次，但 Buffers_0 是空的。
    这不影响正确性，只是会有空循环。*/
}
asyncLogger::asyncLogger(const char *filename)
{
    fp = fopen(filename, "a");
}

asyncLogger::~asyncLogger()
{
    running = false;
    sem_.post();
    if (thread_.joinable())
    {
        thread_.join();
    }
    fclose(fp);
}
