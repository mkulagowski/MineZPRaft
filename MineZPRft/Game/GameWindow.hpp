/**
 * @file
 * @author mkkulagowski (mkkulagowski(at)gmail.com)
 * @brief  GameWindow class declaration.
 */

#ifndef __GAME_GAMEWINDOW_HPP__
#define __GAME_GAMEWINDOW_HPP__

#include "../Common/Window.hpp"

#include "GamePlayer.hpp"
#include "Renderer/Renderer.hpp"

#include <utility>

class GameWindow : public WindowManager
{
public:
    GameWindow(GamePlayer* playerPtr);

    /**
     * Update events which are to happen every frame.
     *
     * @param deltaTime Time taken to render a frame. Used to make calculations FPS-independent.
     *
     * The function takes over all events which should happen every frame - movement, reacting to
     * held buttons on keyboard, recalculating new player position etc.
     *
     * @remarks The function will be called in a Game Loop and for performance reasons will not
     *          throw an exception.
     */
    void Update(double deltaTime) noexcept;

private:
    void OnKeyPress(int key);
    void OnMouseDown(uint32_t button, int x, int y);
    void OnMouseMove(int x, int y, int deltaX, int deltaY);
    void OnMouseUp(uint32_t button);
    void OnResize(uint32_t width, uint32_t height);

    GamePlayer* mPlayerPtr;
};

#endif // __GAME_GAMEWINDOW_HPP__
