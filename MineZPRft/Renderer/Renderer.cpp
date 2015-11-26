/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  OpenGL Renderer definitions
 */

#include "Renderer.hpp"

#include "Common/Common.hpp"
#include "Extensions.hpp"

#include <iostream>

using namespace OGLExt;

Renderer::Renderer()
    : mCamera()
    , mMainShader()
    , mVB(GL_NONE)
    , mDummyVAO(GL_NONE)
{
}

Renderer::~Renderer()
{
    // unbind
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    glBindVertexArray(GL_NONE);

    // destroy
    glDeleteBuffers(1, &mVB);
    glDeleteVertexArrays(1, &mDummyVAO);
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

    // Generate VAO and bind it because OGL
    glGenVertexArrays(1, &mDummyVAO);
    glBindVertexArray(mDummyVAO);

    // Load shader
    ShaderDesc sd;
    sd.vsPath = desc.shaderPath + "/MainVS.glsl";
    sd.fsPath = desc.shaderPath + "/MainFS.glsl";
    mMainShader.Init(sd);

    // Create VBO
    float verts[] =
    {
    // Following structure is in this temporary VBO:
    //  pos.xyz,          color.rgba
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    };

    glGenBuffers(1, &mVB);
    glBindBuffer(GL_ARRAY_BUFFER, mVB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    // Define Vertex Attributes
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 28, reinterpret_cast<const void*>(0));
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 28, reinterpret_cast<const void*>(12));

    // Color used to clear buffers
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void Renderer::AddMesh(const Mesh* mesh)
{
    mMeshArray.push_back(mesh);
}

void Renderer::Draw() noexcept
{
    // clear the buffer
    glClear(GL_COLOR_BUFFER_BIT);

    // drawing pass
    mMainShader.MakeCurrent();

    glDrawArrays(GL_TRIANGLES, 0, 3);
    glFinish();
}
