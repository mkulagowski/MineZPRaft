# @file
# @author LKostyra (costyrra.xl@gmail.com)
# @brief  CMake for MineZPRft

MESSAGE("Generating Makefile for MineZPRft")

FILE(GLOB COMMON_SOURCES       Common/*.cpp)
FILE(GLOB COMMON_HEADERS       Common/*.hpp)
FILE(GLOB COMMON_LINUX_SOURCES Common/Linux/*.cpp)
FILE(GLOB COMMON_LINUX_HEADERS Common/Linux/*.hpp)
FILE(GLOB GAME_SOURCES         Game/*.cpp)
FILE(GLOB GAME_HEADERS         Game/*.hpp)
FILE(GLOB MATH_SOURCES         Math/*.cpp)
FILE(GLOB MATH_HEADERS         Math/*.hpp)
FILE(GLOB RENDERER_SOURCES     Renderer/*.cpp)
FILE(GLOB RENDERER_HEADERS     Renderer/*.hpp)
FILE(GLOB TERRAIN_SOURCES      Terrain/*.cpp)
FILE(GLOB TERRAIN_HEADERS      Terrain/*.hpp)

# Search for dependencies
PKG_CHECK_MODULES(MINEZPRFT_DEPS REQUIRED
                  x11
                  gl)

# setup directories
INCLUDE_DIRECTORIES(${MINEZPRFT_DEPS_INCLUDE_DIRS} ${MZPR_ROOT_DIRECTORY}/MineZPRft)

ADD_EXECUTABLE(MineZPRft ${COMMON_SOURCES} ${COMMON_HEADERS}
                         ${COMMON_LINUX_SOURCES} ${COMMON_LINUX_HEADERS}
                         ${GAME_SOURCES} ${GAME_HEADERS}
                         ${MATH_SOURCES} ${MATH_HEADERS}
                         ${RENDERER_SOURCES} ${RENDERER_HEADERS}
                         ${TERRAIN_SOURCES} ${TERRAIN_HEADERS})
SET_TARGET_PROPERTIES(MineZPRft PROPERTIES
                      COMPILE_FLAGS "-pthread"
                      LINK_FLAGS "-pthread")
TARGET_LINK_LIBRARIES(MineZPRft ${MINEZPRFT_DEPS_LIBRARIES})
ADD_CUSTOM_COMMAND(TARGET MineZPRft POST_BUILD COMMAND
                   ${CMAKE_COMMAND} -E copy $<TARGET_FILE:MineZPRft> ${MZPR_OUTPUT_DIRECTORY}/${targetfile})
