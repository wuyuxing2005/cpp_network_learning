#include "base/Buffer_vec.h"

#include <algorithm>
#include <cassert>
#include <cstring>

Buffer::Buffer() : read_index_(kPrePendIndex), write_index_(kPrePendIndex), buffer_(kInitalSize)
{
}

Buffer::~Buffer()
{
}

char *Buffer::getBegin()
{
    return &*buffer_.begin();
}

char *Buffer::getReadBegin()
{
    return getBegin() + read_index_;
}

char *Buffer::getWriteBegin()
{
    return getBegin() + write_index_;
}

void Buffer::append(const char *message, int len)
{
    // 确保大小可以放得下
    EnsureCanAppend(len);
    std::copy(message, message + len, getWriteBegin());
    write_index_ += len;
}

void Buffer::append(std::string message)
{
    append(message.c_str(), message.size());
}

void Buffer::append(const char *message)
{
    append(message, strlen(message));
}

int Buffer::CanReadSize()
{
    return write_index_ - read_index_;
}

int Buffer::CanWriteSize()
{
    return buffer_.size() - write_index_;
}

int Buffer::PrependSize()
{
    return read_index_;
}

void Buffer::EnsureCanAppend(int len)
{
    if (CanWriteSize() >= len)
    {
        return;
    }
    if (CanWriteSize() + PrependSize() >= kPrePendIndex + len)
    {
        std::copy(getReadBegin(), getWriteBegin(), getBegin() + kPrePendIndex);
        read_index_ = kPrePendIndex;
        write_index_ = kPrePendIndex + CanReadSize();
    }
    else // 真正扩容
    {
        buffer_.resize(write_index_ + len);
    }
}

char *Buffer::PeekAll_c()
{
    return getReadBegin();
}

std::string Buffer::Peek_s(int len)
{
    assert(CanReadSize() >= len);
    return std::string(getReadBegin(), getReadBegin() + len);
}

std::string Buffer::PeekAll_s()
{
    return std::string(getReadBegin(), getReadBegin() + CanReadSize());
}

void Buffer::Retrieve(int len)
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

void Buffer::RetrieveAll()
{
    write_index_ = kPrePendIndex;
    read_index_ = kPrePendIndex;
}

char *Buffer::RetrieveAll_c()
{
    char *temp = getReadBegin();
    RetrieveAll();
    return temp;
}

std::string Buffer::Retrieve_s(int len)
{
    std::string result = Peek_s(len);
    Retrieve(len);
    return result;
}

std::string Buffer::RetrieveAll_s()
{
    std::string result = PeekAll_s();
    RetrieveAll();
    return result;
}
