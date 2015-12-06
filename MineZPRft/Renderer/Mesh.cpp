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
{
}

Mesh::~Mesh()
{
}

void Mesh::Init(const MeshDesc& desc)
{
    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, desc.dataSize, desc.dataPtr, GL_STATIC_DRAW);

    // Define Vertex Attributes
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // TODO TEMPORARY - probably each mesh will have its own Vertex Attributes specified.
    //      Plus, there is no need to do it per mesh initialization.
    // Attribute 0 - Vertex position, at stride = 0 (the beginning of specified vertex)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 28,
                          reinterpret_cast<const void*>(0));
    // Attribute 1 - Vertex color at stride = 3 * sizeof(float) (in other words, right after
    //               Vertex Position attribute).
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 28,
                          reinterpret_cast<const void*>(3 * sizeof(float)));

    mVertCount = static_cast<GLsizei>(desc.vertCount);
}

void Mesh::Bind() const noexcept
{
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
}

GLsizei Mesh::GetVertCount() const noexcept
{
    return mVertCount;
}

void Mesh::Update(const MeshUpdateDesc& desc) noexcept
{
    UNUSED(desc);
}
