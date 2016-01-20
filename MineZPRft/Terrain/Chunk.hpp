/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Chunk class definition.
 */

#ifndef __TERRAIN_CHUNK_HPP__
#define __TERRAIN_CHUNK_HPP__

#include <cstddef>
#include <vector>
#include <atomic>

#include "Voxel.hpp"
#include "Renderer/Mesh.hpp"

/**
 * Chunk dimensions.
 * Will be useful when measuring performance between specific chunk sizes.
 */
#define CHUNK_X 32
#define CHUNK_Y 128
#define CHUNK_Z 32

enum class ChunkState: unsigned char
{
    NotGenerated = 0,
    Generated,
    Updated
};

struct ChunkDesc
{
    std::string chunkPath;          ///< Path to current save directory with chunk data.
    std::string chunkFileExt;       ///< File extension for chunk files.
};

struct quad
{
    Vector start; // starting points
    int w, h; // width and height
    VoxelType v; // type of voxel to which the quad belongs
};


class Chunk
{
public:
    Chunk();
    Chunk(const Chunk& other);
    ~Chunk();

    /**
     * Triggers initialization of resources used by Chunk.
     */
    void Init();

    /**
     * Set voxel in current chunk to a specific type.
     *
     * @param x     X coordinate inside voxel array
     * @param y     Y coordinate inside voxel array
     * @param z     Z coordinate inside voxel array
     * @param voxel Type of voxel to be set
     *
     * Function sets voxel inside a Chunk to a specified value.
     *
     * By default, Chunk object is initialized with VoxelType::Air chunks. It is TerrainManager's
     * duty to fill Chunk with valid voxels.
     *
     * Following dimensions are used to access specific voxels inside a chunk:
     * <code>
     *     ____
     *    /__ /|
     *   |   | |
     *   |   | | y
     *   |   | |
     *   |   | |
     *   |___|/ z
     *     x
     * </code>
     *
     * @remarks For performance the function assumes there will be no exception thrown. Exceeding
     * voxel array dimensions should not happen, however to spare us an access violation when it
     * happens, the function will produce a warning log and will return without any modifications
     * done to voxel array.
     */
    void SetVoxel(size_t x, size_t y, size_t z, VoxelType voxel) noexcept;

    /**
     * Retrieve a voxel from the chunk.
     *
     * @param x X coordinate inside voxel array
     * @param y Y coordinate inside voxel array
     * @param z Z coordinate inside voxel array
     * @return Voxel type residing at coordinates [x, y, z]
     *
     * The function retrieves voxel type from a chunk. Following dimensions are used to access
     * specific voxels:
     * <code>
     *     ____
     *    /__ /|
     *   |   | |
     *   |   | | y
     *   |   | |
     *   |   | |
     *   |___|/ z
     *     x
     * </code>
     *
     * @remarks For performance the function assumes there will be no exception thrown. Exceeding
     * voxel array dimensions should not happen, however to spare us an access violation when it
     * happens, the function will produce a warning log and will return without any modifications
     * done to voxel array.
     */
    VoxelType GetVoxel(size_t x, size_t y, size_t z) noexcept;

    /**
     * Shift the chunk by changing its World Matrix
     *
     * @param chunkX Number of X-th chunk in the generated world, relative to the center
     * @param chunkZ Number of Z-th chunk in the generated world, relative to the center
     */
    void Shift(int chunkX, int chunkZ);

    /**
     * Fills the Chunk with Perlin-generated voxels.
     *
     * @param chunkX        Number of X-th chunk in the generated world, relative to currentChunkX.
     * @param chunkZ        Number of Z-th chunk in the generated world, relative to currentChunkZ.
     * @param currentChunkX Number of X-th chunk on which player currently is.
     * @param currentChunkZ Number of Z-th chunk on which player currently is.
     * @param useGreedyMeshing False to use Naive, true to use Greedy Meshing.
     *
     * The chunks in the world create a two-dimensional grid. All are connected and it is assumed,
     * that the map generated in between them is seamless.
     */
    void Generate(int chunkX, int chunkZ, int currentChunkX, int currentChunkZ,
                  bool useGreedyMeshing) noexcept;

    /**
     * Acquire pointer to a Mesh object managed by Chunk.
     */
    const Mesh* GetMeshPtr();

    /**
     * Commits update to Mesh object. As a result, Chunk will switch itself to "Updated" state and
     * Mesh object will become unlocked to use for Renderer.
     *
     * @remarks This call triggers OpenGL calls. It must be called by main rendering thread.
     */
    void CommitMeshUpdate();

    /**
     * Sets a Chunk to be in a "not generated" state, locks Chunk's Mesh to withhold it from
     * rendering.
     */
    void ResetState() noexcept;

