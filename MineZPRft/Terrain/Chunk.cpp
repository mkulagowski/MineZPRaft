/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Chunk methods definition.
 */

#define NOMINMAX
#include "Chunk.hpp"

#include "Common/Logger.hpp"
#include "Math/Common.hpp"
#include "NoiseGenerator.hpp"
#include "Renderer/Renderer.hpp"
#include "Common/FileSystem.hpp"
#include "Math/Vector.hpp"
#include "Math/Matrix.hpp"

#include <fstream>

namespace
{
// TODO Get rid of CHUNK_* consts. They should be customizable.
// TODO Consider moving CHUNK_DIR to user home directory
const std::string CHUNK_DIR = "ChunkBank";
const std::string CHUNK_FILEEXT = ".riQrll";
const int HEIGHTMAP_HEIGHT = 16;
const double AIR_THRESHOLD = 0.3;
const int FLOAT_COUNT_PER_VERTEX_NAIVE = 7;
const int FLOAT_COUNT_PER_VERTEX_GREEDY = 10;
const float ALPHA_COMPONENT = 1.0f; // Alpha color component should stay at 1,0 (full opacity).


} // namespace


Chunk::Chunk()
    : mState(ChunkState::NotGenerated)
{
    for (VoxelType& voxel : mVoxels)
        voxel = VoxelType::Air;
}

Chunk::Chunk(const Chunk& other)
{
    for (int i = 0; i < CHUNK_X * CHUNK_Y * CHUNK_Z; ++i)
        mVoxels[i] = other.mVoxels[i];

    mVerts = other.mVerts;
    mGreedyGenerated = other.mGreedyGenerated;
    MeshDesc md;
    md.dataPtr = mVerts.data();
    md.dataSize = mVerts.size() * sizeof(float);
    md.vertCount = mVerts.size() / (mGreedyGenerated ? FLOAT_COUNT_PER_VERTEX_GREEDY
                                                     : FLOAT_COUNT_PER_VERTEX_NAIVE);
    mMesh.Init(md);
    mState = other.mState.load();
    if (mState == ChunkState::Updated)
        mMesh.SetLocked(false);
    else
        mMesh.SetLocked(true);
}

Chunk::~Chunk()
{
}

void Chunk::Init()
{
    MeshDesc md;
    md.dataPtr = 0;
    md.dataSize = 0;
    md.vertCount = 0;
    mMesh.Init(md);
    mMesh.SetLocked(true);
}

void Chunk::SetVoxel(size_t x, size_t y, size_t z, VoxelType voxel) noexcept
{
    // calculate the index
    size_t index = 0;
    if (!CalculateIndex(x, y, z, index))
        return;

    mVoxels[index] = voxel;
}

VoxelType Chunk::GetVoxel(size_t x, size_t y, size_t z) noexcept
{
    // calculate the index
    size_t index = 0;
    if (!CalculateIndex(x, y, z, index))
        return VoxelType::Unknown;

    return mVoxels[index];
}

void Chunk::Shift(int chunkX, int chunkZ)
{
    Vector shift((static_cast<float>(chunkX) - 0.5f) * CHUNK_X,
                 static_cast<float>(-(CHUNK_Y / 4) - HEIGHTMAP_HEIGHT),
                 (static_cast<float>(chunkZ) - 0.5f) * CHUNK_Z,
                 0.0f);
    // TODO rotation should be unnecessary! Probably a bug in Perlin
    mMesh.SetWorldMatrix(CreateTranslationMatrix(shift) * CreateRotationMatrixY(MATH_PIF));
}

