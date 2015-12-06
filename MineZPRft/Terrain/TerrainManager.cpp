/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Terrain Manager definitions.
 */

#include "TerrainManager.hpp"

#include "Common/Common.hpp"
#include "Common/Logger.hpp"

namespace
{

const int HEIGHTMAP_HEIGHT = 16;
const double AIR_THRESHOLD = 0.3;
const int FLOAT_COUNT_PER_VERTEX = 7;

// TODO temporary components for Stone and Bedrock.
//      MUST be replaced with Voxel Database.
const float STONE_COLOR_COMPONENT = 0.7f;
const float BEDROCK_COLOR_COMPONENT = 0.1f;
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

    LOG_I("Generating terrain...");

    // Further "generation loops" will assume that bottom two layers of chunk are
    // filled with Bedrock, so their Y iterator will begin from 2.

    // Stage 1 - fill bottom quarter of chunk with stone
    for (int z = 0; z < CHUNK_Z; ++z)
        for (int y = 2; y < CHUNK_Y / 4; ++y)
            for (int x = 0; x < CHUNK_X; ++x)
                mChunk.SetVoxel(x, y, z, Voxel::Stone);

    LOG_D("  Terrain Stage 1 done");

    // Stage 2.1 - generate a heightmap using Perlin
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
                    mChunk.SetVoxel(x, y, z, Voxel::Stone);
            }

    LOG_D("  Terrain Stage 2 done");

    // Stage 3 - cut through the terrain with some Perlin-generated caves
    for (int z = 0; z < CHUNK_Z; ++z)
        for (int y = 2; y < CHUNK_Y; ++y)
            for (int x = 0; x < CHUNK_X; ++x)
            {
                // TODO adjust scaling
                // reuse noise variable from before to get new noise information
                noise = mNoiseGen.Noise(x * 0.1, z * 0.1, y * 0.1);

                if (noise > AIR_THRESHOLD)
                    mChunk.SetVoxel(x, y, z, Voxel::Air);
            }

    LOG_D("  Terrain Stage 3 done");

    // Stage 4 - force-fill first two layers of the ground with bedrock
    for (int z = 0; z < CHUNK_Z; ++z)
        for (int y = 0; y < 2; ++y)
            for (int x = 0; x < CHUNK_X; ++x)
                mChunk.SetVoxel(x, y, z, Voxel::Bedrock);

    LOG_D("  Terrain Stage 4 done");

    // Stage 5 - create Mesh from chunk
    std::vector<float> verts;
    for (int z = 0; z < CHUNK_Z; ++z)
        for (int y = 0; y < CHUNK_Y; ++y)
            for (int x = 0; x < CHUNK_X; ++x)
            {
                Voxel vox = mChunk.GetVoxel(x, y, z);
                if (vox != Voxel::Air && vox != Voxel::Unknown)
                {
                    // Shifted to keep the chunk in the middle of the scene.
                    // Because (for performance issues) we fill the chunk only to 1/4 of its size,
                    // (plus the heightmap) we shift the voxels to CHUNK_Y / 4 + HEIGHTMAP_HEIGHT
                    verts.push_back(static_cast<float>(x - CHUNK_X / 2));
                    verts.push_back(static_cast<float>(y - (CHUNK_Y / 4 + HEIGHTMAP_HEIGHT)));
                    verts.push_back(static_cast<float>(z - CHUNK_Z / 2));

                    // TODO temporary solution to distinguish two available voxel types.
                    //      Replace with receiving voxel color from Voxel DB.
                    if (vox == Voxel::Stone)
                    {
                        verts.push_back(STONE_COLOR_COMPONENT);
                        verts.push_back(STONE_COLOR_COMPONENT);
                        verts.push_back(STONE_COLOR_COMPONENT);
                        verts.push_back(ALPHA_COMPONENT);
                    }
                    else if (vox == Voxel::Bedrock)
                    {
                        verts.push_back(BEDROCK_COLOR_COMPONENT);
                        verts.push_back(BEDROCK_COLOR_COMPONENT);
                        verts.push_back(BEDROCK_COLOR_COMPONENT);
                        verts.push_back(ALPHA_COMPONENT);
                    }
                }
            }

    MeshDesc md;
    md.dataPtr = verts.data();
    md.dataSize = verts.size() * sizeof(float);
    md.vertCount = verts.size() / FLOAT_COUNT_PER_VERTEX;
    mTerrain.Init(md);

    mRenderer.AddMesh(&mTerrain);

    // Inform that the terrain has finally been generated.
    LOG_D("  Terrain Stage 5 done");
    LOG_I("Done generating terrain.");
}

void TerrainManager::Update() noexcept
{

}
