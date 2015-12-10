/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Filesystem utility definitions on Linux
 */

#include "../FileSystem.hpp"
#include "../UTFfuncs.hpp"
#include "../Common.hpp"
#include "../Logger.hpp"

#include <memory>
#include <iostream>
#include <unistd.h>

namespace
{

std::string GetParentDir(const std::string& path)
{
    size_t found = path.find_last_of("/\\");
    return path.substr(0, found);
}

} // namespace

namespace FS
{

std::string GetExecutableDir()
{
    std::string linkPath = "/proc/self/exe";
    std::string execPathStr = "";
    char* execPath = realpath(linkPath.data(), nullptr);

    if (!execPath)
        LOG_E("Failed to resolve executable's path : " << GetLastErrorString() << std::endl);
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
        LOG_E("Failed to change current directory to '" << dir
                  << "': " << GetLastErrorString() << std::endl);
        // TODO exception
        return;
    }

    LOG_I("Current directory changed to: " << dir << std::endl);
}

std::string GetCurrentWorkingDir()
{
    char* currPath = getcwd(nullptr, 0);
    if (!currPath)
    {
        LOG_W("Error while getting current working directory." << std::endl);
        return "";
    }

    std::string currPathStr(currPath);
    free(currPath);

    return currPathStr;
}

} // namespace FS
