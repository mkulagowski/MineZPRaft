/**
 * @file
 * @author mkkulagowski (mkkulagowski(at)gmail.com)
 * @brief  Renderer Texture declarations
 */

#ifndef __RENDERER_TEXTURE_HPP__
#define __RENDERER_TEXTURE_HPP__

#include "Defines.hpp"

class Texture
{
public:
    Texture();
    Texture(std::string path);
    ~Texture();


    void LoadBMP(std::string path);
    void LoadDDS(std::string path);
    GLuint GetTextureId();

private:
    GLuint mTextureId;
};

#endif // __RENDERER_TEXTURE_HPP__
#pragma once
