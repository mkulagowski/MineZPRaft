/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  OpenGL Renderer declarations
 */

#pragma once

#include <string>
#include <vector>

#include "Mesh.hpp"
#include "Camera.hpp"

struct RendererDesc
{
    std::string shaderPath;
};

class Renderer
{
public:
    Renderer();
    ~Renderer();

    /**
     * Initializes the Renderer Object.
     *
     * @param desc Renderer description structure.
     *
     * The function performs all OpenGL-related initialization work - extensions are initialized,
     * Shaders are loaded and all Rendering Pipeline States are set up for further work.
     *
     * @note This function assumes that there currently is valid OpenGL context enabled.
     *
     * @remarks This function might throw if Renderer cannot be initialized and exceptions cannot
     * be acquired from current OpenGL Context.
     */
    void Init(const RendererDesc& desc);

    /**
     * Add new Mesh to to-render mesh list.
     *
     * @param mesh Pointer to constant Mesh object to be added.
     *
     * The function adds already created Mesh object to Mesh Array. The Array is used later on to
     * iterate over every element and draw it inside Draw function.
     *
     * @note Renderer only reads data from Mesh object and assures nothing will be changed inside.
     * Any modifications, or updates to Mesh objects, are to be done by other managers.
     *
     * @remarks If the Mesh object is invalid, the function will throw an exception.
     */
    void AddMesh(const Mesh* mesh);

    /**
     * Commence drawing of provided Mesh objects.
     *
     * The function will iterate over every Mesh Array element and use its contents to draw them
     * using OpenGL functions.
     *
     * @remarks This function will be called inside main draw loop. Thus, for speed purposes, it
     * assumes all data used for rendering are correct and does no error checking. Therefore, it is
     * assumed that the function will not throw.
     */
    void Draw() noexcept;

private:
    typedef std::vector<const Mesh*> MeshArrayType;

    Camera mCamera;
    MeshArrayType mMeshArray;
};
