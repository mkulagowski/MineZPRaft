/**
* @file
* @author mkkulagowski (mkkulagowski(at)gmail.com)
* @brief  Logger class definition.
*/

#include "Common/Logger.hpp"

#include <ctime>
#include <fstream>
#include <algorithm>
#include <iomanip>

Logger::Logger()
    : mCurrentWorkingDir("unknown")
{
    mTimer.Start();
    std::string infoStr = "MineZPRft built <" + std::string(__DATE__)
        + "> <" + __TIME__ + ">. Logger launched <" + GetDateTime() + ">\n";
    Log(LogType::Info, __FILE__, __func__, __LINE__, infoStr);
}

Logger::~Logger()
{
}

void Logger::Log(LogType type, std::string file, const std::string& func,
                 int line, const std::string& message)
{
    std::string fileStr(file);
    // Convert to lowercase to make paths look uniform
    std::transform(fileStr.begin(), fileStr.end(),
                   fileStr.begin(), ::tolower);

    // If possible, erase CWD from filename
    if (fileStr.find(mCurrentWorkingDir) != std::string::npos)
        file.erase(0, mCurrentWorkingDir.size() + 1);

    // Get time elapsed in seconds since logger creation.
    std::stringstream timeElapsedStream;
    timeElapsedStream << std::setfill(' ') << std::setw(12) << std::fixed
        << std::setprecision(6) << mTimer.Stop();

    // Create output message with the following format:
    // [<time elapsed>] [<type>] <file> <func>@<line>: <msg>
    std::string output = "[" + timeElapsedStream.str() +"] ["
        + LogTypeToString(type) + "] " + file + " " + func + "@"
        + std::to_string(line) + ": " + message;

    // Append to the log file
    std::ofstream logFile;
    logFile.open("MineZPRft.log", std::ios::out | std::ios::app);
    if (logFile.is_open())
        logFile << output;
    logFile.close();

    // Print message to the console
    PrintMessage(type, output);
}

// Get current date/time, format is YYYY-MM-DD HH:mm:ss
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
            return "INFO ";
            break;

        case LogType::Error:
            return "ERROR";
            break;

        case LogType::Warning:
            return "WARN ";
            break;

        case LogType::Other:
            return "OTHER";
            break;

        default:
            return "UNKN ";
    }
}

void Logger::SetCurrentWorkingDir(const std::string& currDir)
{
    mCurrentWorkingDir = currDir;
    std::transform(mCurrentWorkingDir.begin(), mCurrentWorkingDir.end(),
                   mCurrentWorkingDir.begin(), ::tolower);
}

Logger& Logger::GetInstance()
{
    static Logger instance;
    return instance;
}
