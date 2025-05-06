cmake_minimum_required(VERSION 3.10)
project(3DAudioVisualizer)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)

# GLFW and GLEW
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)
pkg_check_modules(GLEW REQUIRED glew)

# Include directories
include_directories(
    ${GLFW_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    /usr/include/glm
    ${CMAKE_CURRENT_SOURCE_DIR}/external
    ${CMAKE_CURRENT_SOURCE_DIR}/external/kissfft
)

# Audio Analyzer static lib
add_library(audioAnalyzer
    audioAnalyzer.cpp
    external/kissfft/kiss_fft.c
    external/miniaudio.cpp
)

# Main executable
add_executable(particle main.cpp)

# Link everything together
target_link_libraries(particle
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${GLFW_LIBRARIES}
    audioAnalyzer
)
