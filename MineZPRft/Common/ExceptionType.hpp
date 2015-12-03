/**
 * @file
 * @author mkkulagowski (mkkulagowski(at)gmail.com)
 * @brief  Exception class helper structures declarations and definitions.
 */

#ifndef __COMMON_EXCEPTIONTYPE_HPP__
#define __COMMON_EXCEPTIONTYPE_HPP__

/**
 * Enumeration that holds all possible types of exceptions
 * To be used with template class Exception<ExceptionType>
 */
enum class ExceptionType
{
    Common,
    Game,
    Math,
    Renderer,
    Terrain,
};

/**
 * Helper struct that allows us to bind string to ExceptionType enum
 */
template <ExceptionType T>
struct ExceptionHelper
{
    static const char* type;
};

// Default type name for all exception types from ExceptionType enum
template<ExceptionType T>
const char* ExceptionHelper<T>::type = "Exception";

// Personalised type names for some exception types from ExceptionType enum
const char* ExceptionHelper<ExceptionType::Game>::type = "GameException";
const char* ExceptionHelper<ExceptionType::Math>::type = "MathException";
const char* ExceptionHelper<ExceptionType::Renderer>::type = "RendererException";
const char* ExceptionHelper<ExceptionType::Terrain>::type = "TerrainException";

#endif // __COMMON_EXCEPTIONTYPE_HPP__