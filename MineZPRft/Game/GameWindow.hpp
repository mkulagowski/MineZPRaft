/**
 * @file
 * @author mkkulagowski (mkkulagowski(at)gmail.com)
 * @brief  GameWindow class declaration.
 */

#ifndef __GAME_GAMEWINDOW_HPP__
#define __GAME_GAMEWINDOW_HPP__

#include "../Common/Window.hpp"
#include <utility>

#if defined(__LINUX__) | defined(__linux__)
#define RMB 2
#elif defined(WIN32)
#define RMB 1
#endif

class GameWindow : public WindowManager
{
public:
    GameWindow();

private:
    void OnKeyPress(int key);
    void OnMouseDown(uint32_t button, int x, int y);
    void OnMouseMove(int x, int y, int deltaX, int deltaY);
    void OnMouseUp(uint32_t button);
};

#endif // __GAME_GAMEWINDOW_HPP__
