/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Common declarations for Renderer
 */

#ifndef __RENDERER_COMMON_HPP__
#define __RENDERER_COMMON_HPP__

#include <iostream>

#ifdef WIN32
#include <Windows.h>
#endif // WIN32

#include <GL/gl.h>


#ifdef _DEBUG

#include <GL/glu.h> // only for gluErrorString()

/**
 * Macro for error checking in OpenGL.
 *
 * Ideally release code should not contain this macro (it is for debug purposes only).
 */
#ifndef GL_GET_ERRORS
#define GL_GET_ERRORS()                                                         \
do                                                                              \
{                                                                               \
    GLenum err;                                                                 \
    while ((err = glGetError()) != GL_NO_ERROR)                                 \
    {                                                                           \
        const char* str = reinterpret_cast<const char*>(gluErrorString(err));   \
        std::cerr << "OpenGL error " << err << ": " << str << std::endl;        \
    }                                                                           \
    std::cerr << "==== Error queue empty ====" << std::endl;                    \
} while(0)
#endif // GL_GET_ERROR

#endif // _DEBUG


#endif // __RENDERER_COMMON_HPP__
