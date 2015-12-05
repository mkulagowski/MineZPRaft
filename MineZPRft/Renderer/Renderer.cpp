/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  OpenGL Renderer definitions
 */

#include "Renderer.hpp"

#include "Common/Common.hpp"
#include "Extensions.hpp"
#include "Math/Vector.hpp"

#include <iostream>

using namespace OGLExt;

Renderer::Renderer()
    : mCamera()
    , mMainShader()
    , mMainShaderViewMatrixLoc(GL_NONE)
    , mMainShaderPerspectiveMatrixLoc(GL_NONE)
    , mDummyVAO(GL_NONE)
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
    // TODO log
    std::cerr << "Loading OpenGL " << glvStr << " Renderer" << std::endl;

    // initialize OpenGL extensions
    if (!ExtensionsInit())
    {
        // TODO exception
        return;
    }

    // renderer viewport & pipeline setups
    glViewport(0, 0, desc.windowWidth, desc.windowHeight);
    glCullFace(GL_FRONT_AND_BACK);
    glFrontFace(GL_CW);
    glEnable(GL_DEPTH_TEST);

    // Generate VAO and bind it because OGL
    glGenVertexArrays(1, &mDummyVAO);
    glBindVertexArray(mDummyVAO);

    // Load camera
    CameraDesc cd;
    cd.fov = 45.0f;
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
    mMainShaderViewMatrixLoc = mMainShader.GetUniform("viewMat");
    mMainShaderPerspectiveMatrixLoc = mMainShader.GetUniform("perspMat");
    // TODO throw if incorrect uniform locations

    glUniformMatrix4fv(mMainShaderPerspectiveMatrixLoc, 1, false, mCamera.GetPerspectiveRaw());
}

void Renderer::AddMesh(const Mesh* mesh)
{
    mMeshArray.push_back(mesh);
}

void Renderer::Draw() noexcept
{
    // clear the buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // drawing pass
    mMainShader.MakeCurrent();
    glUniformMatrix4fv(mMainShaderViewMatrixLoc, 1, false, mCamera.GetViewRaw());

    for (const auto& mesh : mMeshArray)
    {
        mesh->Bind();

        glDrawArrays(GL_POINTS, 0, 3);
    }

    glFinish();
}

Camera* Renderer::GetCameraPtr()
{
    return &mCamera;
}
