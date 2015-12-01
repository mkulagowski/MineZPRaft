/**
 * @file
 * @author LKostyra
 * @brief  Definitions of keycodes used as input, platform-specific
 */

#ifndef __GAME_GAMEKEYCODES_H__
#define __GAME_GAMEKEYCODES_H__

#if defined(WIN32)

#include <Windows.h>

/// TODO BEFORE PUSH replace with Windows keycodes
/// keyboard key codes
enum KeyCode
{
    W = XK_w,
    S = XK_s,
    A = XK_a,
    D = XK_d,
    LShift = VK_SHIFT
};

/// mouse key codes
#define RMB 1

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
#define RMB 2

#endif // WIN32

#endif // __GAME_GAMEKEYCODES_H__
