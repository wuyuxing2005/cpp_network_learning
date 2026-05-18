#include "LogStream.h"

FixedBuffer::FixedBuffer()
{
    cur = data;
}

FixedBuffer::~FixedBuffer()
{
}

char *FixedBuffer::current()
{
    return cur;
}

int FixedBuffer::len() const
{
    return cur - data;
}

int FixedBuffer::avail() const
{
    return MAX_STREAM_LEN - len();
}

void FixedBuffer::append(const char *s, int len)
{
    if (avail() >= len)
    {
        memcpy(cur, s, len);
        cur += len;
    }
}

void FixedBuffer::addLen(int len)
{
    cur += len;
}

void FixedBuffer::resetCur()
{
    cur = data;
}

char *FixedBuffer::getData()
{
    return data;
}

const char *FixedBuffer::getData() const
{
    return data;
}

void FixedBuffer::clear()
{
    memset(data, 0, sizeof(data));
}

LogStream::LogStream()
{
}

LogStream::~LogStream()
{
}
int LogStream::getLen()
{
    return buffer.len();
}
void LogStream::append(const char *s, int len)
{
    buffer.append(s, len);
}
char *LogStream::getData()
{
    return buffer.getData();
}
void LogStream::resetBuffer()
{
    buffer.resetCur();
    buffer.clear();
}

fmt::~fmt()
{
}

const char *fmt::getData() const
{
    return data;
}

int fmt::getLen() const
{
    return len;
}

LogStream &LogStream::operator<<(bool v)
{
    if (v)
    {
        buffer.append("true", 4);
    }
    else
    {
        buffer.append("false", 5);
    }
    return *this;
}

LogStream &LogStream::operator<<(short num)
{
    formatInteger(num);
    return *this;
}

LogStream &LogStream::operator<<(unsigned short num)
{
    formatInteger(num);
    return *this;
}

LogStream &LogStream::operator<<(int num)
{
    formatInteger(num);
    return *this;
}

LogStream &LogStream::operator<<(unsigned int num)
{
    formatInteger(num);
    return *this;
}

LogStream &LogStream::operator<<(long num)
{
    formatInteger(num);
    return *this;
}

LogStream &LogStream::operator<<(unsigned long num)
{
    formatInteger(num);
    return *this;
}

LogStream &LogStream::operator<<(long long num)
{
    formatInteger(num);
    return *this;
}

LogStream &LogStream::operator<<(unsigned long long num)
{
    formatInteger(num);
    return *this;
}

LogStream &LogStream::operator<<(const double &num)
{
    char buf[64];
    int len = snprintf(buf, sizeof(buf), "%g", num);
    if (len > 0)
    {
        if (static_cast<size_t>(len) >= sizeof(buf))
        {
            len = sizeof(buf) - 1;
        }
        buffer.append(buf, len);
    }
    return *this;
}

LogStream &LogStream::operator<<(const float &num)
{
    return *this << static_cast<double>(num);
}

LogStream &LogStream::operator<<(char v)
{
    buffer.append(&v, 1);
    return *this;
}

LogStream &LogStream::operator<<(const char *str)
{
    if (str)
    {
        buffer.append(str, strlen(str));
    }
    else
    {
        buffer.append("(null)", 6);
    }
    return *this;
}

LogStream &LogStream::operator<<(const std::string &v)
{
    buffer.append(v.c_str(), v.size());
    return *this;
}
