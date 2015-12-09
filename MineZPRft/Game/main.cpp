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
    // Navigate to project root directory
    FS::ChangeDirectory(FS::GetExecutableDir() + "/../../..");

    // Give logger CWD, so the logs look nicer
    Logger::GetInstance().SetCurrentWorkingDir(FS::GetCurrentWorkingDir());

    // Start the main game loop
    GameManager::GetInstance().GameLoop();

    return 0;
}