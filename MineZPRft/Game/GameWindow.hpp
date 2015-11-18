#pragma once
#include "../Common/Window.hpp"

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