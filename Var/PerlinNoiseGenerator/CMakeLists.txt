# @file
# @author LKostyra (costyrra.xl@gmail.com)
# @brief  CMake for PerlinNoiseGenerator

MESSAGE("Generating Makefile for PerlinNoiseGenerator")

FILE(GLOB PERLINGEN_SOURCES *.cpp)
FILE(GLOB PERLINGEN_HEADERS *.hpp)

ADD_EXECUTABLE(PerlinNoiseGenerator ${PERLINGEN_SOURCES} ${PERLINGEN_HEADERS})
ADD_CUSTOM_COMMAND(TARGET PerlinNoiseGenrator POST_BUILD COMMAND
                   ${CMAKE_COMMAND} -E copy $<TARGET_FILE:PerlinNoiseGenerator> ${MZPR_OUTPUT_DIRECTORY}/${targetfile})
