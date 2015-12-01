/**
 * @file
 * @author mkkulagowski (mkkulagowski(at)gmail.com)
 * @brief  GameManager class declaration.
 */

#ifndef __GAME_GAMEMANAGER_HPP__
#define __GAME_GAMEMANAGER_HPP__

#include "GameWindow.hpp"
#include "Renderer/Renderer.hpp"
#include "Common/Timer.hpp"

#include <memory>

/**
 * Game manager class.
 */
class GameManager
{
public:
    /**
     * Run main game loop
     */
    void GameLoop();

    /**
     * Get GameManager instance
     */
    static GameManager& GetInstance();

private:
    GameManager();
    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;
    ~GameManager();

    Timer mFrameTimer;
    GamePlayer mPlayer;
    GameWindow mWindow;
    Renderer mRenderer;
};

#endif // __GAME_GAMEMANAGER_HPP__
