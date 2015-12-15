/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Voxel and Voxel Database definitions.
 */

#include "Voxel.hpp"

/**
 * Statically generated Voxel Database.
 *
 * Maps two values - VoxelType enum class to a Voxel struct. All elements should be accessed by
 * using a VoxelType enum as a key to a map.
 */
const VoxelDBType VoxelDB =
{
    /**
     * Pass-through voxel, also known as "nothing". This type should be the one to initialize
     * any chunk inside a Voxel Array.
     */
    {
        VoxelType::Air,
        {
            0.0f, 0.0f, 0.0f,
        }
    },

    /**
     * The unbreakable voxel. Should be used to lay on the very bottom of the chunk. It's one
     * and only purpose is to not let the Player fall down to infinity.
     */
    {
        VoxelType::Bedrock,
        {
            0.2f, 0.2f, 0.2f,
        }
    },

    /**
     * Typical building material, the Stone block. Has a somewhat-greyish color and will be used
     * as a foundation beneath Dirt Voxels.
     */
    {
        VoxelType::Stone,
        {
            0.7f, 0.65f, 0.75f,
        }
    },

    /**
     * The Voxel That Shall Not Be Used, aka. The Unknown Voxel. This voxel should be a default
     * returned value when provided Voxel type by user is not available.
     */
    {
        VoxelType::Unknown,
        {
            1.0f, 0.0f, 0.0f,
        }
    },
};
