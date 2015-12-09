/**
 * @file
 * @author mkkulagowski (mkkulagowski(at)gmail.com)
 * @brief  Main function of the solution.
 */

#include "GameManager.hpp"

#include "Common/FileSystem.hpp"
#include "Common/Logger.hpp"

int main()
{
    // NOTE logs from here on will have their filename unstripped

    // Navigate to project root directory
    FS::ChangeDirectory(FS::GetExecutableDir() + "/../../..");

    // Give logger CWD to enable file path stripping
    Logger::GetInstance().SetCurrentWorkingDir(FS::GetCurrentWorkingDir());

    // NOTE logs from here on will have their filename stripped

    // Start the main game loop
    GameManager::GetInstance().GameLoop();

    return 0;
}