void Chunk::Generate(int chunkX, int chunkZ, int currentChunkX, int currentChunkZ,
                     bool useGreedyMeshing) noexcept
{
    // Set coords for chunk
    mCoordX = chunkX + currentChunkX;
    mCoordZ = chunkZ + currentChunkZ;

    // If Chunk was saved to disk, load it from file.
    if (LoadFromDisk())
    {
        if (useGreedyMeshing)
            GenerateVBOGreedy();
        else
            GenerateVBONaive();

        LOG_D("Chunk [" << mCoordX << ", "
              << mCoordZ << "] was successfully read from disk.");
        return;
    }

    NoiseGenerator& noiseGen = NoiseGenerator::GetInstance();

    // Further "generation loops" will assume that bottom two layers of chunk are
    // filled with Bedrock, so their Y iterator will begin from 2.

    // Stage 0 - cleanup before use
    for (int i = 0; i < CHUNK_X * CHUNK_Y * CHUNK_Z; ++i)
        mVoxels[i] = VoxelType::Air;

    // Stage 1 - fill bottom quarter of chunk with stone
    for (int z = 0; z < CHUNK_Z; ++z)
        for (int y = 2; y < CHUNK_Y / 4; ++y)
            for (int x = 0; x < CHUNK_X; ++x)
                SetVoxel(x, y, z, VoxelType::Stone);

    LOG_D("  Chunk [" << mCoordX << ", " << mCoordZ << "] Stage 1 done");

    // Stage 2.1 - generate a heightmap using Perlin
    double noise;
    std::vector<double> heightMap;
    for (int z = 0; z < CHUNK_Z; ++z)
        for (int x = 0; x < CHUNK_X; ++x)
        {
            // TODO adjust scaling
            // Noise arguments are shifted according to Chunk::Generate() arguments.
            // This way the map will be seamless and the chunks connected.
            noise = noiseGen.Noise((x + (CHUNK_Z * mCoordZ)) / 32.0,
                                   0.0,
                                   (z + (CHUNK_X * mCoordX)) / 32.0);

            // Noise-returned values span -1..1 range,
            // Add 1 to them to convert it to 0..2 range.
            noise += 1.0;

            // To make Stage 2.2 easier, convert values from range 0..2 to 0..HEIGHTMAP_HEIGHT
            noise *= HEIGHTMAP_HEIGHT / 2;

            // Dump the data inside vector for further analysis
            heightMap.push_back(noise);
        }

    // Stage 2.2 - convert generated heightmap to stone voxels
    for (int z = 0; z < CHUNK_Z; ++z)
        for (int y = CHUNK_Y / 4; y < (CHUNK_Y / 4) + HEIGHTMAP_HEIGHT; ++y)
            for (int x = 0; x < CHUNK_X; ++x)
            {
                // Add a stone voxel if heightmap's value is higher
                // than currently processed voxel's Y coordinate.
                if (heightMap[x * CHUNK_Z + z] >= static_cast<double>(y - (CHUNK_Y / 4)))
                    SetVoxel(x, y, z, VoxelType::Stone);
            }

    LOG_D("  Chunk [" << mCoordX << ", " << mCoordZ << "] Stage 2 done");
    /*
    // Stage 3 - cut through the terrain with some Perlin-generated caves
    for (int z = 0; z < CHUNK_Z; ++z)
        for (int y = 2; y < CHUNK_Y; ++y)
            for (int x = 0; x < CHUNK_X; ++x)
            {
                // TODO adjust scaling
                // NOTE chunkZ applies to X coordinate and chunkX applies to Z coordinate.
                //      Otherwise, the chunk would be rotated and the map would lost its
                //      seamlessness.
                noise = noiseGen.Noise((x + CHUNK_X * mCoordX) * 0.1,
                                        y * 0.1,
                                       (z + CHUNK_Z * mCoordZ) * 0.1);

                if (noise > AIR_THRESHOLD)
                    SetVoxel(x, y, z, VoxelType::Air);
            }

    LOG_D("  Chunk [" << mCoordX << ", " << mCoordZ << "] Stage 3 done");*/

    // Stage 4 - force-fill first two layers of the ground with bedrock
    for (int z = 0; z < CHUNK_Z; ++z)
        for (int y = 0; y < 2; ++y)
            for (int x = 0; x < CHUNK_X; ++x)
                SetVoxel(x, y, z, VoxelType::Bedrock);

    LOG_D("  Chunk [" << mCoordX << ", " << mCoordZ << "] Stage 4 done");

    if (useGreedyMeshing)
        GenerateVBOGreedy();
    else
        GenerateVBONaive();
    LOG_D("  Chunk [" << mCoordX << ", " << mCoordZ << "] generated.");

    return;
}

const Mesh* Chunk::GetMeshPtr()
{
    return &mMesh;
}

void Chunk::CommitMeshUpdate()
{
    MeshUpdateDesc md;
    md.dataPtr = mVerts.data();
    md.dataSize = mVerts.size() * sizeof(float);
    md.vertCount = mVerts.size() / (mGreedyGenerated ? FLOAT_COUNT_PER_VERTEX_GREEDY
                                                     : FLOAT_COUNT_PER_VERTEX_NAIVE);
    mMesh.Update(md);
    mState = ChunkState::Updated;
    mMesh.SetLocked(false);
}

void Chunk::ResetState() noexcept
{
    mState = ChunkState::NotGenerated;
    mMesh.SetLocked(true);
}

bool Chunk::IsGenerated() const noexcept
{
    return mState == ChunkState::Generated;
}

bool Chunk::NeedsGeneration() const noexcept
{
    return mState == ChunkState::NotGenerated;
}

