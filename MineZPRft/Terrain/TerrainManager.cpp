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

    std::vector<float> verts;

    for (int z = 0; z < 32; ++z)
        for (int y = 0; y < 64; ++y)
            for (int x = 0; x < 64; ++x)
            {
                double noiseVal = mNoiseGen.Noise(x / 32.0, y / 32.0, z / 16.0);

                if (noiseVal < 0.2)
                {
                    verts.push_back(static_cast<float>(x));
                    verts.push_back(static_cast<float>(z));
                    verts.push_back(static_cast<float>(y));
                    verts.push_back(0.5f);
                    verts.push_back(0.5f);
                    verts.push_back(0.5f);
                    verts.push_back(1.0f);
                }
            }

    MeshDesc md;
    md.dataPtr = verts.data();
    md.dataSize = verts.size() * sizeof(float);
    md.vertCount = verts.size() / 7;
    mTerrain.Init(md);

    mRenderer.AddMesh(&mTerrain);
}

void TerrainManager::Update() noexcept
{

}
