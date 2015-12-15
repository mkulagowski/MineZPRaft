/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Voxel and Voxel Database declarations.
 */

#ifndef __TERRAIN_VOXEL_HPP__
#define __TERRAIN_VOXEL_HPP__

#include <map>
#include <type_traits>

/**
 * Enumerates voxels available to set inside a Chunk.
 *
 * Refer to VoxelDatabase.cpp for more detailed descriptions about each voxel.
 */
enum class VoxelType : unsigned char
{
    Air = 0,
    Bedrock,
    Stone,
    Unknown
};

/**
 * Underlying type of VoxelType enum, used to cast the enums to known simple types.
 */
typedef std::underlying_type<VoxelType>::type VoxelUnderType;

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

#endif // __TERRAIN_VOXEL_HPP__
