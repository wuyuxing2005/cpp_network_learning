#include "base/Buffer.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>

Buffer::Buffer() : read_index_(kPrePendIndex), write_index_(kPrePendIndex), buffer_(kInitalSize)
{
}

Buffer::Buffer(char *str) : Buffer()
{
    append(str);
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
    append(message.c_str(), static_cast<int>(message.size()));
}

void Buffer::append(const char *message)
{
    append(message, static_cast<int>(strlen(message)));
}

void Buffer::append(const char *message, ssize_t len)
{
    append(message, static_cast<int>(len));
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
        int readable = CanReadSize();
        std::copy(getReadBegin(), getWriteBegin(), getBegin() + kPrePendIndex);
        read_index_ = kPrePendIndex;
        write_index_ = kPrePendIndex + readable;
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

std::istream &Buffer::inputLine()
{
    std::string line;
    RetrieveAll();
    if (std::getline(std::cin, line))
    {
        append(line);
    }
    return std::cin;
}

std::string Buffer::getString()
{
    return PeekAll_s();
}

int Buffer::getSize()
{
    return CanReadSize();
}

const char *Buffer::getChar_c()
{
    return getReadBegin();
}

void Buffer::clear_s()
{
    RetrieveAll();
}
