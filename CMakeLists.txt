cmake_minimum_required(VERSION 3.20)
project(comp_grafika_laba1_var9 LANGUAGES C CXX)

# Set C++ standard
# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# enable_language(C)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Specify paths
set(GLFW_DIR    "${CMAKE_SOURCE_DIR}/lib/glfw-3.4.bin.WIN64")
set(GLEW_DIR    "${CMAKE_SOURCE_DIR}/lib/glew-2.1.0")
set(GLAD_DIR    "${CMAKE_SOURCE_DIR}/lib/glad")
set(GLM_DIR     "${CMAKE_SOURCE_DIR}/lib/glm")
set(ASSIMP_DIR "${CMAKE_SOURCE_DIR}/lib/assimp")

# Add glad library
add_library(glad STATIC ${GLAD_DIR}/src/glad.c)
target_include_directories(glad PUBLIC ${GLAD_DIR}/include)

# Include directories
include_directories(
    ${GLFW_DIR}/include
    ${GLEW_DIR}/include
    ${GLAD_DIR}/include
    ${GLM_DIR}
    ${ASSIMP_DIR}/include
)

# Link directories
link_directories(
    ${GLFW_DIR}/lib-vc2022
    ${GLEW_DIR}/lib/Release/x64
    ${ASSIMP_DIR}/lib
)

# Add executable
add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/shader_load.hpp
    src/shader_load.cpp
    src/my_viev.hpp
    src/my_viev.cpp
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    OpenGL::GL
    glfw3
    glew32
    glad
    assimp-vc143-mt
    User32
    Gdi32
    Shell32
)

# Copy DLLs to output directory
file(GLOB ASSIMP_DLLS
    "${ASSIMP_DIR}/bin/*.dll"
    "${ASSIMP_DIR}/debug/bin/*.dll"
)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${GLEW_DIR}/bin/Release/x64/glew32.dll"
        "${GLFW_DIR}/lib-vc2022/glfw3.dll"
        ${ASSIMP_DLLS}
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

# Copy shaders folder to the build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/shaders"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/models"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/models
)