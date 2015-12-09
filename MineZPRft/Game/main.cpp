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
    // Note: logs from here on will have their filename unstripped

    // Navigate to project root directory
    FS::ChangeDirectory(FS::GetExecutableDir() + "/../../..");

    // Give logger CWD, so the logs from here on have their filename stripped
    Logger::GetInstance().SetCurrentWorkingDir(FS::GetCurrentWorkingDir());

    // Note: logs from here on will have their filename stripped

    // Start the main game loop
    GameManager::GetInstance().GameLoop();

    return 0;
}