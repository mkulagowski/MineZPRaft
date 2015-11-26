/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  OpenGL Extensions definitions
 */

#include "Extensions.hpp"

#include "Common/GetExtension.hpp"


namespace OGLExt {

/// Buffers
PFNGLGENBUFFERSPROC glGenBuffers = nullptr;
PFNGLBINDBUFFERPROC glBindBuffer = nullptr;
PFNGLBUFFERDATAPROC glBufferData = nullptr;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = nullptr;

/// Vertex Attrib Array
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = nullptr;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = nullptr;

/// Vertex Array Objects
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = nullptr;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray = nullptr;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = nullptr;

/// Shaders
PFNGLCREATESHADERPROC glCreateShader = nullptr;
PFNGLSHADERSOURCEPROC glShaderSource = nullptr;
PFNGLCOMPILESHADERPROC glCompileShader = nullptr;
PFNGLGETSHADERIVPROC glGetShaderiv = nullptr;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = nullptr;
PFNGLDELETESHADERPROC glDeleteShader = nullptr;
PFNGLCREATEPROGRAMPROC glCreateProgram = nullptr;
PFNGLATTACHSHADERPROC glAttachShader = nullptr;
PFNGLLINKPROGRAMPROC glLinkProgram = nullptr;
PFNGLGETPROGRAMIVPROC glGetProgramiv = nullptr;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = nullptr;
PFNGLDELETEPROGRAMPROC glDeleteProgram = nullptr;
PFNGLUSEPROGRAMPROC glUseProgram = nullptr;

bool ExtensionsInit()
{
    bool allExtensionsAvailable = true;

    // TODO acquire GL EXT String and check if all extensions we need
    //      are available.

    // Buffers
    OGL_GET_EXTENSION(PFNGLGENBUFFERSPROC, glGenBuffers);
    OGL_GET_EXTENSION(PFNGLBINDBUFFERPROC, glBindBuffer);
    OGL_GET_EXTENSION(PFNGLBUFFERDATAPROC, glBufferData);
    OGL_GET_EXTENSION(PFNGLDELETEBUFFERSPROC, glDeleteBuffers);

    // Vertex Attrib Array
    OGL_GET_EXTENSION(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray);
    OGL_GET_EXTENSION(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer);
    OGL_GET_EXTENSION(PFNGLDISABLEVERTEXATTRIBARRAYPROC, glDisableVertexAttribArray);

    // Vertex Array Objects
    OGL_GET_EXTENSION(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays);
    OGL_GET_EXTENSION(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray);
    OGL_GET_EXTENSION(PFNGLDELETEVERTEXARRAYSPROC, glDeleteVertexArrays);

    // Shaders
    OGL_GET_EXTENSION(PFNGLCREATESHADERPROC, glCreateShader);
    OGL_GET_EXTENSION(PFNGLSHADERSOURCEPROC, glShaderSource);
    OGL_GET_EXTENSION(PFNGLCOMPILESHADERPROC, glCompileShader);
    OGL_GET_EXTENSION(PFNGLGETSHADERIVPROC, glGetShaderiv);
    OGL_GET_EXTENSION(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog);
    OGL_GET_EXTENSION(PFNGLDELETESHADERPROC, glDeleteShader);
    OGL_GET_EXTENSION(PFNGLCREATEPROGRAMPROC, glCreateProgram);
    OGL_GET_EXTENSION(PFNGLATTACHSHADERPROC, glAttachShader);
    OGL_GET_EXTENSION(PFNGLLINKPROGRAMPROC, glLinkProgram);
    OGL_GET_EXTENSION(PFNGLGETPROGRAMIVPROC, glGetProgramiv);
    OGL_GET_EXTENSION(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog);
    OGL_GET_EXTENSION(PFNGLDELETEPROGRAMPROC, glDeleteProgram);
    OGL_GET_EXTENSION(PFNGLUSEPROGRAMPROC, glUseProgram);

    return allExtensionsAvailable;
}

} // namepsace OGLExt
