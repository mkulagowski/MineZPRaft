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
const int FLOAT_COUNT_PER_VERTEX = 7;
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
    MeshDesc md;
    md.dataPtr = mVerts.data();
    md.dataSize = mVerts.size() * sizeof(float);
    md.vertCount = mVerts.size() / FLOAT_COUNT_PER_VERTEX;
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
    Vector shift(static_cast<float>((chunkX - 0.5) * CHUNK_X),
                 static_cast<float>((-(CHUNK_Y / 4) - HEIGHTMAP_HEIGHT)),
                 static_cast<float>((chunkZ - 0.5) * CHUNK_Z),
                 0.0f);
    // TODO rotation should be unnecessary! Probably a bug in Perlin
    mMesh.SetWorldMatrix(CreateTranslationMatrix(shift) * CreateRotationMatrixY(MATH_PIF));
}

void Chunk::Generate(int chunkX, int chunkZ, int currentChunkX, int currentChunkZ) noexcept
{
    // Set coords for chunk
    mCoordX = chunkX + currentChunkX;
    mCoordZ = chunkZ + currentChunkZ;

    // If Chunk was saved to disk, load it from file.
    if (LoadFromDisk())
    {
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

    GenerateVBONaive();

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
    md.vertCount = mVerts.size() / FLOAT_COUNT_PER_VERTEX;
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
}

void Chunk::GenerateVBOGreedy()
{
    for (int z = 0; z < CHUNK_Z; ++z)
        for (int y = 0; y < CHUNK_Y / 4 + HEIGHTMAP_HEIGHT; ++y)
            for (int x = 0; x < CHUNK_X; ++x)
            {
            }
    mMesh.SetPrimitiveType(MeshPrimitiveType::Triangles);
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