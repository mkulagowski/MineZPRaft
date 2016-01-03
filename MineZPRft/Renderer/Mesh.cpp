/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Renderer Mesh definitions
 */

#include "Mesh.hpp"

#include "Extensions.hpp"
#include "Common/Common.hpp"

using namespace OGLExt;

Mesh::Mesh()
    : mVBO(GL_NONE)
    , mVertCount(0)
    , mWorldMatrix(MATRIX_IDENTITY)
    , mLocked(false)
{
}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &mVBO);
}

void Mesh::Init(const MeshDesc& desc)
{
    if (mVBO != GL_NONE)
    {
        LOG_E("Mesh is already initialized! Use Mesh::Update() to update its contents.");
        // TODO exception
        return;
    }

    glGenBuffers(1, &mVBO);

    if (desc.dataPtr != 0)
    {
        Update({desc.dataPtr, desc.dataSize, desc.vertCount});
    }
}

void Mesh::Bind() const noexcept
{
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);

    // TODO Probably the attributes are chunk-specific. Introduction of new Meshes might
    //      be then problematic. Consider adding "VertexLayout" class or similar.
    // Attribute 0 - Vertex position, at stride = 0 (the beginning of specified vertex)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 28,
                          reinterpret_cast<const void*>(0));
    // Attribute 1 - Vertex color at stride = 3 * sizeof(float) (in other words, right after
    //               Vertex Position attribute).
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 28,
                          reinterpret_cast<const void*>(3 * sizeof(float)));
}

void Mesh::SetWorldMatrix(const Matrix& matrix) noexcept
{
    mWorldMatrix = matrix;
}

const float* Mesh::GetWorldMatrixRaw() const noexcept
{
    return mWorldMatrix.Data();
}

GLsizei Mesh::GetVertCount() const noexcept
{
    return mVertCount;
}

void Mesh::Update(const MeshUpdateDesc& desc) noexcept
{
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, desc.dataSize, desc.dataPtr, GL_STATIC_DRAW);
    mVertCount = static_cast<GLsizei>(desc.vertCount);
}

void Mesh::SetLocked(bool locked) noexcept
{
    mLocked = locked;
}

bool Mesh::IsLocked() const noexcept
{
    return mLocked;
}
