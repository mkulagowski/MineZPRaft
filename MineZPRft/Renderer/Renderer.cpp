/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  OpenGL Renderer definitions
 */

#include "Renderer.hpp"

#include "Common/Common.hpp"
#include "Extensions.hpp"
#include "Math/Vector.hpp"
#include "Common/Logger.hpp"

#include <iostream>

using namespace OGLExt;

Renderer::Renderer()
    : mCamera()
    , mTerrainShaderNaive()
    , mTerrainShaderUniforms()
    , mDummyVAO(GL_NONE)
    , initDone(false)
{
}

Renderer::~Renderer()
{
    // unbind
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    glBindVertexArray(GL_NONE);

    // destroy
    glDeleteVertexArrays(1, &mDummyVAO);
}

Renderer& Renderer::GetInstance()
{
    static Renderer instance;
    return instance;
}

void Renderer::Init(const RendererDesc& desc)
{
    const GLubyte* glv = glGetString(GL_VERSION);
    std::string glvStr(reinterpret_cast<const char*>(glv));
    LOG_I("Loading OpenGL " << glvStr << " Renderer");

    // initialize OpenGL extensions
    if (!ExtensionsInit())
    {
        // TODO exception
        return;
    }

    // renderer viewport & pipeline setups
    glViewport(0, 0, desc.windowWidth, desc.windowHeight);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    // Generate VAO and bind it because OGL
    glGenVertexArrays(1, &mDummyVAO);
    glBindVertexArray(mDummyVAO);

    // Load camera
    CameraDesc cd;
    cd.fov = 60.0f;
    cd.aspectRatio = static_cast<float>(desc.windowWidth) / static_cast<float>(desc.windowHeight);
    cd.nearDist = 0.1f;
    cd.farDist = 1000.0f;
    cd.initialView.pos = Vector(3.0f, 0.0f,-3.0f, 1.0f);
    cd.initialView.dir = Vector(0.0f, 0.0f, 0.0f, 1.0f);
    cd.initialView.up = Vector(0.0f, 1.0f, 0.0f, 0.0f);
    mCamera.Init(cd);

    // Color used to clear buffers
    glClearColor(0.1f, 0.3f, 0.7f, 0.0f);

    // Load terrain shader
    ShaderDesc sd;
    sd.vsPath = desc.shaderPath + "/TerrainNaiveVS.glsl";
    sd.gsPath = desc.shaderPath + "/TerrainNaiveGS.glsl";
    sd.fsPath = desc.shaderPath + "/TerrainNaiveFS.glsl";
    mTerrainShaderNaive.Init(sd);

    // Initialize uniforms constant throughout program lifetime
    mTerrainShaderNaive.MakeCurrent();
    mTerrainShaderUniforms.worldMatrix = mTerrainShaderNaive.GetUniform("worldMat");
    mTerrainShaderUniforms.viewMatrix = mTerrainShaderNaive.GetUniform("viewMat");
    mTerrainShaderUniforms.perspectiveMatrix = mTerrainShaderNaive.GetUniform("perspMat");
    mTerrainShaderUniforms.playerPos = mTerrainShaderNaive.GetUniform("playerPos");
    // TODO throw if incorrect uniform locations

    // Load main shader
    sd.vsPath = desc.shaderPath + "/MainVS.glsl";
    sd.gsPath.clear();
    sd.fsPath = desc.shaderPath + "/MainFS.glsl";
    mMainShader.Init(sd);

    // Initialize uniforms constant throughout program lifetime
    mMainShader.MakeCurrent();
    mMainShaderUniforms.worldMatrix = mMainShader.GetUniform("worldMat");
    mMainShaderUniforms.viewMatrix = mMainShader.GetUniform("viewMat");
    mMainShaderUniforms.perspectiveMatrix = mMainShader.GetUniform("perspMat");
    mMainShaderUniforms.playerPos = mMainShader.GetUniform("playerPos");
    // TODO throw if incorrect uniform locations

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    initDone = true;
}

void Renderer::AddMesh(const Mesh* mesh) noexcept
{
    mMeshArray.push_back(mesh);
}

void Renderer::ReplaceTerrainMesh(size_t index, const Mesh* mesh) noexcept
{
    mTerrainMeshArray[index] = mesh;
}

void Renderer::ReserveTerrainMeshPool(size_t meshCount) noexcept
{
    mTerrainMeshArray.resize(meshCount);
}

void Renderer::Draw() noexcept
{
    // Clear the buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw all mesh arrays
    DrawPerMeshArray(mTerrainMeshArray);
    DrawPerMeshArray(mMeshArray);

    // When v-sync is off, this function assures that the application will not move on
    // until all OpenGL calls are processed by the driver.
    glFinish();
}

void Renderer::ResizeViewport(GLsizei w, GLsizei h)
{
    if (w < 1 || h < 1)
        return;

    glViewport(0, 0, w, h);

    CameraDesc cd;

    cd.fov = 60.0f;
    cd.aspectRatio = static_cast<float>(w) / static_cast<float>(h);
    cd.nearDist = 0.1f;
    cd.farDist = 1000.0f;

    mCamera.UpdatePerspective(cd);

    if (initDone)
        glUniformMatrix4fv(mTerrainShaderUniforms.perspectiveMatrix, 1, false,
                           mCamera.GetPerspectiveRaw());
}

Camera* Renderer::GetCameraPtr()
{
    return &mCamera;
}

void Renderer::DrawPerMeshArray(const MeshArrayType& meshArray)
{
    GLsizei vertCount;
    for (const auto& mesh : meshArray)
    {
        if (!mesh->IsLocked())
        {
            const GLenum primType = mesh->GetGLPrimitiveType();
            mesh->Bind();

            if (primType == GL_POINTS)
            {
                mTerrainShaderNaive.MakeCurrent();
                glUniformMatrix4fv(mTerrainShaderUniforms.worldMatrix, 1, false,
                                   mesh->GetWorldMatrixRaw());
                glUniformMatrix4fv(mTerrainShaderUniforms.viewMatrix, 1, false,
                                   mCamera.GetViewRaw());
                glUniformMatrix4fv(mTerrainShaderUniforms.perspectiveMatrix, 1, false,
                                   mCamera.GetPerspectiveRaw());
                const float* posRaw = mCamera.GetPosRaw();
                glUniform4f(mTerrainShaderUniforms.playerPos,
                            posRaw[0], posRaw[1], posRaw[2], 1.0f);
            }
            else if (primType == GL_TRIANGLES)
            {
                mMainShader.MakeCurrent();
                glUniformMatrix4fv(mMainShaderUniforms.worldMatrix, 1, false,
                                   mesh->GetWorldMatrixRaw());
                glUniformMatrix4fv(mMainShaderUniforms.viewMatrix, 1, false,
                                   mCamera.GetViewRaw());
                glUniformMatrix4fv(mMainShaderUniforms.perspectiveMatrix, 1, false,
                                   mCamera.GetPerspectiveRaw());
                const float* posRaw = mCamera.GetPosRaw();
                glUniform4f(mMainShaderUniforms.playerPos,
                            posRaw[0], posRaw[1], posRaw[2], 1.0f);
            }

            vertCount = mesh->GetVertCount();
            if (vertCount > 0)
                glDrawArrays(primType, 0, mesh->GetVertCount());
        }
    }
}
