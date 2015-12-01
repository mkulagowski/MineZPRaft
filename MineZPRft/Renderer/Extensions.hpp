/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  OpenGL Extensions declarations
 */

#ifndef __RENDERER_EXTENSIONS_HPP__
#define __RENDERER_EXTENSIONS_HPP__

#include "Defines.hpp"
#include "GL/glext.h"

// Namespace wrapping OpenGL Extension Functions to avoid overwriting them on Linux
namespace OGLExt {

/// Buffers
extern PFNGLGENBUFFERSPROC glGenBuffers;
extern PFNGLBINDBUFFERPROC glBindBuffer;
extern PFNGLBUFFERDATAPROC glBufferData;
extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;

/// Vertex Attrib Array
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;

/// Vertex Array objects
extern PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
extern PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;

/// Shaders
extern PFNGLCREATESHADERPROC glCreateShader;
extern PFNGLSHADERSOURCEPROC glShaderSource;
extern PFNGLCOMPILESHADERPROC glCompileShader;
extern PFNGLGETSHADERIVPROC glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
extern PFNGLDELETESHADERPROC glDeleteShader;
extern PFNGLCREATEPROGRAMPROC glCreateProgram;
extern PFNGLATTACHSHADERPROC glAttachShader;
extern PFNGLLINKPROGRAMPROC glLinkProgram;
extern PFNGLGETPROGRAMIVPROC glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
extern PFNGLDELETEPROGRAMPROC glDeleteProgram;
extern PFNGLUSEPROGRAMPROC glUseProgram;
extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
extern PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;

/**
 * Initialize OpenGL extensions used by Renderer
 *
 * @return True if all extensions were successfully acquired, false otherwise
 */
bool ExtensionsInit();

} // namespace OGLExt

#endif // __RENDERER_EXTENSIONS_HPP__