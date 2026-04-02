#pragma once

#include <atomic>
#include <iostream>

namespace debuglog
{
inline std::atomic<bool> &EnabledFlag()
{
    static std::atomic<bool> enabled{false};
    return enabled;
}

inline void SetEnabled(bool enabled)
{
    EnabledFlag().store(enabled, std::memory_order_relaxed);
}

inline bool Enabled()
{
    return EnabledFlag().load(std::memory_order_relaxed);
}
}

#define CPP_NETWORK_LOG if (!debuglog::Enabled()) {} else std::cout
