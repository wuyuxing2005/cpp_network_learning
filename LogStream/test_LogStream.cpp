#include "LogStream.h"

#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

static std::string bufferToString(const FixedBuffer &buffer)
{
    return std::string(buffer.getData(), buffer.len());
}

static void testFixedBuffer()
{
    FixedBuffer buffer;

    assert(buffer.len() == 0);
    assert(buffer.avail() == MAX_STREAM_LEN);

    buffer.append("hello", 5);
    assert(buffer.len() == 5);
    assert(bufferToString(buffer) == "hello");

    buffer.append(" world", 6);
    assert(bufferToString(buffer) == "hello world");

    buffer.clear();
    buffer.resetCur();
    assert(buffer.len() == 0);
}

static void testFmt()
{
    fmt padded("%04d", 42);
    assert(std::string(padded.getData(), padded.getLen()) == "0042");

    fmt fixed("%.2f", 3.14159);
    assert(std::string(fixed.getData(), fixed.getLen()) == "3.14");
}

static void testLogStreamGetData(const std::string &outputPath)
{
    LogStream stream;

    stream << true << ' '
           << false << ' '
           << 123 << ' '
           << -456 << ' '
           << 12.5 << ' '
           << 1.25f << ' '
           << "hello" << ' '
           << std::string("world") << ' '
           << fmt("%03d", 7);

    const std::string expected = "true false 123 -456 12.5 1.25 hello world 007";
    const std::string actual(stream.getData(), stream.getLen());
    assert(actual == expected);
    assert(stream.getLen() == static_cast<int>(expected.size()));

    std::ofstream output(outputPath);
    assert(output.is_open());
    output.write(stream.getData(), stream.getLen());
    output << std::endl;

    stream.resetBuffer();
    stream << static_cast<const char *>(nullptr);
    assert(std::string(stream.getData(), stream.getLen()) == "(null)");
}

int main()
{
    testFixedBuffer();
    testFmt();
    testLogStreamGetData("cpp_network_learning/LogStream/logstream_output.txt");

    std::cout << "LogStream tests passed" << std::endl;
    return 0;
}
