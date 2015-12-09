/**
* @file
* @author mkkulagowski (mkkulagowski(at)gmail.com)
* @brief  Logger class declaration.
*/

#ifndef __COMMON_LOGGER_HPP__
#define __COMMON_LOGGER_HPP__

#include <sstream>

// Generic log macro. Please use specific log macros, located below.
#define LOG_GENERIC(type, msg)                                        \
do                                                                    \
{                                                                     \
    std::stringstream stream;                                         \
    stream << msg;                                                    \
    Logger::GetInstance().Log(type, __FILE__, __func__,               \
                                __LINE__, stream.str());              \
}while(0)

/**
 * Macros to make logging easier
 * @param msg Streams object. E.x. "this " << 2 << std::endl;
 */
#define LOG_INFO(msg) LOG_GENERIC(LogType::Info, msg)
#define LOG_WARNING(msg) LOG_GENERIC(LogType::Warning, msg)
#define LOG_ERROR(msg) LOG_GENERIC(LogType::Error, msg)
#define LOG(msg) LOG_GENERIC(LogType::Other, msg)

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
     * @param type Type of log message
     * @param file Name of file, where the function is used
     * @param func Name of the function, inside which the function is used
     * @param line Line number, at which this function is used
     * @param message String containing formatted message
     */
    void Log(LogType type, const std::string& file, const std::string& func,
             int line, const std::string& message) const;

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
};

#endif // __COMMON_LOGGER_HPP__
