/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Voxel Database declarations.
 */

#ifndef __TERRAIN_VOXELDATABASE_HPP__
#define __TERRAIN_VOXELDATABASE_HPP__

#include <map>

/**
 * Enumerates voxels available to set inside a Chunk.
 *
 * Refer to VoxelDatabase.hpp for more detailed descriptions about each voxel.
 */
enum class VoxelType : unsigned char
{
    Air = 0,
    Bedrock,
    Stone,
    Unknown
};

/**
 * Describes each voxel with a set of attributes specific to it.
 */
struct Voxel
{
    float colorRed;     ///< Voxel color, Red component
    float colorGreen;   ///< Voxel color, Green component
    float colorBlue;    ///< Voxel color, Blue component
};

typedef std::map<VoxelType, Voxel> VoxelDBType;

extern const VoxelDBType VoxelDB;

#endif // __TERRAIN_VOXELDATABASE_HPP__
