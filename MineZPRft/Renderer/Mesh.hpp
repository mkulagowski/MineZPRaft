/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Renderer Mesh declarations
 */

#ifndef __RENDERER_MESH_HPP__
#define __RENDERER_MESH_HPP__

#include "Defines.hpp"

#include "Math/Matrix.hpp"
#include <cstdlib>
#include <atomic>

/**
 * Structure for Mesh initialization.
 */
struct MeshDesc
{
    void* dataPtr;      ///< Pointer for data to use during mesh initialization.
    size_t dataSize;    ///< Size of the data.
    size_t vertCount;   ///< Amount of vertices.
    // TODO format of kept data and other stuff that VBO might need to initialize.
};

/**
 * Structure for Mesh update.
 */
struct MeshUpdateDesc
{
    void* dataPtr;      ///< Pointer for data to use during mesh initialization.
    size_t dataSize;    ///< Size of the data.
    size_t vertCount;   ///< Amount of vertices.
};


enum class MeshPrimitiveType: unsigned char
{
    Points = 0,
    Triangles
};

class Mesh
{
public:
    Mesh();
    ~Mesh();

    /**
     * Initialize a new Mesh from provided data.
     *
     * @param desc Mesh description structure
     *
     * The function will create a new OpenGL Vertex Buffer Object and preinitialize it with data
     * kept inside MeshDesc structure.
     *
     * @remarks This function might throw if there is an error during VBO initialization.
     */
    void Init(const MeshDesc& desc);

    /**
     * Bind Mesh to OpenGL.
     */
    void Bind() const noexcept;

    /**
     * Replaces current World Matrix with a new one @p matrix.
     *
     * @param matrix Matrix to replace current World Matrix.
     *
     * @remarks For performance the function will not throw.
     */
    void SetWorldMatrix(const Matrix& matrix) noexcept;

    /**
     * Acquire pointer to raw World Matrix data.
     */
    const float* GetWorldMatrixRaw() const noexcept;

    /**
     * Get vertex count for current mesh.
     */
    GLsizei GetVertCount() const noexcept;

    /**
     * Update Mesh with new data.
     *
     * @param desc Mesh update description structure
     *
     * @remarks In the project, the function will be used by Terrain Generator to update meshes. It
     * will be called during main draw loop work. Since the performance in this part is crucial,
     * there is no error checking and no throws.
     */
    void Update(const MeshUpdateDesc& desc) noexcept;

    /**
     * Sets a "locked" flag for Mesh object.
     *
     * @param locked True if mesh is supposed to be locked from rendering, false otherwise.
     *
     * When locked flag is set to True, Renderer will skip the mesh during rendering process.
     * This is usually used when other thread is about to change the contents of the Mesh (ex. its
     * world matrix).
     */
    void SetLocked(bool locked) noexcept;

    /**
     * Acquires current state of "locked" flag.
     */
    bool IsLocked() const noexcept;

    /**
     * Sets a primitive type for this Mesh object.
     *
     * @param type Primitive type used in Mesh.
     */
    void SetPrimitiveType(MeshPrimitiveType type) noexcept;

    /**
     * Acquires Mesh Primitive type translated to OpenGL macro form.
     */
    GLenum GetGLPrimitiveType() const noexcept;

private:
    GLuint mVBO;
    GLsizei mVertCount;
    Matrix mWorldMatrix;
    std::atomic<bool> mLocked;
    MeshPrimitiveType mPrimitiveType;
};

#endif // __RENDERER_MESH_HPP__
