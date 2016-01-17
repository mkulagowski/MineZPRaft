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
#include <sys/stat.h>
#include <sys/types.h>
#include <cstring>

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
    char* execPath = ::realpath(linkPath.data(), nullptr);

    if (!execPath)
        LOG_E("Failed to resolve executable's path : " << GetLastErrorString());
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
                  << "': " << GetLastErrorString());
        // TODO exception
        return;
    }

    LOG_I("Current directory changed to: " << dir);
}

std::string GetCurrentWorkingDir()
{
    char* currPath = getcwd(nullptr, 0);
    if (!currPath)
    {
        LOG_W("Error while getting current working directory.");
        return "";
    }

    std::string currPathStr(currPath);
    free(currPath);

    return currPathStr;
}

bool CreateDir(const std::string& path)
{
    if (::mkdir(path.c_str(), 0777) != 0)
    {
        LOG_E("Failed to create directory '" << path << "' : "
                  << GetLastErrorString());
        return false;
    }

    LOG_I("Created directory '" << path << "'");
    return true;
}

bool IsDir(const std::string& path)
{
    struct stat st;
    if (::stat(path.c_str(), &st) != 0)
        return false;

    if (S_ISDIR(st.st_mode))
        return true;

    return false;
}

} // namespace FS
