#pragma once

#include "LogStream.h"

#include <cstring>
#include <functional>
#include <string>

class Logger
{
public:
    // 禁止拷贝和移动构造函数和赋值运算符
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;
    Logger(Logger &&) = delete;
    Logger &operator=(Logger &&) = delete;
    class SourceFile
    {
    public:
        SourceFile(const char *data)
        {
            data_ = data;
            size_ = static_cast<int>(std::strlen(data_));
            const char *index = std::strrchr(data, '/');
            if (index)
            {
                data_ = index + 1;
                size_ -= data_ - data;
            }
        }
        const char *data_;
        int size_;
    };
    enum LogLevel
    {
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL
    };
    static LogLevel getLevel();
    static void setLevel(LogLevel level);
    static void setOutputFunc(std::function<void(const char *msg, int len)> func);
    static void setFlushFunc(std::function<void()> func);
    static void fileOutputFunc(const char *msg, int len);
    int getline();
    LogStream &getStream();
    Logger(const char *_SourceFile, int _line, LogLevel _loglevel);
    ~Logger();

private:
    class Imt // 用来组装日志信息的类
    {
    public:
        Imt(const char *_SourceFile, int _line, LogLevel _loglevel);
        ~Imt();
        int loglevel;
        int getline();
        LogStream &getStream();
        const char *getSourceFileData();
        void formatTime();
        std::string getLoglevel();

    private:
        int line;
        LogStream logstream;
        Logger::SourceFile sf;
    };
    Imt *imt;
};
#define LOG_INFO                            \
    if (Logger::getLevel() <= Logger::INFO) \
    Logger(__FILE__, __LINE__, Logger::INFO).getStream()
#define LOG_DEBUG                            \
    if (Logger::getLevel() <= Logger::DEBUG) \
    Logger(__FILE__, __LINE__, Logger::DEBUG).getStream()
#define LOG_WARN                            \
    if (Logger::getLevel() <= Logger::WARN) \
    Logger(__FILE__, __LINE__, Logger::WARN).getStream()
#define LOG_ERROR                            \
    if (Logger::getLevel() <= Logger::ERROR) \
    Logger(__FILE__, __LINE__, Logger::ERROR).getStream()
#define LOG_FATAL                            \
    if (Logger::getLevel() <= Logger::FATAL) \
    Logger(__FILE__, __LINE__, Logger::FATAL).getStream()
