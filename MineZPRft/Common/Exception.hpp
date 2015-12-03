/**
 * @file
 * @author mkkulagowski (mkkulagowski(at)gmail.com)
 * @brief  Exception template class declarations and definitions.
 */

#ifndef __COMMON_EXCEPTIONS_HPP__
#define __COMMON_EXCEPTIONS_HPP__

#include <exception>
#include <string>


/**
 * Macro used for throwing exceptions with more information.
 * Thanks to the behaviour of macros,
 * __FILE__ and __LINE__ can be used successfully.
 */
#define THROW(type, msg) throw(type(__FILE__, __LINE__, msg))


/**
 * Class used for exceptions
 */
class Exception : public std::exception
{
public:
    Exception();
    Exception(const std::string& file, int line, const std::string& msg);
    ~Exception();

    /**
     * Get error message
     */
    const char* what() const noexcept;
    void init(const std::string& file, int line, const std::string& msg);

protected:
    std::string mMsg;
};

/**
 * Macro used for declaring new exception subclasses
 */
#define DECLARE_EXCEPTION(baseEx, newEx)                                 \
class newEx : public baseEx                                              \
{                                                                        \
public:                                                                  \
    newEx() { mMsg = #newEx; };                                          \
    newEx(const std::string& file, int line, const std::string& msg)     \
    : newEx() { init(file, line, msg); };                                \
    ~newEx() {};                                                         \
}

// New exceptions declarations
DECLARE_EXCEPTION(Exception, MathException);

#endif // __COMMON_EXCEPTIONS_HPP__