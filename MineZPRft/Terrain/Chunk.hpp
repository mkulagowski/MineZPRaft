/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Chunk class definition.
 */

#ifndef __TERRAIN_CHUNK_HPP__
#define __TERRAIN_CHUNK_HPP__

#include <cstddef>

/**
 * Chunk dimensions.
 * Will be useful when measuring performance between specific chunk sizes.
 */
#define CHUNK_X 16
#define CHUNK_Y 256
#define CHUNK_Z 16


/**
 * Enumerates voxels available to set inside a Chunk.
 */
enum class Voxel : unsigned char
{
    Air = 0,        ///< Pass-through voxel, aka. "nothing". The default one to initialize.
    Bedrock,        ///< Unbreakable material, used at the very end of the map.
    Stone,          ///< Stone block, destructable.
    Unknown         ///< Maximum possible voxel type. Typically used to return an error.
};


class Chunk
{
public:
    Chunk();
    ~Chunk();

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
    void SetVoxel(size_t x, size_t y, size_t z, Voxel voxel) noexcept;

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
    Voxel GetVoxel(size_t x, size_t y, size_t z) noexcept;

private:
    /**
     * Translates three coordinates to a single index inside mVoxels array. Additionally checks if
     * coordinates are correct and returns an error if they exceed mVoxels dimensions.
     *
     * @param [in[  x     X coordinate inside voxel array.
     * @param [in]  y     Y coordinate inside voxel array.
     * @param [in]  z     Z coordinate inside voxel array.
     * @param [out] index Index inside mVoxels array.
     * @return True on success, false if index has exceeded
     *
     * @remarks For performance the function assumes there will be no exception thrown. Error is
     * reported through return value to propagate it further to public methods of Chunk.
     */
    bool CalculateIndex(size_t x, size_t y, size_t z, size_t& index) noexcept;

    /**
     * Array of voxels, which represent a single chunk.
     */
    Voxel mVoxels[CHUNK_X*CHUNK_Y*CHUNK_Z];
};

#endif // __TERRAIN_CHUNK_HPP__
