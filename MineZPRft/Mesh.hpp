/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Renderer Mesh declarations
 */

#pragma once

/**
 * Structure for Mesh initialization.
 */
struct MeshDesc
{
    void* dataPtr;      ///< Pointer for data to use during mesh initialization.
    size_t dataSize;    ///< Size of the data.
    // TODO format of kept data and other stuff that VBO might need to initialize.
};

/**
 * Structure for Mesh update.
 */
struct MeshUpdateDesc
{
    void* dataPtr;      ///< Pointer for data to use during mesh initialization.
    size_t dataSize;    ///< Size of the data.
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
     * Update Mesh with new data.
     *
     * @param desc Mesh update description structure
     *
     * @remarks In the project, the function will be used by Terrain Generator to update meshes. It
     * will be called during main draw loop work. Since the performance in this part is crucial,
     * there is no error checking and no throws.
     */
    void Update(const MeshUpdateDesc& desc) noexcept;

private:
    // TODO OpenGL VBO ID should be here, along with other required parameters
};