    /**
     * Returns whether the mesh has finished generation and is ready to commit the changes to
     * Mesh object.
     *
     * @return True if the Chunk was generated, but not commited to VBO. False in all other
     * situations.
     */
    bool IsGenerated() const noexcept;

    /**
     * Returns whether the Chunk needs generation
     */
    bool NeedsGeneration() const noexcept;

    /**
     * Loads Chunk's voxel data from disk.
     *
     * @return True, if loading was successfull. False otherwise.
     */
    bool LoadFromDisk();

    /**
     * Writes Chunk's voxel data to disk.
     *
     * @return True, if writing was successfull. False otherwise.
     *
     * @remarks Chunk needs to be generated beforehand. Otherwise this function
     * will fail.
     */
    bool SaveToDisk();

    /**
     * Checks intersection with every non-air voxel in the chunk
     *
     * @param pos      Ray origin, in world space.
     * @param dir      Ray direction, in world space. Must be normalized!
     * @param distance Output : distance between @p pos and the intersection with the voxel
     * @param coords   Output : coordinates of the intersected voxel within chunk
     *
     * @return True, if intersection was found. False otherwise.
     */
    bool ChunkRayIntersection(Vector pos, Vector dir, float &distance, Vector &coords);

    /**
     * Generates a VBO from current state of mVoxels array using naive method.
     *
     * Created Mesh will contain a cloud of points, which shall be evolved into triangles
     * by Geometry Shader.
     *
     * The Naive generator is faster and more reliable, but enforces more workload on GPU. Thus,
     * it is mostly used for debugging purposes. Release code should contain Chunk Mesh
     * generated using Chunk::GenerateVBOGreedy().
     */
    void GenerateVBONaive();

    /**
     * Generates a VBO from current state of mVoxels array using Greedy Meshing algorithm.
     *
     * Created Mesh will contain a typical triangle mesh. No Geometry Shader work is needed
     * to render the Chunk, giving us more GPU workload for graphical effects.
     */
    void GenerateVBOGreedy();

private:
    /**
     * Translates three coordinates to a single index inside mVoxels array. Additionally checks if
     * coordinates are correct and returns an error if they exceed mVoxels dimensions.
     *
     * @param [in]  x     X coordinate inside voxel array.
     * @param [in]  y     Y coordinate inside voxel array.
     * @param [in]  z     Z coordinate inside voxel array.
     * @param [out] index Index inside mVoxels array.
     * @return True on success, false if index has exceeded the bounds of mVoxels array.
     *
     * @remarks For performance the function assumes there will be no exception thrown. Error is
     * reported through return value to propagate it further to public methods of Chunk.
     */
    bool CalculateIndex(size_t x, size_t y, size_t z, size_t& index) noexcept;

    /**
     * Checks intersection with single OBB
     *
     * @param pos              Ray origin, in world space.
     * @param dir              Ray direction, in world space. Must be normalized!
     * @param obb_min          Minimum X,Y,Z coords of the mesh when not transformed at all
     * @param obb_max          Maximum X,Y,Z coords of the mesh when not transformed at all
     * @param worldMat         Transformation applied to the mesh
     * @param intersectionDist Output : distance between @p pos and the intersection with the OBB
     *
     * @return True, if intersection was found. False otherwise.
     */
    bool OBBRayIntersection(Vector pos, Vector dir, Vector obb_min, Vector obb_max,
                            Matrix worldMat, float& intersectionDist);

    /**
     * Processes Chunk from X plane perspective.
     */
    void ProcessPlaneX(const VoxelType* voxels, const Vector& shift,
                       std::vector<quad>& resultQuads);

    /**
     * Processes Chunk from Y plane perspective.
     */
    void ProcessPlaneY(const VoxelType* voxels, const Vector& shift,
                       std::vector<quad>& resultQuads);

    /**
     * Processes Chunk from Z plane perspective.
     */
    void ProcessPlaneZ(const VoxelType* voxels, const Vector& shift,
                       std::vector<quad>& resultQuads);

    /**
     * Pushes generated quads to mVerts array
     */
    void PushVertsFromQuads(const std::vector<quad>& quads, const Vector& normal);

    /**
     * 1D Array of voxels, which represent a single chunk.
     */
    VoxelType mVoxels[CHUNK_X * CHUNK_Y * CHUNK_Z];
    std::vector<float> mVerts;
    Mesh mMesh;
    std::atomic<ChunkState> mState;
    int mCoordX, mCoordZ;
    bool mGreedyGenerated;
};

#endif // __TERRAIN_CHUNK_HPP__
