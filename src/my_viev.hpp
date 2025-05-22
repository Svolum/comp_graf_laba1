#pragma once
#include <GLFW/glfw3.h>

extern float yaw, pitch, roll, sensetivity;
extern bool firstMouse;

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);