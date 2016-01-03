/**
* @file
* @author mkkulagowski (mkkulagowski(at)gmail.com)
* @brief  Logger class OS specific methods definition.
*/

#include "Common/Logger.hpp"

#include <iostream>
#include <Windows.h>

uint8_t Logger::LogTypeToColor(LogType type) const
{
    switch (type)
    {
        case LogType::Info:
            // Blue
            return 0x01;
            break;

        case LogType::Error:
            // Red
            return 0x04;
            break;

        case LogType::Warning:
            // Yellow
            return 0x06;
            break;

        case LogType::Debug:
            // Cyan
            return 0x03;
            break;

        default:
            // White
            return 0x07;
    }
}

void Logger::PrintColored(LogType type, const std::string& message) const
{
    HANDLE hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO console_info;

    // Get current console settings
    GetConsoleScreenBufferInfo(hstdout, &console_info);

    // Set up new color
    SetConsoleTextAttribute(hstdout, 0x08 | LogTypeToColor(type));

    //Print colored message
    std::cout << message << std::flush;

    // Revert console to previous settings
    SetConsoleTextAttribute(hstdout, console_info.wAttributes);
}

void Logger::PrintMessage(LogType type, const std::string& message) const
{
    // Write to the console
    PrintColored(type, message);

    // Write to OutputDebugString
    OutputDebugString(message.c_str());
}
