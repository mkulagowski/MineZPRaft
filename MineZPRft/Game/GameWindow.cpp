/**
 * @file
 * @author mkkulagowski (mkkulagowski(at)gmail.com)
 * @brief  GameWindow class implementation.
 */

#include "GameWindow.hpp"
#include "../Common/Common.hpp"
#include "Math/Vector.hpp"

GameWindow::GameWindow(GamePlayer* playerPtr)
    : mPlayerPtr(playerPtr)
{
}

void GameWindow::Update()
{
    Vector playerShift;

    // get and calculate axes relative to Player's position and orientation
    // TODO probably there is no need to recalculate these vectors.
    //      They can be easily taken from Camera object hidden inside Player.
    //      Needs investigation and rewriting.
    const Vector& pos = mPlayerPtr->GetPosition();
    const Vector& dir = mPlayerPtr->GetDirection();

    Vector front = dir - pos;
    front.Normalize();
    Vector right = front.Cross(mPlayerPtr->GetUp());
    right.Normalize();

    if (IsKeyPressed(25))
        playerShift += front;
    if (IsKeyPressed(39))
        playerShift -= front;
    if (IsKeyPressed(38))
        playerShift -= right;
    if (IsKeyPressed(40))
        playerShift += right;

    playerShift.Normalize();
    // TODO get delta time and scale playerShift according to it
    playerShift *= 0.1f;

    mPlayerPtr->Update(playerShift);
}

void GameWindow::OnKeyPress(int key)
{
    std::string title = "Pressed " + std::to_string(key);
    SetTitle(title.c_str());

    // q,e - switch cube types
    // shift - faster movement
}

void GameWindow::OnMouseDown(uint32_t button, int x, int y)
{
    UNUSED(button);
    UNUSED(x);
    UNUSED(y);
    // draw LAZOR to see where to create/delete cube
}

void GameWindow::OnMouseMove(int x, int y, int deltaX, int deltaY)
{
    UNUSED(x);
    UNUSED(y);
    UNUSED(deltaX);
    UNUSED(deltaY);
    // moves camera and movement direction
}

void GameWindow::OnMouseUp(uint32_t button)
{
    UNUSED(button);
    // TODO: if OnMouseDown is not triggered constantly while holding
    //       the button, then move the below over there
    // LMB - create cube
    // RMB - delete cube
}
