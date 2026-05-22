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
        lock_.lock();
        while (running && Buffers_0.empty())
        {
            struct timespec ts{};
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += 3;
            cond_.time_wait(ts, lock_.getlock());
        }
        if (running && buffer0Bytes_ < maxBufferBytes_)
        {
            struct timespec ts{};
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += 3;
            cond_.time_wait(ts, lock_.getlock());
        }
        if (!Buffers_0.empty())
        {
            Buffers_1.swap(Buffers_0);
            buffer0Bytes_ = 0;
        }
        lock_.unlock();

        for (auto &buffer : Buffers_1)
        {
            if (fp)
            {
                fwrite(buffer.getChar_c(), 1, buffer.getSize(), fp);
            }
        }
        if (fp)
        {
            fflush(fp);
        }
        Buffers_1.clear();
    }
    lock_.lock();
    if (!Buffers_0.empty())
    {
        Buffers_1.swap(Buffers_0);
        buffer0Bytes_ = 0;
    }
    lock_.unlock();
    for (auto &buffer : Buffers_1)
    {
        if (fp)
        {
            fwrite(buffer.getChar_c(), 1, buffer.getSize(), fp);
        }
    }
    if (fp)
    {
        fflush(fp);
    }
    Buffers_1.clear();
}
void asyncLogger::append(Buffer &buffer)
{
    lock_.lock();
    Buffers_0.push_back(buffer);
    buffer0Bytes_ += buffer.getSize();
    if (buffer0Bytes_ >= maxBufferBytes_)
    {
        cond_.signal();
    }
    lock_.unlock();
}
asyncLogger::asyncLogger(const char *filename)
{
    fp = fopen(filename, "a");
    running = false;
    buffer0Bytes_ = 0;
    maxBufferBytes_ = 4 * 1024;
}

asyncLogger::~asyncLogger()
{
    running = false;
    lock_.lock();
    cond_.broadcast();
    lock_.unlock();
    if (thread_.joinable())
    {
        thread_.join();
    }
    if (fp)
    {
        fclose(fp);
    }
}
