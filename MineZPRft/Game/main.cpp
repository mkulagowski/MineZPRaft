/**
 * @file
 * @author mkkulagowski (mkkulagowski(at)gmail.com)
 * @brief  Main function of the solution.
 */

#include "GameManager.hpp"

#include "Common/FileSystem.hpp"
#include "Common/Logger.hpp"
#include "Common/Common.hpp"

int main(int argc, char * argv[])
{
    // NOTE logs from here on will have their filename unstripped

    // Navigate to project root directory
    FS::ChangeDirectory(FS::GetExecutableDir() + "/../../..");

    // Give logger CWD to enable file path stripping
    Logger::GetInstance().SetCurrentWorkingDir(FS::GetCurrentWorkingDir());

    // NOTE logs from here on will have their filename stripped

    // Check for 2 numeric args. If given, use them as window size.
    if (argc == 3 && IsNumeric(argv[1]) && IsNumeric(argv[2]))
    {
        int w = std::stoi(argv[1]);
        int h = std::stoi(argv[2]);
        GameManager::GetInstance().GetWindow()->SetSize(w, h);
    }

    // Start the main game loop
    GameManager::GetInstance().GameLoop();

    return 0;
}