bool Chunk::CalculateIndex(size_t x, size_t y, size_t z, size_t& index) noexcept
{
    if ((x > CHUNK_X) || (y > CHUNK_Y) || (z > CHUNK_Z))
    {
        LOG_W("Chunk coordinates [" << x << ", " << y << ", " << z
              << "] exceed available Chunk dimensions! (which are ["
              << CHUNK_X << ", " << CHUNK_Y << ", " << CHUNK_Z << "])");
        return false;
    }

    // Convert 3D coordinates to a 1D array index. This way the one-dimensional array,
    // which is easily accessible by Renderer, can be used as a 3D array.
    index = x * CHUNK_Y * CHUNK_Z + y * CHUNK_Z + z;
    return true;
}

void Chunk::GenerateVBONaive()
{
    mVerts.clear();
    for (int z = 0; z < CHUNK_Z; ++z)
        for (int y = 0; y < CHUNK_Y / 4 + HEIGHTMAP_HEIGHT; ++y)
            for (int x = 0; x < CHUNK_X; ++x)
            {
                VoxelType vox = GetVoxel(x, y, z);
                if (vox != VoxelType::Air && vox != VoxelType::Unknown)
                {
                    // do some checks before adding a voxel to VBO
                    // first of all, test only if we are not a bounding voxel chunk
                    // otherwise we must add it anyway
                    if ((x > 0) && (x < CHUNK_X - 1) &&
                        (y > 0) && (y < CHUNK_Y - 1) &&
                        (z > 0) && (z < CHUNK_Z - 1))
                    {
                        // Now see if there is VoxelType::Air in our neighbourhood
                        // If it is, continue to add voxel to VBO. Otherwise, discard.
                        VoxelType voxPlusX = GetVoxel(x+1, y, z);
                        VoxelType voxMinusX = GetVoxel(x-1, y, z);
                        VoxelType voxPlusY = GetVoxel(x, y+1, z);
                        VoxelType voxMinusY = GetVoxel(x, y-1, z);
                        VoxelType voxPlusZ = GetVoxel(x, y, z+1);
                        VoxelType voxMinusZ = GetVoxel(x, y, z-1);

                        if ((voxPlusX != VoxelType::Air) &&
                            (voxMinusX != VoxelType::Air) &&
                            (voxPlusY != VoxelType::Air) &&
                            (voxMinusY != VoxelType::Air) &&
                            (voxPlusZ != VoxelType::Air) &&
                            (voxMinusZ != VoxelType::Air))
                            // We are surrounded by voxels. Ergo, we are not visible.
                            // Discard current voxel to not render unseen voxels.
                            continue;
                    }

                    auto voxDataIt = VoxelDB.find(vox);
                    if (voxDataIt == VoxelDB.end())
                    {
                        LOG_E("Voxel " << static_cast<VoxelUnderType>(vox)
                              << " was not found in database!");
                        continue;
                    }

                    mVerts.push_back(static_cast<float>(x));
                    mVerts.push_back(static_cast<float>(y));
                    mVerts.push_back(static_cast<float>(z));

                    const Voxel& voxData = voxDataIt->second;
                    mVerts.push_back(voxData.colorRed);
                    mVerts.push_back(voxData.colorGreen);
                    mVerts.push_back(voxData.colorBlue);
                    mVerts.push_back(ALPHA_COMPONENT);
                }
            }
    mMesh.SetPrimitiveType(MeshPrimitiveType::Points);
    // TODO Consider if this won't race with rest of the code
    // If so check Chunk::Generate() and TerrainManager::Update()
    mState = ChunkState::Generated;
    mGreedyGenerated = false;
}

void Chunk::ProcessPlaneX(const VoxelType* voxels, const Vector& shift,
                         std::vector<quad>& resultQuads)
{
    bool quadProcessing;
    quad q;
    for (int x = 0; x < CHUNK_X; ++x)
        for (int y = 0; y < CHUNK_Y / 4 + HEIGHTMAP_HEIGHT; ++y)
        {
            // reset flags
            quadProcessing = false;

            for (int z = 0; z < CHUNK_Z; ++z)
            {
                size_t index;
                CalculateIndex(x, y, z, index);
                VoxelType vox = voxels[index];
                if (vox != VoxelType::Air && vox != VoxelType::Unknown)
                {
                    // we found a voxel in this line that is not air!
                    if (!quadProcessing)
                    {
                        // begin the processing, as this is the first one in this line
                        q = {Vector(static_cast<float>(x),
                                    static_cast<float>(y),
                                    static_cast<float>(z), 0.0f) + shift,
                             1, 1, vox};
                        quadProcessing = true;
                    }
                    else
                    {
                        if (vox == q.v)
                            // extend the quad that is already started (unless the voxel type matches)
                            q.w++;
                        else
                        {
                            // otherwise, close the quad and start a new one
                            resultQuads.push_back(q);
                            q = {Vector(static_cast<float>(x),
                                        static_cast<float>(y),
                                        static_cast<float>(z), 0.0f) + shift,
                                 1, 1, vox};
                        }
                    }
                }
                else
                {
                    // we hit air during process, close the quad if it is being processed
                    if (quadProcessing)
                    {
                        resultQuads.push_back(q);
                        quadProcessing = false;
                    }
                }
            }

            if (quadProcessing)
                // line finished during quad processing
                // we can close the quad and push it back
                resultQuads.push_back(q);
        }
}

