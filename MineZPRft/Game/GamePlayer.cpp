/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Definition of GamePlayer class
 */

#include "GamePlayer.hpp"

GamePlayer::GamePlayer()
{
}

GamePlayer::~GamePlayer()
{
}

void GamePlayer::Init(Camera* camera)
{
    mCameraPtr = camera;

    mPos = Vector(2.0f, 0.25f, 2.0f, 1.0f);
    mDir = Vector(0.0f, 0.25f, 0.0f, 1.0f);
    mUp = Vector(0.0f, 1.0f, 0.0f, 0.0f);
}

void GamePlayer::Update(const Vector& shift)
{
    // update Player position
    mPos += shift;

    // send the update to Camera
    CameraUpdateDesc cud;
    cud.pos = mPos;
    cud.dir = mDir;
    cud.up = mUp;
    mCameraPtr->Update(cud);
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
