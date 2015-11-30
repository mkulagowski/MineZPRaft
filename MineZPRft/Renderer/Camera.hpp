/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Renderer's Camera declarations
 */

#pragma once

#include "Math/Vector.hpp"
#include "Math/Matrix.hpp"


/**
 * Structure describing Camera parameters for regular update.
 */
struct CameraUpdateDesc
{
    Vector pos;     ///< Position of camera in 3D space
    Vector dir;     ///< Direction of camera (aka. Look At vector)
    Vector up;      ///< Camera's Up vector
};

/**
 * Structure describing Camera initial properties
 */
struct CameraDesc
{
    /// perspective info
    float fov;          ///< Field of View.
    float aspectRatio;  ///< Aspect Ratio (screen width / screen height).
    float nearDist;     ///< Near distance (objects closer to camera than nearDist will clip)
    float farDist;      ///< Far distance (objects further from camera than farDist will clip)

    /// View initial info
    CameraUpdateDesc initialView;
};

class Camera
{
public:
    Camera();
    ~Camera();

    /**
     * Initialize the camera.
     *
     * @param desc Camera description structure.
     *
     * This function will load all necessary data for Camera and calculate all matrices needed for
     * correct Camera work.
     *
     * @remarks In case of error during initialization, the function may throw.
     */
    void Init(const CameraDesc& desc);

    /**
     * Update the camera.
     *
     * @param request Camera update description structure.
     *
     * @remarks This function will be used during main draw loop every frame. For speed purposes it
     * assures no exception thrown from it.
     */
    void Update(const CameraUpdateDesc& desc) noexcept;

private:
    Matrix mView;
    Matrix mPerspective;
};
