/**
 * @file
 * @author mkkulagowski (mkkulagowski(at)gmail.com)
 * @brief  Exception template class declarations and definitions.
 */

#ifndef __COMMON_EXCEPTIONS_HPP__
#define __COMMON_EXCEPTIONS_HPP__

#include "ExceptionType.hpp"

#include <cstdarg>
#include <memory>
#include <exception>
#include <string>

 // ==================================
 // Declarations
 // ==================================

/**
 * Macro used for throwing exceptions with more information.
 * Thanks to the behaviour of macros,
 * __FILE__ and __LINE__ can be used successfully.
 */
#define THROW(type, ... ) throw(Exception<type>(__FILE__, __LINE__,  \
                                                __VA_ARGS__))

/**
 * Dummy class to let us catch all types of template exceptions
 */
class AnyException : public std::exception
{
};


/**
 * Template class used for exceptions
 * All exception types are defined in ExceptionType.hpp
 */
template<ExceptionType T>
class Exception : public AnyException
{
public:
    Exception();
    Exception(std::string file, int line, std::string msg, ...);
    ~Exception();

    /**
     * Get error message
     */
    const char* what() const noexcept;

private:
    std::string mType;
    std::string mFile;
    int mLine;
    std::string mMsg;

    void init(int msgLength, std::string msg, va_list args);
};


// ==================================
// Definitions
// ==================================

template<ExceptionType T>
Exception<T>::Exception()
    : mType(ExceptionHelper<T>::type)
{
    mMsg = mType + '\n';
}

template<ExceptionType T>
Exception<T>::Exception(std::string file, int line, std::string msg, ...)
    : mFile(file)
    , mLine(line)
    , mType(ExceptionHelper<T>::type)
{
    va_list ap;

    // Check message length
    va_start(ap, msg);
    int msgLength = vsnprintf(nullptr, 0, msg.c_str(), ap);
    va_end(ap);

    // Write formatted message to the mMsg
    va_start(ap, msg);
    init(msgLength, msg, ap);
    va_end(ap);
}

template<ExceptionType T>
Exception<T>::~Exception()
{
}

template<ExceptionType T>
void Exception<T>::init(int msgLength, std::string msg, va_list args)
{
    // Get formatted message from arguments
    std::unique_ptr<char[]> formatted(new char[msgLength]);

    vsnprintf(formatted.get(), msgLength, msg.c_str(), args);

    // Message format: <exception_type> at <file>@<line>: <message_from_ctor>
    mMsg = std::string(mType + " at " + mFile + '@' + std::to_string(mLine)
                       + ": " + formatted.get());
    if (mMsg.back() != '\n')
        mMsg += '\n';
}

template<ExceptionType T>
const char* Exception<T>::what() const noexcept
{
    return mMsg.c_str();
}

#endif // __COMMON_EXCEPTIONS_HPP__