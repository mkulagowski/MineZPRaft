/**
 * @file
 * @author mkkulagowski (mkkulagowski(at)gmail.com)
 * @brief  GameManager class implementation.
 */

#include "GameManager.hpp"

GameManager::GameManager()
    : mPlayer()
    , mWindow(&mPlayer)
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
    desc.shaderPath = "Data/Shaders";
    desc.windowWidth = static_cast<GLsizei>(width);
    desc.windowHeight = static_cast<GLsizei>(height);
    mRenderer.Init(desc);

    // forward camera from Renderer to mPlayer
    mPlayer.Init(mRenderer.GetCameraPtr());
}

GameManager::~GameManager()
{
    if (!mWindow.IsClosed())
        mWindow.Close();
}

GameManager& GameManager::GetInstance()
{
    static GameManager instance;
    return instance;
}

void GameManager::GameLoop()
{
    mFrameTimer.Start();
    while (!mWindow.IsClosed())
    {
        double frameTime = mFrameTimer.Stop();
        mFrameTimer.Start();

        mWindow.ProcessMessages();
        mWindow.Update(frameTime);

        // TODO terrain manager will have to update itself here
        mRenderer.Draw();

        mWindow.SwapBuffers();
    }
}

