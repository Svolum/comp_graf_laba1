#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "ERROR: could not start GLFW3." << std::endl;
        return 1;
    }

    // Specify OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create window
    GLFWwindow* window = glfwCreateWindow(512, 512, "Trapezoid", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        std::cerr << "ERROR: could not create window." << std::endl;
        return 1;
    }

    // Make context current
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "ERROR: could not initialize GLEW." << std::endl;
        return 1;
    }

    // Print OpenGL version
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "GLEW Version: " << glewGetString(GLEW_VERSION) << std::endl;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Set background color (Variant 9: 1.0, 0.4, 0.1)
        glClearColor(1.0f, 0.4f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw trapezoid (Variant 9: color 0.7, 0.1, 0.6)
        glBegin(GL_QUADS);
        glColor3f(0.7f, 0.1f, 0.6f);
        glVertex2f(-0.5f, -0.5f); // Bottom-left
        glVertex2f(0.5f, -0.5f);  // Bottom-right
        glVertex2f(0.3f, 0.5f);   // Top-right
        glVertex2f(-0.3f, 0.5f);  // Top-left
        glEnd();

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glfwTerminate();
    return 0;
}