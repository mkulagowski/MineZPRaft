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

    float verts[] =
    {
    // Structure in this temporary VBO is following:
    //  pos.xyz,          color.rgba
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    };

    MeshDesc md;
    md.dataPtr = verts;
    md.dataSize = sizeof(verts);
    mTerrain.Init(md);

    mRenderer.AddMesh(&mTerrain);
}

void TerrainManager::Update() noexcept
{

}
