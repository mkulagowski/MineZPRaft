/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Chunk class definition.
 */

#ifndef __TERRAIN_CHUNK_HPP__
#define __TERRAIN_CHUNK_HPP__

class Chunk
{
public:
    Chunk();
    ~Chunk();

private:
    unsigned char voxels[16*16*256];
};

#endif // __TERRAIN_CHUNK_HPP__
