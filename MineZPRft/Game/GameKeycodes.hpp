/**
 * @file
 * @author LKostyra
 * @brief  Definitions of keycodes used as input, platform-specific
 */

#ifndef __GAME_GAMEKEYCODES_H__
#define __GAME_GAMEKEYCODES_H__

#if defined(WIN32)

#include <Windows.h>

/// keyboard key codes
enum Key
{
    /// On Windows regular keys match ASCII-table values of capital letters
    W = 'W',
    S = 'S',
    A = 'A',
    D = 'D',

    /// All special signs are Virtual Key-Codes
    /// Taken from https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx
    LShift = VK_SHIFT
};

/// mouse key codes
enum Mouse
{
    RMB = 1
};

#elif defined(__linux__) | defined(__LINUX__)

/// Used keyboard key codes
enum Key
{
    W = 25,
    S = 39,
    A = 38,
    D = 40,
    LShift = 50
};

/// mouse key codes
enum Mouse
{
    RMB = 2,
};

#endif // WIN32



#endif // __GAME_GAMEKEYCODES_H__
