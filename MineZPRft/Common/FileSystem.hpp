/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Filesystem utility declarations
 */

#ifndef __COMMON_FILESYSTEM_HPP__
#define __COMMON_FILESYSTEM_HPP__

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

/**
 * Extract current working directory.
 */
std::string GetCurrentWorkingDir();

/**
 * Create @p dir
 *
 * @param dir Directory which will be created.
 */
bool CreateDir(const std::string& path);

/**
 * Check if @p is a directory.
 *
 * @param dir Directory to check.
 */
bool IsDir(const std::string& path);

} // namespace FS

#endif // __COMMON_FILESYSTEM_HPP__