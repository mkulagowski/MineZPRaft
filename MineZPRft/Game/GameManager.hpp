#pragma once

#include "GameWindow.hpp"
#include <memory>

class GameManager
{
public:
    void GameLoop();
    static GameManager& GetInstance();

private:
    GameManager();
    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;
    ~GameManager();

    std::unique_ptr<GameWindow> pWindow;
    //std::unique_ptr<TerrainManager> pTerrainMgr;
    //std::unique_ptr<Renderer> pRenderer;
};
