cmake_minimum_required(VERSION 3.10)
project(ParticleVisualizer)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)

# Add GLFW and GLEW
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)
pkg_check_modules(GLEW REQUIRED glew)

# Manually set GLM include path
include_directories(
    ${GLFW_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    /usr/include/glm
)

add_executable(particle main.cpp)

target_link_libraries(particle
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${GLFW_LIBRARIES}
)
