/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Terrain Manager declaration.
 */

#ifndef __TERRAIN_TERRAINMANAGER_HPP__
#define __TERRAIN_TERRAINMANAGER_HPP__

#include "ChunkPool.hpp"

#include <vector>

#include "Common/TaskQueue.hpp"

struct TerrainDesc
{
    std::string terrainPath;        ///< Path to current save directory with terrain data.
    unsigned int visibleRadius;     ///< Visible chunks in straight line from current chunk.
    bool useGreedyMeshing;
};

/**
 * Defines states which Generator uses to calculate X and Z chunk coordinate during generation
 * process.
 *
 * "Dec" is short for "Decreases", "Inc" is short for "Increases".
 */
enum class GeneratorState: unsigned char
{
    ZIncXDec = 0,
    ZDecXDec,
    ZDecXInc,
    ZIncXInc
};

class TerrainManager
{
public:
    /**
     * Retrieve an instance of TerrainManager object
     *
     * @return TerrainManager instance
     */
    static TerrainManager& GetInstance();

    /**
     * Initialize the Terrain using provided structure.
     *
     * @param desc Description of terrain to create.
     */
    void Init(const TerrainDesc& desc);

    /**
     * Update Terrain state and contents.
     *
     * The method does all the work needed to refresh current Terrain state. This includes:
     *   * Generating a Mesh from chunk if need occurs
     *   * Replacing contents of current Mesh objects
     *   * Generating new chunks if these are not generated
     *   * Loading chunks from disk if they are generated but not loaded to RAM
     *
     * Overall, there is a lot work to be done here. Thus, the performance here is crucial.
     * Possibly, some work will be distributed to additional threads to avoid lagging.
     *
     * @remarks The function for performance will not throw.
     */
    void Update(int chunkX, int chunkZ, Vector pos, Vector dir, bool ray) noexcept;

private:
    TerrainManager();
    TerrainManager(const TerrainManager&) = delete;
    TerrainManager(TerrainManager&&) = delete;
    TerrainManager& operator=(const TerrainManager&) = delete;
    TerrainManager& operator=(TerrainManager&&) = delete;
    ~TerrainManager();

    /**
     * Calls Chunk::Generate() per each available chunk.
     */
    void GenerateChunks();

    /**
     * Calculate how many chunks we need for @p radius visible chunks.
     */
    unsigned int CalculateChunkCount(unsigned int radius);

    /**
     * Shifts coordinates of chunk to next available in order. If needed, generator state is also
     * shifted to next available.
     *
     * Function works in place. There is no error-sensitive work to be done here, so no errors are
     * to be returned.
     */
    void ShiftChunkCoords(int& xChunk, int& zChunk, GeneratorState& state);

    ChunkPool mChunkPool;
    std::vector<Chunk*> mChunks;
    int mCurrentChunkX;
    int mCurrentChunkZ;
    unsigned int mChunkCount;
    unsigned int mVisibleRadius;
    bool mUseGreedyMeshing;
    TaskQueue<> mGeneratorQueue;
};

#endif // __TERRAIN_TERRAINMANAGER_HPP__
