/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Filesystem utility declarations
 */

#pragma once

#include <string>

namespace FS {

/**
 * Extract current Executable Directory using OS-specific functions
 *
 * @return Full path to program executable
 */
std::string GetExecutableDir();

/**
 * Change current working directory to @p dir
 *
 * @param dir Directory to which cwd will be changed.
 */
void ChangeDirectory(const std::string& dir);

} // namespace FS
