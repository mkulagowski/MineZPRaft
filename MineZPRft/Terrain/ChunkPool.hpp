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


template <class T>
inline void hash_combine(std::size_t & seed, const T & v)
{
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

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


class ChunkPool
{
public:
    typedef std::pair<int, int> ChunkKeyType;
    typedef std::map<ChunkKeyType, Chunk> ChunkMapType;

    ChunkPool();
    ~ChunkPool();

    void SetRadiusToCover(unsigned int radius);

    Chunk* GetChunk(int x, int z);

private:
    ChunkMapType mChunks;
};

#endif // __TERRAIN_CHUNKPOOL_HPP__
