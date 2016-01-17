/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Filesystem utility definitions
 */

#include "../FileSystem.hpp"
#include "../Common.hpp"
#include "../UTFfuncs.hpp"
#include "Common/Logger.hpp"

#include <memory>
#include <iostream>

#include <Windows.h>
#include <direct.h>

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
    std::unique_ptr<TCHAR[]> execPath;
    DWORD sizeRead = 0;

    // Maximum length of a relative paths, available in Windows
    unsigned int len = MAX_PATH;

    // Maximum length of a path, available in Windows
    const unsigned int maxPathWide = 32768;

    for (; len < maxPathWide; len *= 2)
    {
        execPath.reset(new TCHAR[len]);
        sizeRead = ::GetModuleFileName(nullptr, execPath.get(), len);

        if (sizeRead < len && sizeRead != 0)
            break;
    }

    // Check if the buffer did not overflow,
    // if not - convert to UTF8 and check result
    if (len >= maxPathWide)
    {
        LOG_E("Failed to resolve executable's path : %s"
                  << GetLastErrorString());
        // TODO exception
        return "";
    }

    return GetParentDir(execPath.get());
}

void ChangeDirectory(const std::string& dir)
{
    if (::SetCurrentDirectory(dir.c_str()) == 0)
    {
        LOG_E("Failed to change directory to '" << dir
                  << "': " << GetLastErrorString());
        // TODO exception
        return;
    }

    LOG_I("CWD changed to: " << dir.c_str());
}

std::string GetCurrentWorkingDir()
{
    char currPath[MAX_PATH];
    ::_getcwd(currPath, MAX_PATH);
    if (!currPath)
    {
        LOG_W("Current working directory longer than MAX_PATH.");
        return "";
    }

    std::string currPathStr(currPath);
    return currPathStr;
}

bool CreateDir(const std::string& path)
{
    if (::CreateDirectory(path.c_str(), nullptr) == 0)
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
    DWORD attrs = ::GetFileAttributes(path.c_str());

    if (attrs == INVALID_FILE_ATTRIBUTES)
        return false;

    if (attrs & FILE_ATTRIBUTE_DIRECTORY)
        return true;

    return false;
}

} // namespace FS
