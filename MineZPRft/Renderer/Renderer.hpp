/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  OpenGL Renderer declarations
 */

#ifndef __RENDERER_RENDERER_HPP__
#define __RENDERER_RENDERER_HPP__

#include <string>
#include <vector>

#include "Mesh.hpp"
#include "Camera.hpp"
#include "Shader.hpp"

/**
 * Structure with data needed to initialize Renderer object.
 */
struct RendererDesc
{
    std::string shaderPath; ///< Path to directory with shaders used in Renderer
    GLsizei windowWidth;    ///< Window width, to set up OpenGL Viewport
    GLsizei windowHeight;   ///< Window height, to set up OpenGL Viewport
};

class Renderer
{
public:
    /**
     * Retrieve an instance of Renderer object.
     *
     * @return Reference to Renderer instance.
     */
    static Renderer& GetInstance();

    /**
     * Initializes the Renderer Object.
     *
     * @param desc Renderer description structure.
     *
     * The function performs all OpenGL-related initialization work - extensions are initialized,
     * Shaders are loaded and all Rendering Pipeline States are set up for further work.
     *
     * @note This function assumes that there is a valid OpenGL context enabled.
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
     */
    void AddMesh(const Mesh* mesh) noexcept;

    /**
     * Replaces Terrain Mesh pointer with a new one.
     *
     * @param index Index of Mesh to replace
     * @param mesh  Mesh pointer
     */
    void ReplaceTerrainMesh(size_t index, const Mesh* mesh) noexcept;

    /**
     * Reserves @p meshCount slots for Terrain Meshes.
     *
     * @param meshCount Amount of slots to reserve for Terrain Meshes.
     */
    void ReserveTerrainMeshPool(size_t meshCount) noexcept;

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

    /**
     * Resize viewport and all dependent etities
     */
    void ResizeViewport(GLsizei w, GLsizei h);

    /**
     * Acquire pointer to Renderer's Camera
     */
    Camera* GetCameraPtr();

private:
    Renderer();
    Renderer(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer& operator=(Renderer&&) = delete;
    ~Renderer();

    typedef std::vector<const Mesh*> MeshArrayType;

    struct TerrainShaderLocs
    {
        GLint worldMatrix;
        GLint viewMatrix;
        GLint perspectiveMatrix;
        GLint playerPos;

        TerrainShaderLocs()
            : worldMatrix(GL_NONE)
            , viewMatrix(GL_NONE)
            , perspectiveMatrix(GL_NONE)
            , playerPos(GL_NONE)
        {}
    };

    Camera mCamera;
    Shader mTerrainShaderNaive;
    TerrainShaderLocs mTerrainShaderUniforms;
    GLuint mDummyVAO; // We don't need this, but OGL has its needs and won't cooperate without it
    bool initDone;
    MeshArrayType mMeshArray;
    MeshArrayType mTerrainMeshArray;
};

#endif // __RENDERER_RENDERER_HPP__
