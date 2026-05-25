#pragma once

#include <memory>
#include <vector>
#include <string>
#include <cstring>
#include <cassert>
static const int kPrePendIndex = 8;  // prependindex长度
static const int kInitalSize = 1024; // 初始化开辟空间长度
class Buffer
{
private:
    std::vector<char> buffer_;
    int read_index_;
    int write_index_;

public:
    char *getBegin()
    {
        return &*buffer_.begin();
    }
    char *getReadBegin()
    {
        return getBegin() + read_index_;
    }
    char *getWriteBegin()
    {
        return getBegin() + write_index_;
    }
    void append(const char *message, int len)
    {
        // 确保大小可以放得下
        // TODO
        EnsureCanAppend(len);
        std::copy(message, message + len, getWriteBegin());
        write_index_ += len;
    }
    void append(std::string message)
    {
        append(message.c_str(), message.size());
    }
    void append(const char *message)
    {
        append(message, strlen(message));
    }
    int CanReadSize()
    {
        return write_index_ - read_index_;
    }
    int CanWriteSize()
    {
        return buffer_.size() - write_index_;
    }
    int PrependSize()
    {
        return read_index_;
    }
    void EnsureCanAppend(int len)
    {
        if (CanWriteSize() >= len)
        {
            return;
        }
        if (CanWriteSize() + PrependSize() >= kPrePendIndex + len)
        {
            std::copy(getReadBegin(), getReadBegin(), getBegin() + kPrePendIndex);
            read_index_ = kPrePendIndex;
            write_index_ = kPrePendIndex + CanReadSize();
        }
        else // 真正扩容
        {
            buffer_.resize(write_index_ + len);
        }
    }
    char *Peek_c(int len)
    {
        return getReadBegin() + len;
    }
    char *PeekAll_c()
    {
        return getReadBegin() + CanReadSize();
    }
    std::string Peek_s(int len)
    {
        return std::string(Peek_c(len));
    }
    std::string PeekAll_s()
    {
        return std::string(PeekAll_c());
    }
    void Retrieve(int len)
    {
        assert(CanReadSize() >= len);
        if (CanReadSize() > len)
        {
            read_index_ += len;
        }
        else
        {
            RetrieveAll();
        }
    }
    void RetrieveAll() // 全部取走
    {
        write_index_ = kPrePendIndex;
        read_index_ = kPrePendIndex;
    }
    char *Retrieve_c(int len)
    {
        assert(CanReadSize() >= len);
        char *temp = Peek_c(len);
        Retrieve(len);
        return temp;
    }
    char *RetrieveAll_c()
    {
        char *temp = PeekAll_c();
        RetrieveAll();
        return temp;
    }
    std::string Retrieve_s(int len)
    {
        return std::string(Retrieve_c(len));
    }
    std::string RetrieveAll_s()
    {
        return std::string(RetrieveAll_c());
    }
    
};