void Chunk::ProcessPlaneY(const VoxelType* voxels, const Vector& shift,
                         std::vector<quad>& resultQuads)
{
    bool quadProcessing;
    quad q;
    for (int y = 0; y < CHUNK_Y / 4 + HEIGHTMAP_HEIGHT; ++y)
        for (int z = 0; z < CHUNK_Z; ++z)
        {
            // reset flags
            quadProcessing = false;

            for (int x = 0; x < CHUNK_X; ++x)
            {
                size_t index;
                CalculateIndex(x, y, z, index);
                VoxelType vox = voxels[index];
                if (vox != VoxelType::Air && vox != VoxelType::Unknown)
                {
                    // we found a voxel in this line that is not air!
                    if (!quadProcessing)
                    {
                        // begin the processing, as this is the first one in this line
                        q = {Vector(static_cast<float>(x),
                                    static_cast<float>(y),
                                    static_cast<float>(z), 0.0f) + shift,
                             1, 1, vox};
                        quadProcessing = true;
                    }
                    else
                    {
                        if (vox == q.v)
                            // extend the quad that is already started (unless the voxel type matches)
                            q.w++;
                        else
                        {
                            // otherwise, close the quad and start a new one
                            resultQuads.push_back(q);
                            q = {Vector(static_cast<float>(x),
                                        static_cast<float>(y),
                                        static_cast<float>(z), 0.0f) + shift,
                                 1, 1, vox};
                        }
                    }
                }
                else
                {
                    // we hit air during process, close the quad if it is being processed
                    if (quadProcessing)
                    {
                        resultQuads.push_back(q);
                        quadProcessing = false;
                    }
                }
            }

            if (quadProcessing)
                // line finished during quad processing
                // we can close the quad and push it back
                resultQuads.push_back(q);
        }
}

void Chunk::ProcessPlaneZ(const VoxelType* voxels, const Vector& shift,
                         std::vector<quad>& resultQuads)
{
    bool quadProcessing;
    quad q;
    for (int z = 0; z < CHUNK_Z; ++z)
        for (int y = 0; y < CHUNK_Y / 4 + HEIGHTMAP_HEIGHT; ++y)
        {
            // reset flags
            quadProcessing = false;

            for (int x = 0; x < CHUNK_X; ++x)
            {
                size_t index;
                CalculateIndex(x, y, z, index);
                VoxelType vox = voxels[index];
                if (vox != VoxelType::Air && vox != VoxelType::Unknown)
                {
                    // we found a voxel in this line that is not air!
                    if (!quadProcessing)
                    {
                        // begin the processing, as this is the first one in this line
                        q = {Vector(static_cast<float>(x),
                                    static_cast<float>(y),
                                    static_cast<float>(z), 0.0f) + shift,
                             1, 1, vox};
                        quadProcessing = true;
                    }
                    else
                    {
                        if (vox == q.v)
                            // extend the quad that is already started (unless the voxel type matches)
                            q.w++;
                        else
                        {
                            // otherwise, close the quad and start a new one
                            resultQuads.push_back(q);
                            q = {Vector(static_cast<float>(x),
                                        static_cast<float>(y),
                                        static_cast<float>(z), 0.0f) + shift,
                                 1, 1, vox};
                        }
                    }
                }
                else
                {
                    // we hit air during process, close the quad if it is being processed
                    if (quadProcessing)
                    {
                        resultQuads.push_back(q);
                        quadProcessing = false;
                    }
                }
            }

            if (quadProcessing)
                // line finished during quad processing
                // we can close the quad and push it back
                resultQuads.push_back(q);
        }

    // TODO a further optimization might be joining same types of quads (matching start, w and v)
    //      into one by increasing their height. Consider if this won't slow our GenerateVBOGreedy
    //      too much.
}

