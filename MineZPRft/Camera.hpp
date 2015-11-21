/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Renderer's Camera declarations
 */

#pragma once

/**
 * Structure describing Camera initial properties
 */
struct CameraDesc
{
    // TODO initial data like fov, aspect ratio, etc
};

/**
 * Structure describing new Camera parameters to update.
 */
struct CameraUpdateDesc
{
    // TODO update data - camera position, direction, etc
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
    // TODO Matrices, vectors, constant buffers, whatevers are needed!
};
