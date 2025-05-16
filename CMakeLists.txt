cmake_minimum_required(VERSION 3.20)
project(comp_grafika_laba1_var9 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Specify paths to GLFW and GLEW
set(GLFW_DIR "${CMAKE_SOURCE_DIR}/lib/glfw-3.4.bin.WIN64")
set(GLEW_DIR "${CMAKE_SOURCE_DIR}/lib/glew-2.1.0")

# Include directories
include_directories(
    ${GLFW_DIR}/include
    ${GLEW_DIR}/include
)

# Link directories
link_directories(
    ${GLFW_DIR}/lib-vc2022
    ${GLEW_DIR}/lib/Release/x64
)

# Add executable
add_executable(${PROJECT_NAME} src/main.cpp)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    OpenGL::GL
    glfw3
    glew32
    User32
    Gdi32
    Shell32
)

# Copy DLLs to output directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${GLEW_DIR}/bin/Release/x64/glew32.dll"
        "${GLFW_DIR}/lib-vc2022/glfw3.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
)