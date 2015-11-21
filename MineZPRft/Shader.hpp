/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Renderer's Shader declarations
 */

#pragma once

struct ShaderDesc
{
    // TODO paths to all needed shaders
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
     * @remarks The function might throw when there is an error during Shader load.
     */
    void Init(const ShaderDesc& desc);

    /**
     * Binds Shader object inside OpenGL to use it for rendering.
     *
     * @remarks The function will be used during main draw loop and will not throw.
     */
    void MakeCurrent() noexcept;

private:
    // TODO OpenGL Shader IDs and other required stuff
};
