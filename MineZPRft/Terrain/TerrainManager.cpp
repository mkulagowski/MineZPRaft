/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Terrain Manager definitions.
 */

#include "TerrainManager.hpp"

#include "Common/Common.hpp"
#include "Common/Logger.hpp"
#include "Renderer/Renderer.hpp"

#include <functional>
#include <thread>


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
    // Set chunk amount
    mChunkCount = CalculateChunkCount(desc.visibleRadius);
    LOG_D("Chunk count: " << mChunkCount << " on radius " << desc.visibleRadius);
    mChunks.resize(mChunkCount);
    mVisibleRadius = desc.visibleRadius;

    LOG_I("Generating terrain...");

    // Reserve some space in Renderer
    Renderer::GetInstance().ReserveTerrainMeshPool(mChunkCount);

    // Generate chunks (this will push tasks to do for generator thread)
    GenerateChunks();

    LOG_I("Done generating terrain.");
}

void TerrainManager::Update(int chunkX, int chunkZ, Vector pos, Vector dir,
                            bool ray) noexcept
{
    if ((mCurrentChunkX != chunkX) || (mCurrentChunkZ != chunkZ))
    {
        mCurrentChunkX = chunkX;
        mCurrentChunkZ = chunkZ;

        GenerateChunks();
    }
    if (ray)
        LOG_I("Ray intersect TBD");

    for (auto& chunk : mChunks)
    {
        if (chunk->IsGenerated())
            chunk->CommitMeshUpdate();
    }

    float rayDistance = 10000.0f;
    Vector rayCoords;
    // Use picking to get voxel we point to
    if (ray && !mChunks[0]->NeedsGeneration())
    {
        float tempDistance = rayDistance;
        Vector tempCoords;
        bool rayFound = mChunks[0]->TestOBBRay(pos, dir, rayDistance, rayCoords);
        if (rayFound)
            mChunks[0]->SetVoxel(rayCoords[0], rayCoords[1], rayCoords[2],
                              VoxelType::Bedrock);

        LOG_I("Ray intersection done. Chunk "
                << (rayFound ? "not found." : "found.")
                << " Voxel[" << rayCoords[0] << "," << rayCoords[1] << "," << rayCoords[2]
                << "]. Distance = " << rayDistance << ".");
    }
}

void TerrainManager::GenerateChunks()
{
    // Add chunk generation to pool for separate thread.
    unsigned int chunkIndex = 0;
    for (unsigned int i = 0; i <= mVisibleRadius; ++i)
    {
        // initialize the values for currently processed radius
        int xChunk = i;
        int zChunk = 0;
        GeneratorState state = GeneratorState::ZIncXDec;
        unsigned int chunksInRadius;

        if (i == 0)
            chunksInRadius = 1;
        else
            chunksInRadius = i * 4;

        for (unsigned int j = 0; j < chunksInRadius; ++j)
        {
            // Get chunk pointer from pool
            Chunk* chunk = mChunkPool.GetChunk(mCurrentChunkX + xChunk,
                                               mCurrentChunkZ + zChunk);
            mChunks[chunkIndex] = chunk;

            // Generate the Chunk if needed
            if (chunk->NeedsGeneration())
            {
                chunk->ResetState();
                mGeneratorQueue.Push(std::bind(&Chunk::Generate, mChunks[chunkIndex],
                                               xChunk, zChunk, mCurrentChunkX, mCurrentChunkZ));
            }

            chunk->Shift(xChunk, zChunk);

            // Send our chunk to Renderer
            Renderer::GetInstance().ReplaceTerrainMesh(chunkIndex, chunk->GetMeshPtr());

            chunkIndex++;
            ShiftChunkCoords(xChunk, zChunk, state);
        }
    }

    // create a detached thread which will do the tasks in parallel
    if (!mGeneratorQueue.IsEmpty())
    {
        std::thread generatorThread([this]() {
            mGeneratorQueue.EmptyWait();
        });
        generatorThread.detach();
    }
}

unsigned int TerrainManager::CalculateChunkCount(unsigned int radius)
{
    if (radius == 0)
        return 1;
    else
        return radius * 4 + CalculateChunkCount(radius - 1);
}

void TerrainManager::ShiftChunkCoords(int& xChunk, int& zChunk, GeneratorState& state)
{
    // Switch xChunk and zChunk according to GeneratorState
    switch (state)
    {
    case GeneratorState::ZIncXDec:
        xChunk--;
        zChunk++;
        if (xChunk == 0)
            state = GeneratorState::ZDecXDec;
        break;
    case GeneratorState::ZDecXDec:
        xChunk--;
        zChunk--;
        if (zChunk == 0)
            state = GeneratorState::ZDecXInc;
        break;
    case GeneratorState::ZDecXInc:
        xChunk++;
        zChunk--;
        if (xChunk == 0)
            state = GeneratorState::ZIncXInc;
        break;
    case GeneratorState::ZIncXInc:
        xChunk++;
        zChunk++;
        if (zChunk == 0)
            state = GeneratorState::ZIncXDec;
        break;
    }
}
