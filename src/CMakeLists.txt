# CMake project file for W4160 Computer Graphics
# Changxi Zheng (cxz@cs.columbia.edu)
project(PA3)

cmake_minimum_required(VERSION 2.6.2 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(BUILD_DEBUG "Turn on the debug mode" OFF)
#===================================================================
## Compiler
# set compiler flags for debug/release
if ( BUILD_DEBUG )
    add_definitions(-DDEBUG)
    set(CMAKE_BUILD_TYPE Debug)
else ()
    add_definitions(-Wno-deprecated)
    set(CMAKE_BUILD_TYPE Release)
endif ()

# package for opengl and glut
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
# find glew
find_package(GLEW REQUIRED)

include_directories(
    ${GLUT_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIR})

add_executable(pa3
    main.cpp
    shader.h
    trackball.h         trackball.cpp
    GLSLProgram.h       GLSLProgram.cpp
    GLScreenCapturer.h  GLScreenCapturer.cpp)
target_link_libraries(pa3
    ${GLUT_LIBRARIES}       ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARY})
