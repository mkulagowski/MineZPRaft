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

    // TODO log info
    std::cerr << "Generating terrain";
    
    // pass 1 - fill all from 2 to 1/4 of height with stone
    for (int z = 0; z < CHUNK_Z; ++z)
        for (int y = 2; y < CHUNK_Y/4; ++y)
            for (int x = 0; x < CHUNK_X; ++x)
                mChunk.SetVoxel(x, y, z, Voxel::Stone);

    std::cerr << ".";

    // pass 2 - generate a heightmap using Perlin
    std::vector<double> heightMap;
    for (int z = 0; z < CHUNK_Z; ++z)
        for (int x = 0; x < CHUNK_X; ++x)
        {
            // TODO adjust scaling
            double noise = mNoiseGen.Noise(x / 16.0, 0.0, z / 16.0);
            noise += 1.0;
            noise *= 8.0;
            heightMap.push_back(noise);
        }

    for (int z = 0; z < CHUNK_Z; ++z)
        for (int y = CHUNK_Y/4; y < (CHUNK_Y/4)+16; ++y)
            for (int x = 0; x < CHUNK_X; ++x)
            {
                // TODO adjust scaling
                if (heightMap[x*CHUNK_Z + z] >= static_cast<double>(y - (CHUNK_Y/4)))
                    mChunk.SetVoxel(x, y, z, Voxel::Stone);
            }

    std::cerr << ".";

    
    // pass 2 - cut through the terrain with some Perlin-generated caves
    for (int z = 0; z < CHUNK_Z; ++z)
        for (int y = 2; y < CHUNK_Y; ++y)
            for (int x = 0; x < CHUNK_X; ++x)
            {
                // TODO adjust scaling
                double noiseVal = mNoiseGen.Noise(x * 0.1, z * 0.1, y * 0.1);

                if (noiseVal > 0.3)
                    mChunk.SetVoxel(x, y, z, Voxel::Air);
            }

    std::cerr << ".";

    // force-fill first two layers of the ground with bedrock
    for (int z = 0; z < CHUNK_X; ++z)
        for (int y = 0; y < 2; ++y)
            for (int x = 0; x < CHUNK_Z; ++x)
                mChunk.SetVoxel(x, y, z, Voxel::Bedrock);

    std::cerr << ".";

    // create Mesh from chunk
    std::vector<float> verts;
    for (int z = 0; z < CHUNK_Z; ++z)
        for (int y = 0; y < CHUNK_Y; ++y)
            for (int x = 0; x < CHUNK_X; ++x)
            {
                Voxel vox = mChunk.GetVoxel(x, y, z);
                if (vox != Voxel::Air && vox != Voxel::Unknown)
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

    std::cerr << ".";

    MeshDesc md;
    md.dataPtr = verts.data();
    md.dataSize = verts.size() * sizeof(float);
    md.vertCount = verts.size() / 7; // float count divided by 7 floats per vertex
    mTerrain.Init(md);

    mRenderer.AddMesh(&mTerrain);

    std::cerr << "DONE" << std::endl;
}

void TerrainManager::Update() noexcept
{

}
