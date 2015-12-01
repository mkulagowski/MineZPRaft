/**
 * @file
 * @author mkkulagowski (mkkulagowski(at)gmail.com)
 * @brief  GameWindow class declaration.
 */

#pragma once

#include "../Common/Window.hpp"

#include "GamePlayer.hpp"

#include <utility>

#if defined(__LINUX__) | defined(__linux__)
#define RMB 2
#elif defined(WIN32)
#define RMB 1
#endif

class GameWindow : public WindowManager
{
public:
    GameWindow(GamePlayer* playerPtr);

    void Update();

private:
    void OnKeyPress(int key);
    void OnMouseDown(uint32_t button, int x, int y);
    void OnMouseMove(int x, int y, int deltaX, int deltaY);
    void OnMouseUp(uint32_t button);

    GamePlayer* mPlayerPtr;
};
