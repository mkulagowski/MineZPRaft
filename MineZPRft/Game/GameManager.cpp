#include "GameManager.hpp"

GameManager::GameManager()
    : pWindow(new GameWindow)
//  , pTerrainMgr(new TerrainManager)
{
    pWindow->SetSize(800, 600);
    pWindow->SetTitle("MineZPRaft Game Window");
}

GameManager::~GameManager()
{
    if (!pWindow->IsClosed())
        pWindow->Close();
}

GameManager& GameManager::GetInstance()
{
    static GameManager instance;
    return instance;
}

void GameManager::GameLoop()
{
    pWindow->Open();
    while (!pWindow->IsClosed())
    {
        pWindow->ProcessMessages();
     // pTerrMgr->UpdateMap(pWindow->GetPlayerPosition());
    }
    pWindow->Close();
}

