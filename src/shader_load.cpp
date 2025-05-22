#pragma once

#include "shader_load.hpp"

std::string loadShaderSource(const char* filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filepath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    // Проверка на ошибки компиляции
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        std::cerr << "Shader compilation failed:\n" << infoLog << std::endl;
    }
    return shader;
}
GLuint createShaderProgram(const char* vertex_shader_path, const char* fragment_shader_path) {
    std::string vert_shader_source = loadShaderSource(vertex_shader_path);
    std::string frag_shader_source = loadShaderSource(fragment_shader_path);
    if (vert_shader_source.empty() || frag_shader_source.empty()) return 0;

    GLuint vertex_shader = compileShader(GL_VERTEX_SHADER, vert_shader_source.c_str());
    GLuint fragment_shader = compileShader(GL_FRAGMENT_SHADER, frag_shader_source.c_str());

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertex_shader);
    glAttachShader(shaderProgram, fragment_shader);
    glLinkProgram(shaderProgram);

    // Проверка на ошибки линковки
    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[1024];
        glGetProgramInfoLog(shaderProgram, 1024, nullptr, infoLog);
        std::cerr << "Shader program linking failed:\n" << infoLog << std::endl;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return shaderProgram;
}