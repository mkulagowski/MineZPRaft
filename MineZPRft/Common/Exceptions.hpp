/**
 * @file
 * @author mkkulagowski (mkkulagowski(at)gmail.com)
 * @brief  Exception classes declaration.
 */

#ifndef __COMMON_EXCEPTIONS_HPP__
#define __COMMON_EXCEPTIONS_HPP__

#include <exception>
#include <string>


// =============================
// Main exception classes
// =============================

/**
 * Virtual class used for exception subclasses creation
 */
class MainException : public std::exception
{
public:
    /**
     * Get error message
     */
    virtual std::string ErrorMsg() const;
};

/**
 * Exception class for Renderer assosiated things
 */
class MainRendererException : public MainException
{
public:
    /**
     * Get error message
     */
    virtual std::string ErrorMsg() const;
};


// =============================
// Renderer exception classes
// =============================

/**
 * Shader file exception
 */
class ShaderFileException : public MainRendererException
{
public:
    /**
     * Get error message
     */
    std::string ErrorMsg() const;
};

#endif // __COMMON_EXCEPTIONS_HPP__