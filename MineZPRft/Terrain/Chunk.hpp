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
     * Fills the Chunk with Perlin-generated voxels.
     *
     * @param chunkX        Number of X-th chunk in the generated world, relative to currentChunkX.
     * @param chunkZ        Number of Z-th chunk in the generated world, relative to currentChunkZ.
     * @param currentChunkX Number of X-th chunk on which player currently is.
     * @param currentChunkZ Number of Z-th chunk on which player currently is.
     *
     * The chunks in the world create a two-dimensional grid. All are connected and it is assumed,
     * that the map generated in between them is seamless.
     */
    void Generate(int chunkX, int chunkZ, int currentChunkX, int currentChunkZ) noexcept;

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
     * 1D Array of voxels, which represent a single chunk.
     */
    VoxelType mVoxels[CHUNK_X * CHUNK_Y * CHUNK_Z];
    std::vector<float> mVerts;
    Mesh mMesh;
    std::atomic<ChunkState> mState;
};

#endif // __TERRAIN_CHUNK_HPP__
