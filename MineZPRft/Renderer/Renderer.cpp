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
    , mMainShader()
    , mMainShaderWorldMatrixLoc(GL_NONE)
    , mMainShaderViewMatrixLoc(GL_NONE)
    , mMainShaderPerspectiveMatrixLoc(GL_NONE)
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

    // Load shader
    ShaderDesc sd;
    sd.vsPath = desc.shaderPath + "/MainVS.glsl";
    sd.gsPath = desc.shaderPath + "/MainGS.glsl";
    sd.fsPath = desc.shaderPath + "/MainFS.glsl";
    mMainShader.Init(sd);

    // Color used to clear buffers
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Initialize uniforms constant throughout program lifetime
    mMainShader.MakeCurrent();
    mMainShaderWorldMatrixLoc = mMainShader.GetUniform("worldMat");
    mMainShaderViewMatrixLoc = mMainShader.GetUniform("viewMat");
    mMainShaderPerspectiveMatrixLoc = mMainShader.GetUniform("perspMat");
    mMainShaderPlayerPosLoc = mMainShader.GetUniform("playerPos");
    // TODO throw if incorrect uniform locations

    glUniformMatrix4fv(mMainShaderPerspectiveMatrixLoc, 1, false, mCamera.GetPerspectiveRaw());

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

    // Drawing pass
    mMainShader.MakeCurrent();
    glUniformMatrix4fv(mMainShaderViewMatrixLoc, 1, false, mCamera.GetViewRaw());

    // Provide the shader with Camera position for lighting/shading calculations
    const float* posRaw = mCamera.GetPosRaw();
    glUniform4f(mMainShaderPlayerPosLoc, posRaw[0], posRaw[1], posRaw[2], posRaw[3]);

    // Draw all "regular" meshes provided
    GLsizei vertCount;
    for (const auto& mesh : mMeshArray)
    {
        if (!mesh->IsLocked())
        {
            // TODO this needs small improvements for regular meshes
            /*
            mesh->Bind();
            glUniformMatrix4fv(mMainShaderWorldMatrixLoc, 1, false, mesh->GetWorldMatrixRaw());

            vertCount = mesh->GetVertCount();
            if (vertCount > 0)
                glDrawArrays(GL_POINTS, 0, mesh->GetVertCount());
            */
        }
    }

    // Do the drawing for Terrain Meshes
    for (const auto& mesh : mTerrainMeshArray)
    {
        if (!mesh->IsLocked())
        {
            mesh->Bind();
            glUniformMatrix4fv(mMainShaderWorldMatrixLoc, 1, false, mesh->GetWorldMatrixRaw());

            vertCount = mesh->GetVertCount();
            if (vertCount > 0)
                glDrawArrays(GL_POINTS, 0, mesh->GetVertCount());
        }
    }

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
        glUniformMatrix4fv(mMainShaderPerspectiveMatrixLoc, 1, false,
                           mCamera.GetPerspectiveRaw());
}

Camera* Renderer::GetCameraPtr()
{
    return &mCamera;
}
