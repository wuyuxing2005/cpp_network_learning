#pragma once
#include "lock/lock.h"
#include "src/base/Buffer.h"
#include <condition_variable>
#include <cstdio>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
class asyncLogger
{
private:
    std::vector<Buffer> Buffers_0;
    std::vector<Buffer> Buffers_1;
    locker lock_;
    sem sem_;
    FILE *fp;
    std::thread thread_;
    bool running;


public:
    void start();
    void run();
    void append(Buffer &buffer);
    asyncLogger(const char *filename);
    ~asyncLogger();
};
