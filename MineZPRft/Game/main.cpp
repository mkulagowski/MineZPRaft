/**
 * @file
 * @author mkkulagowski (mkkulagowski(at)gmail.com)
 * @brief  Main function of the solution.
 */

#include "GameManager.hpp"

#include "Common/FileSystem.hpp"

int main()
{
    FS::ChangeDirectory(FS::GetExecutableDir() + "/../../..");

    GameManager::GetInstance().GameLoop();

    return 0;
}