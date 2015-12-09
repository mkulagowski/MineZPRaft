/**
* @file
* @author mkkulagowski (mkkulagowski(at)gmail.com)
* @brief  Logger class OS specific methods definition.
*/

#include "Common/Logger.hpp"

#include <iostream>

uint8_t Logger::LogTypeToColor(LogType type) const
{
    switch (type)
    {
        case LogType::Info:
            // Blue
            return 0x04;
            break;

        case LogType::Error:
            // Red
            return 0x01;
            break;

        case LogType::Warning:
            // Yellow
            return 0x03;
            break;

        case LogType::Other:
            // Cyan
            return 0x06;
            break;

        default:
            // White
            return 0x07;
    }
}

void Logger::PrintColored(LogType type, const std::string& message) const
{
    // Prepare color output using escape sequence
    std::string colorString = "\033[1;";
    colorString += std::to_string(0x1E + LogTypeToColor(type)) + "m";

    // Print colored message
    std::cout << colorString <<  message;

    // Revert console to previous settings
    colorString = "\033[0m";
    std::cout << colorString << std::flush;
}

void Logger::PrintMessage(LogType type, const std::string& message) const
{
    // Write to the console
    PrintColored(type, message);
}