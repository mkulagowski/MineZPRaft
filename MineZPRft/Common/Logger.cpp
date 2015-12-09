/**
* @file
* @author mkkulagowski (mkkulagowski(at)gmail.com)
* @brief  Logger class definition.
*/

#include "Common/Logger.hpp"

#include <ctime>
#include <fstream>

Logger::Logger()
{
}

Logger::~Logger()
{
}

void Logger::Log(LogType type, const std::string& file, const std::string& func,
                 int line, const std::string& message) const
{
    // Create output message with the following format:
    // <timestamp> [<type>] <file> <func>@<line>: <msg>
    std::string output = GetDateTime() + " [" + LogTypeToString(type) + "] "
        + file + " " + func + "@" + std::to_string(line) + ": " + message;

    // Append to the log file
    std::ofstream logFile;
    logFile.open("MineZPRft.log", std::ios::out | std::ios::app);
    if (logFile.is_open())
        logFile << output << std::endl;
    logFile.close();

    // Print message to the console
    PrintMessage(type, output);
}

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
const std::string Logger::GetDateTime() const
{
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];

    // localtime is deprecated on Win, yet localtime_s is not visible for Unix
#if defined(WIN32)
    localtime_s(&tstruct, &now);
#elif defined(__LINUX__) | defined(__linux__)
    tstruct = *localtime(&now);
#endif // defined(WIN32)

    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);

    return buf;
}

const std::string Logger::LogTypeToString(LogType type) const
{
    switch (type)
    {
        case LogType::Info:
            return "INFO";
            break;

        case LogType::Error:
            return "ERROR";
            break;

        case LogType::Warning:
            return "WARNING";
            break;

        case LogType::Other:
            return "OTHER";
            break;

        default:
            return "UNKNOWN";
    }
}

Logger& Logger::GetInstance()
{
    static Logger instance;
    return instance;
}
