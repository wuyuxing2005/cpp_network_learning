
#include "Logger.h"
#include "asyncLogger.h"
#include "Buffer.h"
int main()
{
    Logger::setOutputFunc(Logger::asyncOutputFunc);
    LOG_INFO << "This is a test info message.";
    LOG_ERROR << "This is a test error message.";
    return 0;
}
