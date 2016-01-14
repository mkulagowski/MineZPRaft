/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Terrain Manager declaration.
 */

#ifndef __TERRAIN_CHUNKPOOL_HPP__
#define __TERRAIN_CHUNKPOOL_HPP__

#include "Chunk.hpp"

#include <map>
#include <tuple>


/**
 * Hash combiner for our integer std::pair key
 */
template <class T>
inline void hash_combine(std::size_t & seed, const T & v)
{
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

/**
 * Missing definition of hash for pair
 */
namespace std
{
  template<typename S, typename T> struct hash<pair<S, T>>
  {
    inline size_t operator()(const pair<S, T> & v) const
    {
      size_t seed = 0;
      ::hash_combine(seed, v.first);
      ::hash_combine(seed, v.second);
      return seed;
    }
  };
}


/**
 * A pool of Chunk objects. Keeps all already generated chunks in memory
 * and manages them in an efficient way.
 */
class ChunkPool
{
public:
    typedef std::pair<int, int> ChunkKeyType;
    typedef std::map<ChunkKeyType, Chunk> ChunkMapType;

    ChunkPool();
    ~ChunkPool();

    /**
     * Acquires a chunk which resides in [X, Z] position in the world.
     *
     * @param x Number of chunk in X axis from the world.
     * @param z Number of chunk in Z axis from the world.
     * @return Pointer to managed Chunk object.
     *
     * The function will construct a new Chunk object if it does not exist in the pool. There is no
     * way the pointer will be invalid.
     *
     * If the Chunk object was just constructed, it is returned in an initialized state. It is
     * caller's duty to invoke Chunk::Generate() on this object to fill it with valid Voxel data.
     */
    Chunk* GetChunk(int x, int z);

private:
    ChunkMapType mChunks;
};

#endif // __TERRAIN_CHUNKPOOL_HPP__
