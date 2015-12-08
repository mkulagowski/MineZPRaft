/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Definition of GamePlayer methods
 */

#include "GamePlayer.hpp"

#include "GameKeycodes.hpp"
#include "Math/Common.hpp"

namespace {

const Vector Z_VECTOR = Vector(0.0f, 0.0f, 1.0f, 0.0f);

} // namespace


GamePlayer::GamePlayer()
{
}

GamePlayer::~GamePlayer()
{
}

void GamePlayer::Init(Camera* camera)
{
    mCameraPtr = camera;

    mPos = Vector(0.0f, 1.0f, 0.0f, 1.0f);
    mAngleX = 0.0f;
    mAngleY = MATH_PI/4.0f;
    mUp = Vector(0.0f, 1.0f, 0.0f, 0.0f);
    Update();
}

void GamePlayer::Update() noexcept
{
    // calculate camera direction
    // the calculations must be separated, otherwise matrixRotX * matrixRotY will result in a
    // weird rotation matrix causing glitches on some angles.
    mDir = CreateRotationMatrixX(mAngleY) * Z_VECTOR;
    mDir = CreateRotationMatrixY(mAngleX) * mDir;
    mDir.Normalize();

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

void GamePlayer::ShiftRotationX(const float deltaX)
{
    mAngleX += deltaX;
}

void GamePlayer::ShiftRotationY(const float deltaY)
{
    mAngleY += deltaY;
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