void Chunk::PushVertsFromQuads(const std::vector<quad>& quads, const Vector& normal)
{
    Vector v0, v1, v2, v3;

    for (const auto& q : quads)
    {
        v0 = q.start;
        if (normal[0] != 0.0f)
        {
            // Xrocessing quads from X pass
            v1 = q.start + Vector(0.0f, 0.0f, static_cast<float>(q.w), 0.0f);
            v2 = q.start + Vector(0.0f, static_cast<float>(q.h), 0.0f, 0.0f);
            v3 = q.start + Vector(0.0f, static_cast<float>(q.h), static_cast<float>(q.w), 0.0f);
        }
        else if (normal[1] != 0.0f)
        {
            // Processing quads from Y pass
            v1 = q.start + Vector(static_cast<float>(q.w), 0.0f, 0.0f, 0.0f);
            v2 = q.start + Vector(0.0f, 0.0f, static_cast<float>(q.h), 0.0f);
            v3 = q.start + Vector(static_cast<float>(q.w), 0.0f, static_cast<float>(q.h), 0.0f);
        }
        else if (normal[2] != 0.0f)
        {
            // Processing quads from Z pass
            v1 = q.start + Vector(static_cast<float>(q.w), 0.0f, 0.0f, 0.0f);
            v2 = q.start + Vector(0.0f, static_cast<float>(q.h), 0.0f, 0.0f);
            v3 = q.start + Vector(static_cast<float>(q.w), static_cast<float>(q.h), 0.0f, 0.0f);
        }

        auto voxDataIt = VoxelDB.find(q.v);
        const Voxel& voxData = voxDataIt->second;

        // Vert order: pos.xyz, norm.xyz, col.rgba
        // first vert
        mVerts.push_back(v0[0]); mVerts.push_back(v0[1]); mVerts.push_back(v0[2]);
        mVerts.push_back(normal[0]); mVerts.push_back(normal[1]); mVerts.push_back(normal[2]);
        mVerts.push_back(voxData.colorRed); mVerts.push_back(voxData.colorGreen);
        mVerts.push_back(voxData.colorBlue); mVerts.push_back(ALPHA_COMPONENT);

        // Decide which order to take according to normals (they will help us
        // select which side of the cube are we processing to set the vert order aka. tri strip)
        // For some weird reason, the order was correct for all passes except for X pass.
        // Instead of figuring out what is wrong, it is much easier to just fix a condition.
        if ((normal[0] == 1.0f) || (normal[1] == -1.0f) || (normal[2] == -1.0f))
        {
            // third vert
            mVerts.push_back(v2[0]); mVerts.push_back(v2[1]); mVerts.push_back(v2[2]);
            mVerts.push_back(normal[0]); mVerts.push_back(normal[1]); mVerts.push_back(normal[2]);
            mVerts.push_back(voxData.colorRed); mVerts.push_back(voxData.colorGreen);
            mVerts.push_back(voxData.colorBlue); mVerts.push_back(ALPHA_COMPONENT);

            // second vert
            mVerts.push_back(v1[0]); mVerts.push_back(v1[1]); mVerts.push_back(v1[2]);
            mVerts.push_back(normal[0]); mVerts.push_back(normal[1]); mVerts.push_back(normal[2]);
            mVerts.push_back(voxData.colorRed); mVerts.push_back(voxData.colorGreen);
            mVerts.push_back(voxData.colorBlue); mVerts.push_back(ALPHA_COMPONENT);

            // second vert
            mVerts.push_back(v1[0]); mVerts.push_back(v1[1]); mVerts.push_back(v1[2]);
            mVerts.push_back(normal[0]); mVerts.push_back(normal[1]); mVerts.push_back(normal[2]);
            mVerts.push_back(voxData.colorRed); mVerts.push_back(voxData.colorGreen);
            mVerts.push_back(voxData.colorBlue); mVerts.push_back(ALPHA_COMPONENT);

            // third vert
            mVerts.push_back(v2[0]); mVerts.push_back(v2[1]); mVerts.push_back(v2[2]);
            mVerts.push_back(normal[0]); mVerts.push_back(normal[1]); mVerts.push_back(normal[2]);
            mVerts.push_back(voxData.colorRed); mVerts.push_back(voxData.colorGreen);
            mVerts.push_back(voxData.colorBlue); mVerts.push_back(ALPHA_COMPONENT);
        }
        else
        {
            // second vert
            mVerts.push_back(v1[0]); mVerts.push_back(v1[1]); mVerts.push_back(v1[2]);
            mVerts.push_back(normal[0]); mVerts.push_back(normal[1]); mVerts.push_back(normal[2]);
            mVerts.push_back(voxData.colorRed); mVerts.push_back(voxData.colorGreen);
            mVerts.push_back(voxData.colorBlue); mVerts.push_back(ALPHA_COMPONENT);

            // third vert
            mVerts.push_back(v2[0]); mVerts.push_back(v2[1]); mVerts.push_back(v2[2]);
            mVerts.push_back(normal[0]); mVerts.push_back(normal[1]); mVerts.push_back(normal[2]);
            mVerts.push_back(voxData.colorRed); mVerts.push_back(voxData.colorGreen);
            mVerts.push_back(voxData.colorBlue); mVerts.push_back(ALPHA_COMPONENT);

            // third vert
            mVerts.push_back(v2[0]); mVerts.push_back(v2[1]); mVerts.push_back(v2[2]);
            mVerts.push_back(normal[0]); mVerts.push_back(normal[1]); mVerts.push_back(normal[2]);
            mVerts.push_back(voxData.colorRed); mVerts.push_back(voxData.colorGreen);
            mVerts.push_back(voxData.colorBlue); mVerts.push_back(ALPHA_COMPONENT);

            // second vert
            mVerts.push_back(v1[0]); mVerts.push_back(v1[1]); mVerts.push_back(v1[2]);
            mVerts.push_back(normal[0]); mVerts.push_back(normal[1]); mVerts.push_back(normal[2]);
            mVerts.push_back(voxData.colorRed); mVerts.push_back(voxData.colorGreen);
            mVerts.push_back(voxData.colorBlue); mVerts.push_back(ALPHA_COMPONENT);
        }

        // fourth vert
        mVerts.push_back(v3[0]); mVerts.push_back(v3[1]); mVerts.push_back(v3[2]);
        mVerts.push_back(normal[0]); mVerts.push_back(normal[1]); mVerts.push_back(normal[2]);
        mVerts.push_back(voxData.colorRed); mVerts.push_back(voxData.colorGreen);
        mVerts.push_back(voxData.colorBlue); mVerts.push_back(ALPHA_COMPONENT);
    }
}

