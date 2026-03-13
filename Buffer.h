#include <string>
#include <iostream>
class Buffer
{
private:
    std::string s;

public:
    Buffer();
    ~Buffer();
    std::istream &inputLine();
    std::string getString();
    int getSize();
    const char *getChar_c();
    void append(char *ch, ssize_t size);
};

Buffer::Buffer()
{
}

Buffer::~Buffer()
{
}

std::istream &Buffer::inputLine()
{
    return getline(std::cin, s);
}
std::string Buffer::getString()
{
    return s;
}
int Buffer::getSize()
{
    return s.size();
}
const char *Buffer::getChar_c()
{
    return s.c_str();
}
void Buffer::append(char *ch, ssize_t size)
{
    s.append(ch, size);
}