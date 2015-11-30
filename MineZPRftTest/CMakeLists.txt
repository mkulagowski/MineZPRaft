# @file
# @author LKostyra (costyrra.xl@gmail.com)
# @brief  CMake for MineZPRftTest

MESSAGE("Generating Makefile for MineZPRftTest")

FILE(GLOB TEST_SOURCES       *.cpp)
FILE(GLOB TEST_HEADERS       *.hpp)

# Units
# TODO uncomment and fill when units are available
FILE(GLOB TEST_UNIT_SOURCES ${MZPR_ROOT_DIRECTORY}/MineZPRft/Math/Vector.cpp
                            ${MZPR_ROOT_DIRECTORY}/MineZPRft/Math/Matrix.cpp)
FILE(GLOB TEST_UNIT_HEADERS ${MZPR_ROOT_DIRECTORY}/MineZPRft/Math/Vector.hpp
                            ${MZPR_ROOT_DIRECTORY}/MineZPRft/Math/Matrix.hpp)

# Requirements
FILE(GLOB TEST_REQ_SOURCES   ${MZPR_ROOT_DIRECTORY}/MineZPRft/Common/Linux/FileSystem.cpp
                             ${MZPR_ROOT_DIRECTORY}/MineZPRft/Common/Linux/Common.cpp)
FILE(GLOB TEST_REQ_HEADERS   ${MZPR_ROOT_DIRECTORY}/MineZPRft/Common/Common.hpp
                             ${MZPR_ROOT_DIRECTORY}/MineZPRft/Common/FileSystem.hpp)


# setup directories
INCLUDE_DIRECTORIES(${MZPR_ROOT_DIRECTORY}/MineZPRft
                    ${MZPR_ROOT_DIRECTORY}/gtest/include)
LINK_DIRECTORIES(${MZPR_OUTPUT_DIRECTORY})

ADD_EXECUTABLE(MineZPRftTest ${TEST_SOURCES} ${TEST_HEADERS}
                             ${TEST_UNIT_SOURCES} ${TEST_UNIT_HEADERS}
                             ${TEST_REQ_SOURCES} ${TEST_REQ_HEADERS})

# gtest requirement, which will come up during linking
SET_TARGET_PROPERTIES(MineZPRftTest PROPERTIES
                      COMPILE_FLAGS "-pthread"
                      LINK_FLAGS "-pthread")

ADD_DEPENDENCIES(MineZPRftTest gtest)
TARGET_LINK_LIBRARIES(MineZPRftTest gtest)
ADD_CUSTOM_COMMAND(TARGET MineZPRftTest POST_BUILD COMMAND
                   ${CMAKE_COMMAND} -E copy $<TARGET_FILE:MineZPRftTest>
                   ${MZPR_OUTPUT_DIRECTORY}/${targetfile})