void Chunk::GenerateVBOGreedy()
{
    VoxelType voxelsCulled[CHUNK_X * CHUNK_Y * CHUNK_Z];
    for (auto& voxel : voxelsCulled)
        voxel = VoxelType::Air;

    mVerts.clear();
    // First stage of greedy meshing - cull invisible voxels like in Naive alg
    for (int z = 0; z < CHUNK_Z; ++z)
        for (int y = 0; y < CHUNK_Y / 4 + HEIGHTMAP_HEIGHT; ++y)
            for (int x = 0; x < CHUNK_X; ++x)
            {
                VoxelType vox = GetVoxel(x, y, z);
                if (vox != VoxelType::Air && vox != VoxelType::Unknown)
                {
                    // First of all, test only if we are not a bounding voxel chunk.
                    // Otherwise we must add the voxel anyway.
                    if ((x > 0) && (x < CHUNK_X - 1) &&
                        (y > 0) && (y < CHUNK_Y - 1) &&
                        (z > 0) && (z < CHUNK_Z - 1))
                    {
                        // Now see if there is VoxelType::Air in our neighbourhood
                        // If there is none, discard the Voxel.
                        VoxelType voxPlusX = GetVoxel(x+1, y, z);
                        VoxelType voxMinusX = GetVoxel(x-1, y, z);
                        VoxelType voxPlusY = GetVoxel(x, y+1, z);
                        VoxelType voxMinusY = GetVoxel(x, y-1, z);
                        VoxelType voxPlusZ = GetVoxel(x, y, z+1);
                        VoxelType voxMinusZ = GetVoxel(x, y, z-1);

                        if ((voxPlusX != VoxelType::Air) &&
                            (voxMinusX != VoxelType::Air) &&
                            (voxPlusY != VoxelType::Air) &&
                            (voxMinusY != VoxelType::Air) &&
                            (voxPlusZ != VoxelType::Air) &&
                            (voxMinusZ != VoxelType::Air))
                            // We are surrounded by voxels. Ergo, we are not visible.
                            // Discard current voxel to not render unseen voxels.
                            continue;
                    }

                    size_t index;
                    CalculateIndex(x, y, z, index);
                    voxelsCulled[index] = vox;
                }
            }

    // With culled Mesh, we have to do six passes now. two per each axis.
    std::vector<quad> quadsXPlus;
    std::vector<quad> quadsXMinus;
    std::vector<quad> quadsYPlus;
    std::vector<quad> quadsYMinus;
    std::vector<quad> quadsZPlus;
    std::vector<quad> quadsZMinus;

    // All shifts are by +/- 0.5f to match the behavior of Naive generator
    ProcessPlaneX(voxelsCulled, Vector( 0.5f,-0.5f,-0.5f, 0.0f), quadsXPlus);
    ProcessPlaneX(voxelsCulled, Vector(-0.5f,-0.5f,-0.5f, 0.0f), quadsXMinus);
    ProcessPlaneY(voxelsCulled, Vector(-0.5f,-0.5f,-0.5f, 0.0f), quadsYPlus);
    ProcessPlaneY(voxelsCulled, Vector(-0.5f, 0.5f,-0.5f, 0.0f), quadsYMinus);
    ProcessPlaneZ(voxelsCulled, Vector(-0.5f,-0.5f, 0.5f, 0.0f), quadsZPlus);
    ProcessPlaneZ(voxelsCulled, Vector(-0.5f,-0.5f,-0.5f, 0.0f), quadsZMinus);

    // Push the quads and build a Mesh from it
    PushVertsFromQuads(quadsXPlus,  Vector( 1.0f, 0.0f, 0.0f, 0.0f));
    PushVertsFromQuads(quadsXMinus, Vector(-1.0f, 0.0f, 0.0f, 0.0f));
    PushVertsFromQuads(quadsYPlus,  Vector( 0.0f, 1.0f, 0.0f, 0.0f));
    PushVertsFromQuads(quadsYMinus, Vector( 0.0f,-1.0f, 0.0f, 0.0f));
    PushVertsFromQuads(quadsZPlus,  Vector( 0.0f, 0.0f, 1.0f, 0.0f));
    PushVertsFromQuads(quadsZMinus, Vector( 0.0f, 0.0f,-1.0f, 0.0f));

    mMesh.SetPrimitiveType(MeshPrimitiveType::Triangles);
    mState = ChunkState::Generated;
    mGreedyGenerated = true;
}

