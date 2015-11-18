#include "GameWindow.hpp"

GameWindow::GameWindow()
    : WindowManager()
{
}

void GameWindow::OnKeyPress(int key)
{
    std::string keyPressed = std::to_string(key);
    // wsad - movement
    // q,e - switch cube types
    // shift - faster movement
}

void GameWindow::OnMouseDown(uint32_t button, int x, int y)
{
    (void)button;
    (void)x;
    (void)y;
    // draw LAZOR to see where to create/delete cube
}

void GameWindow::OnMouseMove(int x, int y, int deltaX, int deltaY)
{
    (void)x;
    (void)y;
    (void)deltaX;
    (void)deltaY;
    // moves camera and movement direction
}

void GameWindow::OnMouseUp(uint32_t button)
{
    (void)button;
    // LMB - create cube
    // RMB - delete cube
}