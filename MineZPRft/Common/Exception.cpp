/**
 * @file
 * @author mkkulagowski (mkkulagowski(at)gmail.com)
 * @brief  Exception template class definition.
 */

#include "Exception.hpp"

#include <memory>


Exception::Exception()
    :mMsg("Exception")
{
}

Exception::Exception(const std::string& file, int line, const std::string msg, ...)
    :Exception()
{
    va_list ap;
    va_start(ap, msg);
    int msgLength = vsnprintf(nullptr, 0, msg.c_str(), ap);
    va_end(ap);
    va_start(ap, msg);
    init(msgLength, file, line, msg, ap);
    va_end(ap);
}

Exception::~Exception()
{
}

void Exception::init(int msgLength, const std::string& file, int line, const std::string& msg, va_list ap)
{
    // Get formatted message from arguments
    std::unique_ptr<char[]> formatted(new char[msgLength]);

    vsnprintf(formatted.get(), msgLength, msg.c_str(), ap);

    // Message format: <exception_type> at <file>@<line>: <message_from_ctor>
    mMsg = std::string(mMsg + " at " + file + '@' + std::to_string(line)
                       + ": " + formatted.get());
    if (mMsg.back() != '\n')
        mMsg += '\n';
}


const char* Exception::what() const noexcept
{
    return mMsg.c_str();
}
