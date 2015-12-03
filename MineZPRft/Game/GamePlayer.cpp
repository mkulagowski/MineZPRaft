/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Definition of GamePlayer methods
 */

#include "GamePlayer.hpp"

#include "GameKeycodes.hpp"

GamePlayer::GamePlayer()
{
}

GamePlayer::~GamePlayer()
{
}

void GamePlayer::Init(Camera* camera)
{
    mCameraPtr = camera;

    mPos = Vector(0.0f, 0.25f, 2.0f, 1.0f);
    mDir = Vector(0.0f, 0.0f, -1.0f, 0.0f);
    mUp = Vector(0.0f, 1.0f, 0.0f, 0.0f);
}

void GamePlayer::Update() noexcept
{
    // send the update to Camera
    CameraUpdateDesc cud;
    cud.pos = mPos;
    cud.dir = mDir;
    cud.up = mUp;
    mCameraPtr->Update(cud);
}

void GamePlayer::SetPosition(const Vector& pos)
{
    mPos = pos;
}

void GamePlayer::SetDirection(const Vector& dir)
{
    mDir = dir;
}

const Vector& GamePlayer::GetPosition()
{
    return mPos;
}

const Vector& GamePlayer::GetDirection()
{
    return mDir;
}

const Vector& GamePlayer::GetUp()
{
    return mUp;
}
