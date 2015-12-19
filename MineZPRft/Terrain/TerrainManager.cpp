/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Terrain Manager definitions.
 */

#include "TerrainManager.hpp"

#include "Common/Common.hpp"
#include "Common/Logger.hpp"


TerrainManager::TerrainManager()
    : mRenderer(Renderer::GetInstance())
{
}

TerrainManager::~TerrainManager()
{
}

TerrainManager& TerrainManager::GetInstance()
{
    static TerrainManager instance;
    return instance;
}

void TerrainManager::Init(const TerrainDesc& desc)
{
    UNUSED(desc);

    LOG_I("Generating terrain...");

    mChunk.Generate(0, 0);
    mChunk2.Generate(1, 0);
    mChunk3.Generate(-1, 0);
    mChunk4.Generate(0, 1);
    mChunk5.Generate(0, -1);

    LOG_I("Done generating terrain.");
}

void TerrainManager::Update() noexcept
{

}
