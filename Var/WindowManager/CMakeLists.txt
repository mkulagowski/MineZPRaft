# @file
# @author LKostyra (costyrra.xl@gmail.com)
# @brief  CMake for WindowManager

MESSAGE("Generating Makefile for WindowManager")

FILE(GLOB WINDOWMANAGER_SOURCES       *.cpp)
FILE(GLOB WINDOWMANAGER_HEADERS       *.hpp)
FILE(GLOB WINDOWMANAGER_LINUX_SOURCES Linux/*.cpp)
FILE(GLOB WINDOWMANAGER_LINUX_HEADERS Linux/*.hpp)

# Search for dependencies
PKG_CHECK_MODULES(WINDOWMANAGER_DEPS REQUIRED
                  x11
                  gl)

# setup directories
INCLUDE_DIRECTORIES(${WINDOWMANAGER_DEPS_INCLUDE_DIRS})

ADD_EXECUTABLE(WindowManager ${WINDOWMANAGER_SOURCES} ${WINDOWMANAGER_HEADERS}
                             ${WINDOWMANAGER_LINUX_SOURCES} ${WINDOWMANAGER_LINUX_HEADERS})
SET_TARGET_PROPERTIES(WindowManager PROPERTIES
                      COMPILE_FLAGS "-pthread"
                      LINK_FLAGS "-pthread")
TARGET_LINK_LIBRARIES(WindowManager ${WINDOWMANAGER_DEPS_LIBRARIES})
ADD_CUSTOM_COMMAND(TARGET WindowManager POST_BUILD COMMAND
                   ${CMAKE_COMMAND} -E copy $<TARGET_FILE:WindowManager> ${MZPR_OUTPUT_DIRECTORY}/${targetfile})