bool Chunk::SaveToDisk()
{
    // Check if there is data to save
    if (NeedsGeneration())
        return false;

    // Check if our directory exists
    if (!FS::IsDir("./" + CHUNK_DIR))
        FS::CreateDir("./" + CHUNK_DIR);

    std::fstream file;

    // Construct filename
    std::string fileName(CHUNK_DIR + "/Chunk_" + std::to_string(mCoordX) + '_'
                         + std::to_string(mCoordZ) + CHUNK_FILEEXT);

    file.open(fileName, std::ios::out | std::ios::trunc);

    if (file.is_open())
    {
        VoxelType tempVox = mVoxels[0];
        VoxelType lastVox;
        uint32_t counter;
        for (int i = 0; i < CHUNK_X * CHUNK_Y * CHUNK_Z; )
        {
            lastVox = tempVox;
            counter = 1;
            i++;

            while (lastVox == tempVox && i < CHUNK_X * CHUNK_Y * CHUNK_Z)
            {
                counter++;
                tempVox = mVoxels[i++];
            }

            if (i < CHUNK_X * CHUNK_Y * CHUNK_Z)
            {
                counter--;
                i--;
            }

            file << counter;
            file << static_cast<VoxelUnderType>(lastVox);

            if (!file)
            {
                LOG_E("Writing to file \"" << fileName << "\" failed.");
                return false;
            }
        }

        file.close();
        return true;
    } else
    {
        LOG_E("Failed to open file \"" << fileName << "\" for writing.");
        return false;
    }
}

bool Chunk::LoadFromDisk()
{
    std::fstream file;

    // Construct filename
    std::string fileName(CHUNK_DIR + "/Chunk_" + std::to_string(mCoordX) + '_'
                         + std::to_string(mCoordZ) + CHUNK_FILEEXT);

    file.open(fileName, std::ios::in);

    if (file.is_open())
    {
        VoxelUnderType tempVox;
        uint32_t counter;
        for (int i = 0; i < CHUNK_X * CHUNK_Y * CHUNK_Z; )
        {
            file >> counter;
            file >> tempVox;

            do
            {
                counter--;
                mVoxels[i++] = static_cast<VoxelType>(tempVox);
            } while (counter && i < CHUNK_X * CHUNK_Y * CHUNK_Z);

            if (!file)
                return false;
        }

        file.close();
        return true;
    } else
        return false;
}

