/**
 * @file
 * @author mkkulagowski (mkkulagowski(at)gmail.com)
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Commonly used declarations.
 */

#pragma once

#include <string>

#ifdef WIN32
#include <Windows.h>
#endif


// Macro for unused parameters to supress warnings in skeletons
#define UNUSED(x) (void)x


/**
 * Acquire last error String from OS-specific API.
 *
 * @return Last error in string.
 */
std::string GetLastErrorString();
