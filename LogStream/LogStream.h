#pragma once

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>
#include <type_traits>

#define MAX_STREAM_LEN 4096
#define kMaxNumericSize 64

class FixedBuffer
{
private:
    char data[MAX_STREAM_LEN];
    char *cur;

public:
    FixedBuffer();
    ~FixedBuffer();

    char *current();
    int len() const;
    int avail() const;
    void append(const char *s, int len);
    void addLen(int len);
    void resetCur();
    char *getData();
    const char *getData() const;
    void clear();
};

class LogStream
{
    typedef LogStream self;

private:
    FixedBuffer buffer;

    template <typename T>
    void formatInteger(T value);

public:
    LogStream();
    ~LogStream();

    self &operator<<(bool v);
    self &operator<<(short num);
    self &operator<<(unsigned short num);
    self &operator<<(int num);
    self &operator<<(unsigned int num);
    self &operator<<(long num);
    self &operator<<(unsigned long num);
    self &operator<<(long long num);
    self &operator<<(unsigned long long num);

    self &operator<<(const float &num);
    self &operator<<(const double &num);
    self &operator<<(char v);
    self &operator<<(const char *str);
    self &operator<<(const std::string &v);
    char *getData();
    int getLen();
    void append(const char *s, int len);
    void resetBuffer();
};

class fmt
{
private:
    char data[32];
    int len;

public:
    template <typename T>
    fmt(const char *format, T val)
    {
        static_assert(std::is_arithmetic<T>::value == true, "Must be arithmetic type");
        int written = snprintf(data, sizeof(data), format, val);
        if (written < 0)
        {
            len = 0;
        }
        else if (static_cast<size_t>(written) >= sizeof(data))
        {
            len = sizeof(data) - 1;
        }
        else
        {
            len = written;
        }
    }

    ~fmt();
    const char *getData() const;
    int getLen() const;
};

template <typename T>
void LogStream::formatInteger(T value)
{
    if (buffer.avail() >= kMaxNumericSize)
    {
        char *buf = buffer.current();
        char *now = buf;
        bool flag = 0;
        if (value < 0)
        {
            flag = 1;
            value = -value;
        }
        do
        {
            int remainder = value % 10;
            *(now++) = remainder + '0';
            value /= 10;
        } while (value != 0);
        if (flag)
        {
            *now = '-';
            now += 1;
        }
        std::reverse(buf, now);
        buffer.addLen(now - buf);
    }
}

inline LogStream &operator<<(LogStream &s, const fmt &_fmt)
{
    s.append(_fmt.getData(), _fmt.getLen());
    return s;
}
