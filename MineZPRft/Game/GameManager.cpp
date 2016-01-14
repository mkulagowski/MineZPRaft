/**
 * @file
 * @author mkkulagowski (mkkulagowski(at)gmail.com)
 * @brief  GameManager class implementation.
 */

#include "GameManager.hpp"
#include "Common/FPSCounter.hpp"

GameManager::GameManager()
    : mFrameTimer()
    , mPlayer()
    , mPlayerChunkX(0)
    , mPlayerChunkZ(0)
    , mWindow(&mPlayer)
    , mRenderer(Renderer::GetInstance())
    , mTerrain(TerrainManager::GetInstance())
{
    // TODO: Add possibility to change the resolution
    mWindow.SetSize(800, 600);
    mWindow.SetTitle("MineZPRft Game Window");

    // We must open the window so OGL context will be initialized
    // Otherwise, Renderer will not be able to start on Windows due to OGL Extensions.
    mWindow.Open();

    uint32_t width, height;
    mWindow.GetSize(width, height);
    RendererDesc desc;
    desc.shaderPath = SHADER_DIR;
    desc.windowWidth = static_cast<GLsizei>(width);
    desc.windowHeight = static_cast<GLsizei>(height);
    mRenderer.Init(desc);

    // forward camera from Renderer to mPlayer
    mPlayer.Init(mRenderer.GetCameraPtr());

    TerrainDesc td;
    td.visibleRadius = 7;
    mTerrain.Init(td);
}

GameManager::~GameManager()
{
}

GameManager& GameManager::GetInstance()
{
    static GameManager instance;
    return instance;
}

void GameManager::GameLoop()
{
    mFrameTimer.Start();

    double frameTime;
    FPSCounter frameCounter;
    while (!mWindow.IsClosed())
    {
        frameTime = mFrameTimer.Stop();
        mFrameTimer.Start();

        frameCounter.Run();

        mWindow.ProcessMessages();
        mWindow.Update(frameTime);
        mWindow.SetTitle(("MineZPRft [" +
                          std::to_string(frameCounter.GetAvgFrameCount()) +
                          " FPS]").c_str());

        CalculatePlayerChunk();
        mTerrain.Update(mPlayerChunkX, mPlayerChunkZ);
        mRenderer.Draw();

        mWindow.SwapBuffers();
    }
}

void GameManager::CalculatePlayerChunk()
{
    const Vector& PlayerPos = mPlayer.GetPosition();
    Vector shift;

    float boundX = static_cast<float>(CHUNK_X / 2);
    float boundZ = static_cast<float>(CHUNK_Z / 2);

    if (PlayerPos[0] > boundX)
    {
        shift -= Vector(static_cast<float>(CHUNK_X), 0.0f, 0.0f, 0.0f);
        mPlayerChunkX--;
    }

    if (PlayerPos[0] < -boundX)
    {
        shift += Vector(static_cast<float>(CHUNK_X), 0.0f, 0.0f, 0.0f);
        mPlayerChunkX++;
    }

    if (PlayerPos[2] > boundZ)
    {
        shift -= Vector(0.0f, 0.0f, static_cast<float>(CHUNK_Z), 0.0f);
        mPlayerChunkZ--;
    }

    if (PlayerPos[2] < -boundZ)
    {
        shift += Vector(0.0f, 0.0f, static_cast<float>(CHUNK_Z), 0.0f);
        mPlayerChunkZ++;
    }

    if (!(shift == Vector()))
    {
        mPlayer.SetPosition(PlayerPos + shift);
        mPlayer.Update();
        LOG_D("Shifted to chunk: [" << mPlayerChunkX << ", " << mPlayerChunkZ << "]");
    }
}
