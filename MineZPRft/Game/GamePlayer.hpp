/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Definition of GamePlayer class
 */

#ifndef __GAME_GAMEPLAYER_H__
#define __GAME_GAMEPLAYER_H__

#include "Renderer/Camera.hpp"
#include "Math/Vector.hpp"

#include "GameKeycodes.hpp"

class GamePlayer
{
public:
    GamePlayer();
    ~GamePlayer();

    /**
     * Initialize Game Player.
     *
     * @param camera Camera object, which will be updated by GamePlayer
     */
    void Init(Camera* camera);

    /**
     * Apply set vectors.
     */
    void Update();

    /**
     * Set new Position vector
     */
    void SetPosition(const Vector& pos);

    /**
     * Set new Direction vector
     */
    void SetDirection(const Vector& dir);

    /**
     * Acquire Player position vector.
     */
    const Vector& GetPosition();

    /**
     * Acquire Player direction vector.
     */
    const Vector& GetDirection();

    /**
     * Acquire Player up vector.
     */
    const Vector& GetUp();


private:
    Camera* mCameraPtr;
    Vector mPos;
    Vector mDir;
    Vector mUp;
};

#endif // __GAME_GAMEPLAYER_H__
