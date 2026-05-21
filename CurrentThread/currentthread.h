#pragma once

#include <sys/syscall.h>
#include <unistd.h>

namespace CurrentThread
{
    // 获取线程ID
    inline int tid()
    {
        static thread_local int t_cachedTid = 0;
        if (t_cachedTid == 0)
        {
            t_cachedTid = static_cast<int>(::syscall(SYS_gettid));
        }
        return t_cachedTid;
    }
}
