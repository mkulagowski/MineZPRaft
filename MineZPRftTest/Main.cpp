/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Main file for MineZPRft Tests
 */

#include <gtest/gtest.h>
#include <Common/FileSystem.hpp>

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);

    FS::ChangeDirectory(FS::GetExecutableDir() + "/../../..");
    int result = RUN_ALL_TESTS();

#if defined(WIN32) && defined(_CRTDBG_MAP_ALLOC)
    _CrtDumpMemoryLeaks();
#endif // defined(WIN32) && defined(_CRTDBG_MAP_ALLOC)

    return result;
}
