#pragma once
#include "lock/lock.h"
#include "src/base/Buffer.h"
#include <condition_variable>
#include <cstdio>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <atomic>
class asyncLogger
{
private:
    std::vector<Buffer> Buffers_0;
    std::vector<Buffer> Buffers_1;
    locker lock_;
    cond cond_;
    FILE *fp;
    std::thread thread_;
    std::atomic<bool> running;
    size_t buffer0Bytes_;
    size_t maxBufferBytes_;

public:
    void start();
    void run();
    void append(Buffer &buffer);
    asyncLogger(const char *filename);
    ~asyncLogger();
};