bool Chunk::ChunkRayIntersection(Vector pos, Vector dir, float &distance, Vector &coords)
{
    Matrix worldMat(GetMeshPtr()->GetWorldMatrixRaw());
    float retDist = std::numeric_limits<float>::max();
    Vector retCoords;
    Vector voxShift(0.5, 0.5, 0.5, 0);

    for (int z = 0; z < CHUNK_Z; ++z)
        for (int y = 0; y < CHUNK_Y; ++y)
            for (int x = 0; x < CHUNK_X; ++x)
                if (GetVoxel(x, y, z) != VoxelType::Air)
                {
                    Vector obb_min(static_cast<float>(x),
                                    static_cast<float>(y),
                                    static_cast<float>(z),
                                    1);
                    Vector obb_max(obb_min);
                    obb_min -= voxShift;
                    obb_max += voxShift;
                    float tempDist;
                    if (OBBRayIntersection(pos, dir, obb_min, obb_max,
                                               worldMat, tempDist))
                        if (tempDist < retDist)
                        {
                            retDist = tempDist;
                            retCoords = Vector(static_cast<float>(x),
                                               static_cast<float>(y),
                                               static_cast<float>(z),
                                               1.0f);
                        }
                }
    if (retCoords == Vector())
        return false;

    distance = retDist;
    coords = retCoords;

    LOG_D("Ray intersection worked for chunk [" << mCoordX << "," << mCoordZ
          << "], voxel[" << coords[0] << "," << coords[1] << "," << coords[2] << "]. Distance"
          << " = " << distance << ".");

    return true;
}

bool Chunk::OBBRayIntersection(Vector pos, Vector dir, Vector obb_min, Vector obb_max,
                               Matrix worldMat, float& intersectionDist)
{
    // Intersection method from Real-Time Rendering and Essential Mathematics for Games

    float tMin = 0.0f;
    float tMax = 100000.0f;

    Vector OBBposition_worldspace(worldMat[12], worldMat[13], worldMat[14], 1.0f);

    Vector delta = OBBposition_worldspace - pos;

    // Test intersection with the 2 planes perpendicular to the OBB's X axis
    {
        Vector xaxis(worldMat[0], worldMat[1], worldMat[2], 1.0f);
        float e = xaxis.Dot(delta);
        float f = dir.Dot(xaxis);

        if (fabs(f) > 0.001f)
        { // Standard case

            float t1 = (e + obb_min[0]) / f; // Intersection with the "left" plane
            float t2 = (e + obb_max[0]) / f; // Intersection with the "right" plane
                                             // t1 and t2 now contain distances betwen ray origin and ray-plane intersections

                                             // We want t1 to represent the nearest intersection,
                                             // so if it's not the case, invert t1 and t2
            if (t1>t2)
            {
                float w = t1; t1 = t2; t2 = w; // swap t1 and t2
            }

            // tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
            if (t2 < tMax)
                tMax = t2;
            // tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
            if (t1 > tMin)
                tMin = t1;

            // And here's the trick :
            // If "far" is closer than "near", then there is NO intersection.
            // See the images in the tutorials for the visual explanation.
            if (tMax < tMin)
                return false;

        }
        else
        { // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
            if (-e + obb_min[0] > 0.0f || -e + obb_max[0] < 0.0f)
                return false;
        }
    }


    // Test intersection with the 2 planes perpendicular to the OBB's Y axis
    // Exactly the same thing than above.
    {
        Vector yaxis(worldMat[4], worldMat[5], worldMat[6], 1.0f);
        float e = yaxis.Dot(delta);
        float f = dir.Dot(yaxis);

        if (fabs(f) > 0.001f)
        {

            float t1 = (e + obb_min[1]) / f;
            float t2 = (e + obb_max[1]) / f;

            if (t1>t2) { float w = t1; t1 = t2; t2 = w; }

            if (t2 < tMax)
                tMax = t2;
            if (t1 > tMin)
                tMin = t1;
            if (tMin > tMax)
                return false;

        }
        else
        {
            if (-e + obb_min[1] > 0.0f || -e + obb_max[1] < 0.0f)
                return false;
        }
    }


    // Test intersection with the 2 planes perpendicular to the OBB's Z axis
    // Exactly the same thing than above.
    {
        Vector zaxis(worldMat[8], worldMat[9], worldMat[10], 1.0f);
        float e = zaxis.Dot(delta);
        float f = dir.Dot(zaxis);

        if (fabs(f) > 0.001f)
        {

            float t1 = (e + obb_min[2]) / f;
            float t2 = (e + obb_max[2]) / f;

            if (t1>t2) { float w = t1; t1 = t2; t2 = w; }

            if (t2 < tMax)
                tMax = t2;
            if (t1 > tMin)
                tMin = t1;
            if (tMin > tMax)
                return false;

        }
        else
        {
            if (-e + obb_min[2] > 0.0f || -e + obb_max[2] < 0.0f)
                return false;
        }
    }

    intersectionDist = tMin;
    return true;
}
