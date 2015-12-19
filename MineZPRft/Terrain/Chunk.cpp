/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Chunk methods definition.
 */

#include "Chunk.hpp"

#include "Common/Logger.hpp"
#include "NoiseGenerator.hpp"
#include "Renderer/Renderer.hpp"


namespace
{

const int HEIGHTMAP_HEIGHT = 16;
const double AIR_THRESHOLD = 0.3;
const int FLOAT_COUNT_PER_VERTEX = 7;
const float ALPHA_COMPONENT = 1.0f; // Alpha color component should stay at 1,0 (full opacity).

} // namespace


Chunk::Chunk()
{
    for (int i = 0; i < CHUNK_X * CHUNK_Y * CHUNK_Z; ++i)
        mVoxels[i] = VoxelType::Air;
}

Chunk::~Chunk()
{
}

void Chunk::SetVoxel(size_t x, size_t y, size_t z, VoxelType voxel) noexcept
{
    // calculate the index
    size_t index = 0;
    if (!CalculateIndex(x, y, z, index))
        return;

    mVoxels[index] = voxel;
}

VoxelType Chunk::GetVoxel(size_t x, size_t y, size_t z) noexcept
{
    // calculate the index
    size_t index = 0;
    if (!CalculateIndex(x, y, z, index))
        return VoxelType::Unknown;

    return mVoxels[index];
}

