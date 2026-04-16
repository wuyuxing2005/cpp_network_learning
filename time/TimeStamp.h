#include <cstdint>
#include <linux/time.h>
#include <sys/time.h>
#include <string>
const int kMicrosecond2Second = 1000 * 1000;
class TimeStamp // 对时间的封装
{
private:
    int64_t micro_seconds;

public:
    TimeStamp();
    explicit TimeStamp(int64_t _micro_seconds);
    bool operator<(const TimeStamp &rhs) const
    {
        return micro_seconds < ((TimeStamp)rhs).microseconds();
    }

    bool operator==(const TimeStamp &rhs) const
    {
        return micro_seconds == ((TimeStamp)rhs).microseconds();
    }
    static TimeStamp getNowTime();
    std::string ToFormattedString() const;
    static TimeStamp AddTime(TimeStamp timestamp, double add_seconds); // 现在的时间加
    int64_t microseconds();
    ~TimeStamp();
};

TimeStamp::TimeStamp(int64_t _micro_seconds)
{
    micro_seconds = _micro_seconds;
}

TimeStamp TimeStamp::getNowTime()
{
    timeval time;
    gettimeofday(&time, nullptr);
    return TimeStamp(time.tv_sec * kMicrosecond2Second + time.tv_usec);
}
TimeStamp TimeStamp::AddTime(TimeStamp timestamp, double add_seconds)
{
    int64_t add_microseconds = static_cast<int64_t>(add_seconds) * kMicrosecond2Second;
    return TimeStamp(timestamp.microseconds() + add_microseconds);
}
int64_t TimeStamp::microseconds()
{
    return micro_seconds;
}
std::string TimeStamp::ToFormattedString() const
{
}
TimeStamp::~TimeStamp()
{
}
