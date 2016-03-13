/**
 * @file
 * @author mkkulagowski (mkkulagowski(at)gmail.com)
 * @brief  Renderer Texture definitions
 */

#include "Texture.hpp"

#include "Extensions.hpp"
#include "Common/Common.hpp"

Texture::Texture()
    : mTextureId(GL_NONE)
{
    glGenTextures(1, &mTextureId);
}

Texture::Texture(std::string path)
    : Texture()
{
    LoadDDS(path);
}

Texture::~Texture()
{
    glDeleteTextures(1, &mTextureId);
}

void Texture::LoadBMP(std::string path)
{
    LOG_D("Reading image " << path);

    // Data read from the header of the BMP file
    unsigned char header[54];
    unsigned int dataPos;
    unsigned int imageSize;
    unsigned int width, height;
    // Actual RGB data
    unsigned char * data;

    // Open the file
    FILE *file;
    fopen_s(&file, path.c_str(), "rb");
    if (!file)
    {
        LOG_E(path << " could not be opened. Have you given the right path ?\n");
        return;
    }

    // Read the header, i.e. the 54 first bytes

    // If less than 54 bytes are read, problem
    if (fread(header, 1, 54, file) != 54)
    {
        LOG_E("Not a correct BMP file\n");
        return;
    }
    // A BMP files always begins with "BM"
    if (header[0] != 'B' || header[1] != 'M')
    {
        LOG_E("Not a correct BMP file\n");
        return;
    }
    // Make sure this is a 24bpp file
    if (*(int*)&(header[0x1E]) != 0)
    {
        LOG_E("Not a correct BMP file\n");
        return;
    }
    if (*(int*)&(header[0x1C]) != 24)
    {
        LOG_E("Not a correct BMP file\n");
        return;
    }

    // Read the information about the image
    dataPos = *(int*)&(header[0x0A]);
    imageSize = *(int*)&(header[0x22]);
    width = *(int*)&(header[0x12]);
    height = *(int*)&(header[0x16]);

    // Some BMP files are misformatted, guess missing information
    if (imageSize == 0)    imageSize = width*height * 3; // 3 : one byte for each Red, Green and Blue component
    if (dataPos == 0)      dataPos = 54; // The BMP header is done that way

                                         // Create a buffer
    data = new unsigned char[imageSize];

    // Read the actual data from the file into the buffer
    fread(data, 1, imageSize, file);

    // Everything is in memory now, the file can be closed
    fclose(file);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, mTextureId);

    // Give the image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

    // OpenGL has now copied the data. Free our own version
    delete[] data;

    // Poor filtering, or ...
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // ... nice trilinear filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::LoadDDS(std::string path)
{
}


GLuint Texture::GetTextureId()
{
    return mTextureId;
}
