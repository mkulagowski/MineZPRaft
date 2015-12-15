/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Terrain Manager definitions.
 */

#include "TerrainManager.hpp"

#include "Common/Common.hpp"

namespace
{

const int HEIGHTMAP_HEIGHT = 16;
const double AIR_THRESHOLD = 0.3;
const int FLOAT_COUNT_PER_VERTEX = 7;
const float ALPHA_COMPONENT = 1.0f; // Alpha color component should stay at 1,0 (full opacity).

} // namespace

TerrainManager::TerrainManager()
    : mNoiseGen(NoiseGenerator::GetInstance())
    , mRenderer(Renderer::GetInstance())
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

    // TODO log info
    std::cerr << "Generating terrain...";

    // Further "generation loops" will assume that bottom two layers of chunk are
    // filled with Bedrock, so their Y iterator will begin from 2.

    // stage 1 - fill all from 2 to 1/4 of height with stone
    for (int z = 0; z < CHUNK_Z; ++z)
        for (int y = 2; y < CHUNK_Y / 4; ++y)
            for (int x = 0; x < CHUNK_X; ++x)
                mChunk.SetVoxel(x, y, z, VoxelType::Stone);

    // stage 2.1 - generate a heightmap using Perlin
    double noise;
    std::vector<double> heightMap;
    for (int z = 0; z < CHUNK_Z; ++z)
        for (int x = 0; x < CHUNK_X; ++x)
        {
            // TODO adjust scaling
            noise = mNoiseGen.Noise(x / 16.0, 0.0, z / 16.0);

            // Noise-returned values span -1..1 range,
            // Add 1 to them to convert it to 0..2 range.
            noise += 1.0;

            // To make pass 2.2 easier, convert values from range 0..2 to 0..HEIGHTMAP_HEIGHT
            noise *= HEIGHTMAP_HEIGHT / 2;

            // Dump the data inside vector for further analysis
            heightMap.push_back(noise);
        }

    // stage 2.2 - convert generated heightmap to voxel chunk
    for (int z = 0; z < CHUNK_Z; ++z)
        for (int y = CHUNK_Y / 4; y < (CHUNK_Y / 4) + HEIGHTMAP_HEIGHT; ++y)
            for (int x = 0; x < CHUNK_X; ++x)
            {
                // Add a voxel if noise value is higher than currently processed voxel's height
                if (heightMap[x * CHUNK_Z + z] >= static_cast<double>(y - (CHUNK_Y / 4)))
                    mChunk.SetVoxel(x, y, z, VoxelType::Stone);
            }

    // stage 3 - cut through the terrain with some Perlin-generated caves
    for (int z = 0; z < CHUNK_Z; ++z)
        for (int y = 2; y < CHUNK_Y; ++y)
            for (int x = 0; x < CHUNK_X; ++x)
            {
                // TODO adjust scaling
                // reuse noise variable from before to get new noise information
                noise = mNoiseGen.Noise(x * 0.1, z * 0.1, y * 0.1);

                if (noise > AIR_THRESHOLD)
                    mChunk.SetVoxel(x, y, z, VoxelType::Air);
            }

    // stage 4 - force-fill first two layers of the ground with bedrock
    for (int z = 0; z < CHUNK_Z; ++z)
        for (int y = 0; y < 2; ++y)
            for (int x = 0; x < CHUNK_X; ++x)
                mChunk.SetVoxel(x, y, z, VoxelType::Bedrock);

    // stage 5 - create Mesh from chunk
    std::vector<float> verts;
    for (int z = 0; z < CHUNK_Z; ++z)
        for (int y = 0; y < CHUNK_Y; ++y)
            for (int x = 0; x < CHUNK_X; ++x)
            {
                VoxelType vox = mChunk.GetVoxel(x, y, z);
                if (vox != VoxelType::Air && vox != VoxelType::Unknown)
                {
                    // Shifted to keep the chunk in the middle of the scene.
                    // Because (for performance issues) we fill the chunk only to 1/4 of its size,
                    // (plus the heightmap) we shift the voxels to CHUNK_Y / 4 + HEIGHTMAP_HEIGHT
                    verts.push_back(static_cast<float>(x - CHUNK_X / 2));
                    verts.push_back(static_cast<float>(y - (CHUNK_Y / 4 + HEIGHTMAP_HEIGHT)));
                    verts.push_back(static_cast<float>(z - CHUNK_Z / 2));

                    const Voxel& voxData = VoxelDB.find(vox)->second;
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
    mTerrain.Init(md);

    mRenderer.AddMesh(&mTerrain);

    // Inform that the terrain has finally been generated.
    std::cerr << "DONE" << std::endl;
}

void TerrainManager::Update() noexcept
{

}
