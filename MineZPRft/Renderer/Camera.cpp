/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Renderer's Camera definitions
 */

#include "Camera.hpp"

#include "Common/Common.hpp"

Camera::Camera()
{
}

Camera::~Camera()
{
}

void Camera::Init(const CameraDesc& desc)
{
    mView = CreateRHLookAtMatrix(desc.initialView.pos,
                                 desc.initialView.dir,
                                 desc.initialView.up);

    mPerspective = CreateRHPerspectiveMatrix(desc.fov,
                                             desc.aspectRatio,
                                             desc.nearDist,
                                             desc.farDist);
}

void Camera::Update(const CameraUpdateDesc& request) noexcept
{
    mPos = request.pos;
    mView = CreateRHLookAtMatrix(request.pos, request.pos + request.dir, request.up);
}

void Camera::UpdatePerspective(const CameraDesc& desc)
{
    mPerspective = CreateRHPerspectiveMatrix(desc.fov,
                                             desc.aspectRatio,
                                             desc.nearDist,
                                             desc.farDist);
}

const float* Camera::GetPosRaw() noexcept
{
    return mPos.Data();
}

const float* Camera::GetViewRaw() noexcept
{
    return mView.Data();
}

const float* Camera::GetPerspectiveRaw() noexcept
{
    return mPerspective.Data();
}
