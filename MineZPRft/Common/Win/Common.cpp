/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Commonly used definitions under Windows
 */

#include "../Common.hpp"

#include <Windows.h>

std::string GetLastErrorString()
{
    DWORD lastError = ::GetLastError();
    char buffer[256];
    ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, lastError,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buffer, 255, NULL);
    return buffer;
}
