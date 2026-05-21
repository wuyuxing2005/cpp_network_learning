#include "Logger.h"
int main()
{
    Logger::setOutputFunc(Logger::fileOutputFunc);
    LOG_INFO << "This is a test info message.";
    LOG_ERROR << "This is a test error message.";
    return 0;
}
