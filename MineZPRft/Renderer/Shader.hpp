/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Renderer's Shader declarations
 */

#ifndef __RENDERER_SHADER_HPP__
#define __RENDERER_SHADER_HPP__

#include "Defines.hpp"

#include <string>

/**
 * Structure describing data needed to initialize a Shader object
 */
struct ShaderDesc
{
    std::string vsPath; ///< Path to Vertex Shader source
    std::string gsPath; ///< Path to Geometry Shader source (optional)
    std::string fsPath; ///< Path to Fragment Shader source
};

class Shader
{
public:
    Shader();
    ~Shader();

    /**
     * Initializes the Shader object.
     *
     * @param desc Shader description structure.
     *
     * The function creates loads sources from ShaderDesc, compiles them to OpenGL Shader Objects
     * and links them into one OpenGL Shader Program Object.
     *
     * @remarks The function might throw when there is an error during Shader loading.
     */
    void Init(const ShaderDesc& desc);

    /**
     * Binds Shader object inside OpenGL to use it for rendering.
     *
     * @remarks The function will be used during main draw loop and will not throw.
     */
    void MakeCurrent() noexcept;

    /**
     * Retrieves @p name uniform from Shader Program.
     *
     * @param name Name of uniform to update
     *
     * @remarks The function will be used during main draw loop and for performance will not throw.
     */
    GLint GetUniform(const char* name) noexcept;

private:
    /**
     * Creates a Shader object of specified type from specified shader path.
     */
    GLuint CreateShader(const GLenum shaderType, const std::string& shaderPath);

    GLuint mShaderProgram; ///< OpenGL Shader Program ID
};

#endif // __RENDERER_SHADER_HPP__
