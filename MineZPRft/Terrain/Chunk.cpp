/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Chunk methods definition.
 */

#include "Chunk.hpp"

#include <iostream>

Chunk::Chunk()
{
    for (int i = 0; i < CHUNK_X*CHUNK_Y*CHUNK_Z; ++i)
        mVoxels[i] = Voxel::Air;
}

Chunk::~Chunk()
{
}

void Chunk::SetVoxel(size_t x, size_t y, size_t z, Voxel voxel) noexcept
{
    // calculate the index
    size_t index = 0;
    if (!CalculateIndex(x, y, z, index))
        return;

    mVoxels[index] = voxel;
}

Voxel Chunk::GetVoxel(size_t x, size_t y, size_t z) noexcept
{
    // calculate the index
    size_t index = 0;
    if (!CalculateIndex(x, y, z, index))
        return Voxel::Unknown;

    return mVoxels[index];
}

bool Chunk::CalculateIndex(size_t x, size_t y, size_t z, size_t& index) noexcept
{
     size_t result = x * CHUNK_Y * CHUNK_Z + y * CHUNK_Z + z;
     if ((result > CHUNK_X * CHUNK_Y * CHUNK_Z) ||
         (x > CHUNK_X) || (y > CHUNK_Y) || (z > CHUNK_Z))
     {
         // TODO log warning
         std::cerr << "Chunk coordinates [" << x << ", " << y << ", " << z
                   << "] exceed available Chunk dimensions! (which are ["
                   << x << ", " << y << ", " << z << "])" << std::endl;
         return false;
     }

    index = result;
    return true;
}
