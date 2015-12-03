/**
 * @file
 * @author mkkulagowski (mkkulagowski(at)gmail.com)
 * @brief  Exception class definition.
 */

#include "Exception.hpp"


Exception::Exception()
    :mMsg("Exception")
{
}

Exception::Exception(const std::string& file, int line, const std::string& msg)
    :Exception()
{
    init(file, line, msg);
}

Exception::~Exception()
{
}

void Exception::init(const std::string& file, int line, const std::string& msg)
{
    // Message format: <exception_type> at <file>@<line>: <message_from_ctor>
    mMsg += " at " + file + '@' + std::to_string(line) + ": " + msg;
    if (mMsg.back() != '\n')
        mMsg += '\n';
}

const char* Exception::what() const noexcept
{
    return mMsg.c_str();
}
