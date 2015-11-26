/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Filesystem utility definitions
 */

#include "../FileSystem.hpp"
#include "../UTFfuncs.hpp"

#include <memory>
#include <iostream>

#include <Windows.h>

namespace {

// TODO it might be useful somewhere else, consider moving to common header
std::string GetLastErrorString()
{
    DWORD lastError = ::GetLastError();
    char buffer[256];
    ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, lastError,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buffer, 255, NULL);
    return buffer;
}

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
    unsigned int len = MAX_PATH; // Maximum length of a relative paths, available in Windows
    const unsigned int maxPathWide = 32768; // Maximum length of a path, available in Windows

    for (; len < maxPathWide; len *= 2)
    {
        execPath.reset(new TCHAR[len]);
        sizeRead = GetModuleFileName(nullptr, execPath.get(), len);

        if (sizeRead < len && sizeRead != 0)
            break;
    }

    // Check if the buffer did not overflow, if not - convert to UTF8 and check result
    if (len >= maxPathWide)
    {
        // TODO log
        std::cerr << "Failed to resolve executable's path : %s" << GetLastErrorString().c_str() << std::endl;
        // TODO exception
        return "";
    }

    return GetParentDir(execPath.get());
}

void ChangeDirectory(const std::string& dir)
{
    if (::SetCurrentDirectory(dir.c_str()) == 0)
    {
        // TODO log
        std::cerr << "Failed to change directory to '" << dir.c_str()
                  << "': " << GetLastErrorString().c_str() << std::endl;
        // TODO exception
        return;
    }

    // TODO log info
    std::cerr << "Current directory changed to: " << dir.c_str() << std::endl;
}

} // namespace FS
