/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Filesystem utility definitions on Linux
 */

#include "../FileSystem.hpp"
#include "../UTFfuncs.hpp"

#include <memory>
#include <iostream>

#include <unistd.h>
#include <string.h>

namespace {

std::string GetParentDir(const std::string& path)
{
    size_t found = path.find_last_of("/\\");
    return path.substr(0, found);
}

} // namespace

namespace FS {

std::string GetExecutableDir()
{
    std::string linkPath = "/proc/self/exe";
    std::string execPathStr = "";
    char* execPath = realpath(linkPath.data(), nullptr);

    if (!execPath)
        // TODO log
        std::cerr << "Failed to resolve executable's path : " << strerror(errno);
    else
    {
        execPathStr = execPath;
        free(execPath);
    }

    return GetParentDir(execPathStr);
}

void ChangeDirectory(const std::string& dir)
{
    if (::chdir(dir.c_str()) != 0)
    {
        // TODO log
        std::cerr << "Failed to change current directory to '" << dir.c_str()
                  << "': " << strerror(errno);
        // TODO exception
        return;
    }

    // TODO log info
    std::cerr << "Current directory changed to: " << dir.c_str() << std::endl;
}

} // namespace FS
