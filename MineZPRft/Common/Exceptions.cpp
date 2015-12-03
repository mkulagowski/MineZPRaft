/**
 * @file
 * @author mkkulagowski (mkkulagowski(at)gmail.com)
 * @brief  Exception classes definition.
 */

#include "Exceptions.hpp"

std::string MainException::ErrorMsg() const
{
    return "Exception in ";
};

std::string MainRendererException::ErrorMsg() const
{
    std::string result(MainException::ErrorMsg());
    result += "Renderer";
    return result;
};

std::string ShaderFileException::ErrorMsg() const
{
    std::string result(MainRendererException::ErrorMsg());
    result += "/Shader: Shaders failed to load from disk.";
    return result;
};