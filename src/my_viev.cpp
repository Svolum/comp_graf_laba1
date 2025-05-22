#include "my_viev.hpp"
float yaw = -90.0f, pitch = 0.0f, roll = 90.0f;
float lastX, lastY;
float sensetivity = 0.01f;
bool firstMouse = true;
// ... mouse_callback implementation ...

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn){
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse){
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    // смещение между последним и текущим кадром
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    xoffset *= sensetivity;
    yoffset *= sensetivity;


    yaw += xoffset;
    pitch += yoffset;
    // нельзя смотреть прямо вверх или вниз
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;
}