void Chunk::Generate(int chunkX, int chunkZ) noexcept
{
    NoiseGenerator& noiseGen = NoiseGenerator::GetInstance();

    // Further "generation loops" will assume that bottom two layers of chunk are
    // filled with Bedrock, so their Y iterator will begin from 2.

    // Stage 1 - fill bottom quarter of chunk with stone
    for (int z = 0; z < CHUNK_Z; ++z)
        for (int y = 2; y < CHUNK_Y / 4; ++y)
            for (int x = 0; x < CHUNK_X; ++x)
                SetVoxel(x, y, z, VoxelType::Stone);

    LOG_D("  Chunk [" << chunkX << ", " << chunkZ << "] Stage 1 done");

    // Stage 2.1 - generate a heightmap using Perlin
    double noise;
    std::vector<double> heightMap;
    for (int z = 0; z < CHUNK_Z; ++z)
        for (int x = 0; x < CHUNK_X; ++x)
        {
            // TODO adjust scaling
            // Noise arguments are shifted according to Chunk::Generate() arguments.
            // This way the map will be seamless and the chunks connected.
            noise = noiseGen.Noise((x + (CHUNK_X * chunkX)) / 16.0,
                                   0.0,
                                   (z + (CHUNK_Z * chunkZ)) / 16.0);

            // Noise-returned values span -1..1 range,
            // Add 1 to them to convert it to 0..2 range.
            noise += 1.0;

            // To make Stage 2.2 easier, convert values from range 0..2 to 0..HEIGHTMAP_HEIGHT
            noise *= HEIGHTMAP_HEIGHT / 2;

            // Dump the data inside vector for further analysis
            heightMap.push_back(noise);
        }

    // Stage 2.2 - convert generated heightmap to stone voxels
    for (int z = 0; z < CHUNK_Z; ++z)
        for (int y = CHUNK_Y / 4; y < (CHUNK_Y / 4) + HEIGHTMAP_HEIGHT; ++y)
            for (int x = 0; x < CHUNK_X; ++x)
            {
                // Add a stone voxel if heightmap's value is higher
                // than currently processed voxel's Y coordinate.
                if (heightMap[x * CHUNK_Z + z] >= static_cast<double>(y - (CHUNK_Y / 4)))
                    SetVoxel(x, y, z, VoxelType::Stone);
            }

    LOG_D("  Chunk [" << chunkX << ", " << chunkZ << "] Stage 2 done");

    // Stage 3 - cut through the terrain with some Perlin-generated caves
    for (int z = 0; z < CHUNK_Z; ++z)
        for (int y = 2; y < CHUNK_Y; ++y)
            for (int x = 0; x < CHUNK_X; ++x)
            {
                // TODO adjust scaling
                // NOTE chunkZ applies to X coordinate and chunkX applies to Z coordinate.
                //      Otherwise, the chunk would be rotated and the map would lost its
                //      seamlessness.
                noise = noiseGen.Noise((x + CHUNK_Z * chunkZ) * 0.1,
                                        y * 0.1,
                                       (z + CHUNK_X * chunkX) * 0.1);

                if (noise > AIR_THRESHOLD)
                    SetVoxel(x, y, z, VoxelType::Air);
            }

    LOG_D("  Chunk [" << chunkX << ", " << chunkZ << "] Stage 3 done");

    // Stage 4 - force-fill first two layers of the ground with bedrock
    for (int z = 0; z < CHUNK_Z; ++z)
        for (int y = 0; y < 2; ++y)
            for (int x = 0; x < CHUNK_X; ++x)
                SetVoxel(x, y, z, VoxelType::Bedrock);

    LOG_D("  Chunk [" << chunkX << ", " << chunkZ << "] Stage 4 done");

    // Stage 5 - create Mesh from chunk
    std::vector<float> verts;
    for (int z = 0; z < CHUNK_Z; ++z)
        for (int y = 0; y < CHUNK_Y / 4 + HEIGHTMAP_HEIGHT; ++y)
            for (int x = 0; x < CHUNK_X; ++x)
            {
                VoxelType vox = GetVoxel(x, y, z);
                if (vox != VoxelType::Air && vox != VoxelType::Unknown)
                {
                    auto voxDataIt = VoxelDB.find(vox);
                    if (voxDataIt == VoxelDB.end())
                    {
                        // TODO log error
                        LOG_E("Voxel " << static_cast<VoxelUnderType>(vox)
                              << " was not found in database!");
                        continue;
                    }

                    // Because (for performance issues) we fill the chunk only to 1/4 of its size,
                    // (plus the heightmap) we shift the voxels to CHUNK_Y / 4 + HEIGHTMAP_HEIGHT
                    // NOTE Like mentioned in Stage 3, for seamlessness purposes chunkZ refers to X
                    //      coordinate and chunk X refers to Z coordinate shift.
                    verts.push_back(static_cast<float>(x + (chunkZ * CHUNK_Z)));
                    verts.push_back(static_cast<float>(y - (CHUNK_Y / 4 + HEIGHTMAP_HEIGHT)));
                    verts.push_back(static_cast<float>(z + (chunkX * CHUNK_X)));

                    const Voxel& voxData = voxDataIt->second;
                    verts.push_back(voxData.colorRed);
                    verts.push_back(voxData.colorGreen);
                    verts.push_back(voxData.colorBlue);
                    verts.push_back(ALPHA_COMPONENT);
                }
            }

    MeshDesc md;
    md.dataPtr = verts.data();
    md.dataSize = verts.size() * sizeof(float);
    md.vertCount = verts.size() / FLOAT_COUNT_PER_VERTEX;
    mMesh.Init(md);

    Renderer::GetInstance().AddMesh(&mMesh);

    // Inform that the terrain has finally been generated.
    LOG_D("  Chunk [" << chunkX << ", " << chunkZ << "] Stage 5 done");
}

bool Chunk::CalculateIndex(size_t x, size_t y, size_t z, size_t& index) noexcept
{
    if ((x > CHUNK_X) || (y > CHUNK_Y) || (z > CHUNK_Z))
    {
        LOG_W("Chunk coordinates [" << x << ", " << y << ", " << z
              << "] exceed available Chunk dimensions! (which are ["
              << CHUNK_X << ", " << CHUNK_Y << ", " << CHUNK_Z << "])");
        return false;
    }

    // Convert 3D coordinates to a 1D array index. This way the one-dimensional array,
    // which is easily accessible by Renderer, can be used as a 3D array.
    index = x * CHUNK_Y * CHUNK_Z + y * CHUNK_Z + z;
    return true;
}
