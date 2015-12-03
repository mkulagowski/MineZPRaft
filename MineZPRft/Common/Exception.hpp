/**
 * @file
 * @author mkkulagowski (mkkulagowski(at)gmail.com)
 * @brief  Exception template class declarations and definitions.
 */

#ifndef __COMMON_EXCEPTIONS_HPP__
#define __COMMON_EXCEPTIONS_HPP__

#include <exception>
#include <string>
#include <cstdarg>
/**
 * Macro used for throwing exceptions with more information.
 * Thanks to the behaviour of macros,
 * __FILE__ and __LINE__ can be used successfully.
 */
#define THROW(type, ... ) throw(type(__FILE__, __LINE__, __VA_ARGS__))

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

class Exception : public AnyException
{
public:
    Exception();
    Exception(const std::string& file, int line, const std::string msg, ...);
    ~Exception();

    /**
     * Get error message
     */
    const char* what() const noexcept;
    void init(int msgLength, const std::string& file, int line, const std::string& msg, va_list ap);

protected:
    std::string mMsg;
};

// DEFINE
#define DECLARE_EXCEPTION(newEx, baseEx)                                 \
class newEx : public baseEx                                              \
{                                                                        \
public:                                                                  \
    newEx() { mMsg = #newEx; };                                          \
    newEx(const std::string& file, int line, const std::string msg, ...) \
    : newEx() { va_list ap; va_start(ap, msg);                           \
                int msgLength = vsnprintf(nullptr, 0, msg.c_str(), ap);  \
                va_end(ap); va_start(ap, msg);                           \
                init(msgLength, file, line, msg, ap); va_end(ap); };     \
    ~newEx() {};                                                         \
}

// LIFE EXAMPLE SIMILAR TO DEFINE
class MathException : public Exception
{
public:
    MathException() { mMsg = "MathException"; };
    MathException(const std::string& file, int line, const std::string msg, ...)
        : MathException()
    {
        va_list ap;
        va_start(ap, msg);
        int msgLength = vsnprintf(nullptr, 0, msg.c_str(), ap);
        va_end(ap);
        va_start(ap, msg);
        init(msgLength, file, line, msg, ap);
        va_end(ap);
    };
    ~MathException() {};
};

// USED DEFINE
DECLARE_EXCEPTION(newException, Exception);

#endif // __COMMON_EXCEPTIONS_HPP__