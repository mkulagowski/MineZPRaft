/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Commonly used definitions on Linux
 */

#include "../Common.hpp"

#include <string.h>

std::string GetLastErrorString()
{
    return strerror(errno);
}

bool isNumeric(char const *string)
{
    return std::all_of(string, string + strlen(string),
                       [](unsigned char c) { return ::isdigit(c); });
}
