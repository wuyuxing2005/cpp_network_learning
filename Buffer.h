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
    void clear_s();
};

