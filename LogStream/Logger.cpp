#include "Logger.h"

#include "../time/TimeStamp.h"
#include "../CurrentThread/currentthread.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <utility>

namespace
{
    thread_local time_t t_last_active_time = 0;
    thread_local char t_time[64]{};
    Logger::LogLevel g_logLevel = Logger::INFO;

    void defaultOutputFunc(const char *msg, int len)
    {
        size_t n = fwrite(msg, 1, len, stdout);
        (void)n;
    }
    void defaultFlushFunc()
    {
        fflush(stdout);
    }
    std::function<void()> g_flushFunc = defaultFlushFunc;
    std::function<void(const char *msg, int len)> g_outputFunc = defaultOutputFunc;
}

Logger::Imt::Imt(const char *_SourceFile, int _line, LogLevel _loglevel)
    : loglevel(_loglevel), line(_line), sf(_SourceFile)
{
    formatTime();
    logstream << CurrentThread::tid() << ' ';
    logstream << getLoglevel() << ' ';
}

Logger::Imt::~Imt()
{
}

std::string Logger::Imt::getLoglevel()
{
    switch (loglevel)
    {
    case DEBUG:
        return "DEBUG";
    case INFO:
        return "INFO";
    case WARN:
        return "WARN";
    case ERROR:
        return "ERROR";
    case FATAL:
        return "FATAL";
    }
    return "UNKNOWN";
}
void Logger::Imt::formatTime()
{
    int64_t now = TimeStamp::getNowTime().microseconds();
    time_t now_time = static_cast<time_t>(now / kMicrosecond2Second);
    int microseconds = static_cast<int>(now % kMicrosecond2Second);

    if (t_last_active_time != now_time)
    {
        struct tm tm_time{};
        localtime_r(&now_time, &tm_time);
        snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d",
                 tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                 tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
        t_last_active_time = now_time;
    }

    fmt micro(".%06d ", microseconds);
    logstream << t_time << micro.getData();
}
void Logger::setFlushFunc(std::function<void()> func)
{
    if (func)
    {
        g_flushFunc = std::move(func);
    }
    else
    {
        g_flushFunc = defaultFlushFunc;
    }
}

void Logger::setOutputFunc(std::function<void(const char *msg, int len)> func)
{
    if (func)
    {
        g_outputFunc = std::move(func);
    }
    else
    {
        g_outputFunc = defaultOutputFunc;
    }
}

void Logger::fileOutputFunc(const char *msg, int len)
{
    FILE *fp = fopen("log.txt", "a");
    if (fp)
    {
        size_t n = fwrite(msg, 1, len, fp);
        (void)n;
        fclose(fp);
    }
}

int Logger::Imt::getline()
{
    return line;
}

LogStream &Logger::Imt::getStream()
{
    return logstream;
}

const char *Logger::Imt::getSourceFileData()
{
    return sf.data_;
}

Logger::LogLevel Logger::getLevel()
{
    return g_logLevel;
}

void Logger::setLevel(Logger::LogLevel level)
{
    g_logLevel = level;
}

Logger::Logger(const char *_SourceFile, int _line, LogLevel _loglevel)
{
    imt = new Imt(_SourceFile, _line, _loglevel);
}

Logger::~Logger()
{
    imt->getStream() << ' ' << imt->getSourceFileData() << ':' << imt->getline() << '\n';
    g_outputFunc(imt->getStream().getData(), imt->getStream().getLen());
    delete imt;
}
int Logger::getline()
{
    return imt->getline();
}

LogStream &Logger::getStream()
{
    return imt->getStream();
}
