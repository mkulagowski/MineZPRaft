/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Terrain Manager definitions.
 */

#define NOMINMAX

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
    mUseGreedyMeshing = desc.useGreedyMeshing;

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

    for (auto& chunk : mChunks)
    {
        if (chunk->IsGenerated())
            chunk->CommitMeshUpdate();
    }

    // Use picking on center chunk and adjacent ones
    if (ray)
    {
        // Final results
        float rayDist = std::numeric_limits<float>::max();
        Vector rayCoords;
        Chunk* rayChunk = nullptr;

        // Temporary results
        float tempDist = std::numeric_limits<float>::max();
        Vector tempCoords;

        // For each out of 5 middle chunks calculate picking.
        // Final result will be voxel with smallest distance from players eyes.
        for (int i = 0; i < 4; ++i)
            if (!mChunks[i]->NeedsGeneration())
                if (mChunks[i]->ChunkRayIntersection(pos, dir, tempDist, tempCoords))
                    if (tempDist < rayDist)
                    {
                        rayDist = tempDist;
                        rayCoords = tempCoords;
                        rayChunk = mChunks[i];
                    }

        // If any voxel was picked, turn it into Bedrock, so we can see it
        if (rayChunk != nullptr)
        {
            rayChunk->SetVoxel(static_cast<size_t>(rayCoords[0]),
                               static_cast<size_t>(rayCoords[1]),
                               static_cast<size_t>(rayCoords[2]),
                               VoxelType::Bedrock);
            if (mUseGreedyMeshing)
                rayChunk->GenerateVBOGreedy();
            else
                rayChunk->GenerateVBONaive();

             LOG_D("Ray intersection done. Chunk found!" << " Voxel["
                   << rayCoords[0] << "," << rayCoords[1] << "," << rayCoords[2]
                   << "]. Distance = " << rayDist << ".");
        }
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
                                               xChunk, zChunk, mCurrentChunkX, mCurrentChunkZ,
                                               mUseGreedyMeshing));
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
