/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Terrain Manager definitions.
 */

#include "TerrainManager.hpp"

#include "Common/Common.hpp"

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

    // pass 1 - fill all from 0 to 1/4 of height with stone
    for (int z = 0; z < CHUNK_Z; ++z)
        for (int y = 2; y < CHUNK_Y/4; ++y)
            for (int x = 0; x < CHUNK_X; ++x)
                mChunk.SetVoxel(x, y, z, Voxel::Stone);

    // pass 2 - cut through the terrain with some Perlin-generated caves
    for (int z = 0; z < CHUNK_Z; ++z)
        for (int y = 2; y < CHUNK_Y; ++y)
            for (int x = 0; x < CHUNK_X; ++x)
            {
                // TODO adjust scaling
                double noiseVal = mNoiseGen.Noise(x / 32.0, y / 32.0, z / 16.0);

                if (noiseVal > 0.3)
                    mChunk.SetVoxel(x, y, z, Voxel::Air);
            }

    // force-fill first two layers of the ground with bedrock
    for (int z = 0; z < CHUNK_X; ++z)
        for (int y = 0; y < 2; ++y)
            for (int x = 0; x < CHUNK_Z; ++x)
                mChunk.SetVoxel(x, y, z, Voxel::Bedrock);

    // create Mesh from chunk
    std::vector<float> verts;
    for (int z = 0; z < CHUNK_Z; ++z)
        for (int y = 0; y < CHUNK_Y; ++y)
            for (int x = 0; x < CHUNK_X; ++x)
            {
                Voxel vox = mChunk.GetVoxel(x, y, z);
                if (vox != Voxel::Air)
                {
                    // shifted to keep the chunk in the middle of the scene
                    verts.push_back(static_cast<float>(x - CHUNK_X/2));
                    verts.push_back(static_cast<float>(y - CHUNK_Y/4));
                    verts.push_back(static_cast<float>(z - CHUNK_X/2));

                    if (vox == Voxel::Stone)
                    {
                        verts.push_back(0.7f);
                        verts.push_back(0.7f);
                        verts.push_back(0.7f);
                        verts.push_back(1.0f);
                    }
                    else if (vox == Voxel::Bedrock)
                    {
                        verts.push_back(0.1f);
                        verts.push_back(0.1f);
                        verts.push_back(0.1f);
                        verts.push_back(1.0f);
                    }
                }
            }

    MeshDesc md;
    md.dataPtr = verts.data();
    md.dataSize = verts.size() * sizeof(float);
    md.vertCount = verts.size() / 7; // float count divided by 7 floats per vertex
    mTerrain.Init(md);

    mRenderer.AddMesh(&mTerrain);
}

void TerrainManager::Update() noexcept
{

}
