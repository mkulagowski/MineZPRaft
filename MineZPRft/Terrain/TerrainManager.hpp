/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Terrain Manager declaration.
 */

#ifndef __TERRAIN_TERRAINMANAGER_HPP__
#define __TERRAIN_TERRAINMANAGER_HPP__

#include "Chunk.hpp"


struct TerrainDesc
{
    std::string terrainPath;
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
    void Update(int chunkX, int chunkZ) noexcept;

private:
    TerrainManager();
    TerrainManager(const TerrainManager&) = delete;
    TerrainManager(TerrainManager&&) = delete;
    TerrainManager& operator=(const TerrainManager&) = delete;
    TerrainManager& operator=(TerrainManager&&) = delete;
    ~TerrainManager();

    Chunk mChunk;
    Chunk mChunk2;
    Chunk mChunk3;
    Chunk mChunk4;
    Chunk mChunk5;
    Chunk mChunk6;
    Chunk mChunk7;
    Chunk mChunk8;
    Chunk mChunk9;
    int mCurrentChunkX;
    int mCurrentChunkZ;
};

#endif // __TERRAIN_TERRAINMANAGER_HPP__
