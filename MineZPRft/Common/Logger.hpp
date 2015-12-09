/**
* @file
* @author mkkulagowski (mkkulagowski(at)gmail.com)
* @brief  Logger class declaration.
*/

#ifndef __COMMON_LOGGER_HPP__
#define __COMMON_LOGGER_HPP__

#include "Timer.hpp"

#include <sstream>

// Generic log macro. Please use specific log macros, located below.
#define LOG_GENERIC(type, msg)                                        \
do                                                                    \
{                                                                     \
    std::stringstream stream;                                         \
    stream << msg;                                                    \
    Logger::GetInstance().Log(type, __FILE__, __func__,               \
                                __LINE__, stream.str());              \
} while(0)

/**
 * Macros to make logging easier
 * @param msg Streams object. E.x. "this " << 2 << std::endl;
 */
#define LOG_I(msg) LOG_GENERIC(LogType::Info, msg)
#define LOG_W(msg) LOG_GENERIC(LogType::Warning, msg)
#define LOG_E(msg) LOG_GENERIC(LogType::Error, msg)

#if defined(NDEBUG)
#define LOG_D(msg) do{} while(0) // For non debug builds turn it off
#else
#define LOG_D(msg) LOG_GENERIC(LogType::Other, msg)
#endif // defined(NDEBUG)

/**
 * Enum class that holds possible types of logs
 */
enum class LogType
{
    Info,
    Error,
    Warning,
    Other,
};

/**
 * Logger class.
 */
class Logger
{
public:
    /**
     * Function used for logging
     * @param type    Type of log message
     * @param file    Name of file, where the function is used
     * @param func    Name of the function, inside which the function is used
     * @param line    Line number, at which this function is used
     * @param message String containing formatted message
     */
    void Log(LogType type, std::string file, const std::string& func,
             int line, const std::string& message);

    /**
     * Set CWD path for Logger. This path will be subtracted from every logged
     * file name.
     * @param currDir String containing current working directory
     */
    void SetCurrentWorkingDir(const std::string& currDir);

    /**
     * Get Logger instance
     */
    static Logger& GetInstance();

private:
    Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    ~Logger();

    const std::string GetDateTime() const;
    const std::string LogTypeToString(LogType type) const;
    uint8_t LogTypeToColor(LogType type) const;
    void PrintColored(LogType type, const std::string& message) const;
    void PrintMessage(LogType type, const std::string& message) const;

    std::string mCurrentWorkingDir;
    Timer mTimer;
};

#endif // __COMMON_LOGGER_HPP__
