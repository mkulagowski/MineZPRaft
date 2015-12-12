/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Renderer's Shader definitions
 */

#include "Shader.hpp"

#include "Extensions.hpp"
#include "Common/Logger.hpp"

#include <fstream>
#include <vector>
#include <iostream>

using namespace OGLExt;

namespace {

const std::string DEFINE_STR = "#define";
const std::string SHADER_HEADER = "#version 330 core\n\
#line 0 1\n"; // to correct the line counter in shader info logs

} // namespace

Shader::Shader()
    : mShaderProgram(GL_NONE)
{
}

Shader::~Shader()
{
    glDeleteProgram(mShaderProgram);
}

void Shader::Init(const ShaderDesc& desc)
{
    GLuint vs = CreateShader(GL_VERTEX_SHADER, desc.vsPath);
    GLuint fs = CreateShader(GL_FRAGMENT_SHADER, desc.fsPath);

    GLuint gs = GL_NONE;
    if (!desc.gsPath.empty())
        gs = CreateShader(GL_GEOMETRY_SHADER, desc.gsPath);

    // Create a shader program
    mShaderProgram = glCreateProgram();

    // link shaders into a shader program
    glAttachShader(mShaderProgram, vs);
    if (gs != GL_NONE)
        glAttachShader(mShaderProgram, gs);
    glAttachShader(mShaderProgram, fs);
    glLinkProgram(mShaderProgram);

    // Cleanup shaders
    glDeleteShader(vs);
    glDeleteShader(fs);
    if (gs)
        glDeleteShader(gs);

    int programStatus = 0;
    glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &programStatus);
    if (!programStatus)
    {
        LOG_ERROR("Shader Program failed to link." << std::endl);

        int logLength = 0;
        glGetProgramiv(mShaderProgram, GL_INFO_LOG_LENGTH, &logLength);
        // When empty, Info Log contains only \0 char (thus the length is 1)
        if (logLength > 1)
        {
            std::vector<char> log(logLength);
            glGetProgramInfoLog(mShaderProgram, logLength, &logLength, log.data());
            LOG_INFO("Shader Program linking output:\n" << log.data()
                     << std::endl);
        }

        // TODO exception
        return;
    }

    LOG_INFO("Successfully compiled and linked shader." << std::endl);
}

void Shader::MakeCurrent() noexcept
{
    glUseProgram(mShaderProgram);
}

GLint Shader::GetUniform(const char* name) noexcept
{
    return glGetUniformLocation(mShaderProgram, name);
}

GLuint Shader::CreateShader(const GLenum shaderType, const std::string& shaderPath)
{
    std::string shaderHead = SHADER_HEADER;
    // TODO attach macros if needed to shaderHead

    // null-termination will make our work with glShaderSource easier
    shaderHead += '\0';

    std::ifstream shaderFile(shaderPath);
    if (!shaderFile)
    {
        LOG_ERROR("Invalid shader file: " << shaderPath << std::endl);
        // TODO exception
        return GL_NONE;
    }
    std::string shaderStr((std::istreambuf_iterator<char>(shaderFile)),
                           std::istreambuf_iterator<char>());

    shaderStr += '\0';

    GLuint shader = glCreateShader(shaderType);
    if (!shader)
    {
        LOG_ERROR("Failed to create Shader Object for " << shaderPath
                  << std::endl);
        // TODO exception
        return GL_NONE;
    }

    const char* shaderCStr[] = { shaderHead.c_str(), shaderStr.c_str() };
    glShaderSource(shader, 2, shaderCStr, nullptr);
    glCompileShader(shader);

    int shaderStatus = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderStatus);
    if (!shaderStatus)
    {
        LOG_ERROR("Failed to compile shader " << shaderPath << std::endl);

        int logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        // When empty, Info Log contains only \0 char (thus the length is 1)
        if (logLength > 1)
        {
            std::vector<char> log(logLength);
            glGetShaderInfoLog(shader, logLength, &logLength, log.data());

            LOG_INFO("Shader '" << shaderPath << "' compilation output:\n"
                     << log.data() << std::endl);
        }

        // TODO exception
        return GL_NONE;
    }

    LOG_INFO("Compiled Shader " << shaderPath << std::endl);

    return shader;
}
