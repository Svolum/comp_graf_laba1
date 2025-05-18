#pragma once

#include <GL/glew.h>

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

std::string loadShaderSource(const char* filepath);
GLuint compileShader(GLenum type, const char* source);
GLuint createShaderProgram(const char* vertPath, const char* fragPath);