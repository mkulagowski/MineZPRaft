# @file
# @author LKostyra (costyrra.xl@gmail.com)
# @brief  CMake for gtest

FILE(GLOB_RECURSE GTEST_SOURCES *.cc)
FILE(GLOB_RECURSE GTEST_HEADERS *.h)

INCLUDE_DIRECTORIES(include)

ADD_LIBRARY(gtest ${GTEST_SOURCES} ${GTEST_HEADERS})

# gtest authors recommend compiling the library with no-missing-field-initializers warning
# For more info visit: https://groups.google.com/forum/#!topic/googletestframework/8S3QSezcoRA
SET_TARGET_PROPERTIES(gtest PROPERTIES
                      COMPILE_FLAGS "-Wno-missing-field-initializers"
                      LINK_FLAGS "-Wno-missing-field-initializers")

ADD_CUSTOM_COMMAND(TARGET gtest POST_BUILD COMMAND
                   ${CMAKE_COMMAND} -E copy $<TARGET_FILE:gtest> ${MZPR_OUTPUT_DIRECTORY}/${targetfile})
