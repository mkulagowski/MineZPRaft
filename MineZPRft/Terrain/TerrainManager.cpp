/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Terrain Manager definitions.
 */

#include "TerrainManager.hpp"

#include "Common/Common.hpp"
#include "Common/Logger.hpp"
#include "Renderer/Renderer.hpp"


TerrainManager::TerrainManager()
    : mCurrentChunkX(0)
    , mCurrentChunkZ(0)
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

    // Temporary chunks to generate a + pattern as a temporary map.
    // Dynamic multichunking will replace this with an array of chunks.
    mChunk.Generate(0, 0, mCurrentChunkX, mCurrentChunkZ);
    mChunk2.Generate(1, 0, mCurrentChunkX, mCurrentChunkZ);
    mChunk3.Generate(-1, 0, mCurrentChunkX, mCurrentChunkZ);
    mChunk4.Generate(0, 1, mCurrentChunkX, mCurrentChunkZ);
    mChunk5.Generate(0, -1, mCurrentChunkX, mCurrentChunkZ);

    // corners
    mChunk6.Generate(1, 1, mCurrentChunkX, mCurrentChunkZ);
    mChunk7.Generate(1,-1, mCurrentChunkX, mCurrentChunkZ);
    mChunk8.Generate(-1,1, mCurrentChunkX, mCurrentChunkZ);
    mChunk9.Generate(-1,-1, mCurrentChunkX, mCurrentChunkZ);

    Renderer::GetInstance().AddMesh(mChunk.GetMeshPtr());
    Renderer::GetInstance().AddMesh(mChunk2.GetMeshPtr());
    Renderer::GetInstance().AddMesh(mChunk3.GetMeshPtr());
    Renderer::GetInstance().AddMesh(mChunk4.GetMeshPtr());
    Renderer::GetInstance().AddMesh(mChunk5.GetMeshPtr());
    Renderer::GetInstance().AddMesh(mChunk6.GetMeshPtr());
    Renderer::GetInstance().AddMesh(mChunk7.GetMeshPtr());
    Renderer::GetInstance().AddMesh(mChunk8.GetMeshPtr());
    Renderer::GetInstance().AddMesh(mChunk9.GetMeshPtr());

    LOG_I("Done generating terrain.");
}

void TerrainManager::Update(int chunkX, int chunkZ) noexcept
{
    if ((mCurrentChunkX != chunkX) || (mCurrentChunkZ != chunkZ))
    {
        mCurrentChunkX = chunkX;
        mCurrentChunkZ = chunkZ;

        mChunk.Generate(chunkX,      chunkZ, mCurrentChunkX, mCurrentChunkZ);
        mChunk2.Generate(chunkX + 1, chunkZ, mCurrentChunkX, mCurrentChunkZ);
        mChunk3.Generate(chunkX - 1, chunkZ, mCurrentChunkX, mCurrentChunkZ);
        mChunk4.Generate(chunkX, chunkZ + 1, mCurrentChunkX, mCurrentChunkZ);
        mChunk5.Generate(chunkX, chunkZ - 1, mCurrentChunkX, mCurrentChunkZ);

        // corners
        mChunk6.Generate(chunkX + 1, chunkZ + 1, mCurrentChunkX, mCurrentChunkZ);
        mChunk7.Generate(chunkX + 1, chunkZ - 1, mCurrentChunkX, mCurrentChunkZ);
        mChunk8.Generate(chunkX - 1, chunkZ + 1, mCurrentChunkX, mCurrentChunkZ);
        mChunk9.Generate(chunkX - 1, chunkZ - 1, mCurrentChunkX, mCurrentChunkZ);
    }
}
