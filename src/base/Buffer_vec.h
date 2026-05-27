#pragma once

#include <vector>
#include <string>

static const int kPrePendIndex = 8;  // prependindex长度
static const int kInitalSize = 1024; // 初始化开辟空间长度

class Buffer
{
private:
    std::vector<char> buffer_;
    int read_index_;
    int write_index_;

public:
    Buffer();
    ~Buffer();
    char *getBegin();
    char *getReadBegin();
    char *getWriteBegin();
    void append(const char *message, int len);
    void append(std::string message);
    void append(const char *message);
    int CanReadSize();
    int CanWriteSize();
    int PrependSize();
    void EnsureCanAppend(int len);
    char *PeekAll_c();
    std::string Peek_s(int len);
    std::string PeekAll_s();
    void Retrieve(int len);
    void RetrieveAll(); // 全部取走
    char *RetrieveAll_c();
    std::string Retrieve_s(int len);
    std::string RetrieveAll_s();
};
