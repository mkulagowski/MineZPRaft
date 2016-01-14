/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Terrain Manager declaration.
 */

#include "ChunkPool.hpp"


ChunkPool::ChunkPool()
{
}

ChunkPool::~ChunkPool()
{
}

void ChunkPool::SetRadiusToCover(unsigned int radius)
{
    (void)radius;
}

Chunk* ChunkPool::GetChunk(int x, int z)
{
    ChunkKeyType key(x, z);

    auto chunkIt = mChunks.find(key);
    if (chunkIt == mChunks.end())
    {
        // chunk not found - add it to the pool and reacquire the iterator
        mChunks.emplace(std::make_pair(key, Chunk()));
        chunkIt = mChunks.find(key); // now we should be sure the Chunk exists in the pool
        // Chunk will be preinitialized due to copy constructor
        // TODO maybe this should be fixed/changed to be more intuitive? If there is a way...
    }

    return &chunkIt->second;
}
