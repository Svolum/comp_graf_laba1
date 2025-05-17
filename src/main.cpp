#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>


std::string loadShaderSource(const char* filepath);
GLuint compileShader(GLenum type, const char* source);
GLuint createShaderProgram(const char* vertPath, const char* fragPath);
int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "ERROR: could not start GLFW3." << std::endl;
        return 1;
    }

    // Specify OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

    // vertices
    float points[] = { // трапеция
        -0.5f,  -0.5f,  0.0f,
        0.5f,   -0.5f,  0.0f,
        0.3f,   0.5f,   0.0f,
        -0.3f,  0.5f,   0.0f,
    };
    GLuint indies[] = {
        0, 1, 2,
        0, 2, 3
    };
    
    // VBO, VAO
    GLuint VBO, VAO, EBO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO); // привязка VAO
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // привязка данных
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // привязка сохранится в VAO, OpenGL использует его в glDrawElements
    // Если потом привязать этот VAO снова — EBO привяжется автоматически
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indies), indies, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0); // Включает использование вершинного атрибута с индексом 0. Без этого вызова данные атрибута не будут переданы в шейдер

    glBindBuffer(GL_ARRAY_BUFFER, 0); // отвязка данных
    glBindVertexArray(0); // отвязка VAO

    GLuint shaderProgram = createShaderProgram("shaders/vertex.glsl", "shaders/fragment.glsl");

    while (!glfwWindowShouldClose(window)) {
        // Set background color (Variant 9: 1.0, 0.4, 0.1)
        glClearColor(1.0f, 0.4f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // динамический цвет
        float timeValue = glfwGetTime();
        float greenValue = sin(timeValue) / 2.0f + 0.5f;
        float redValue = cos(timeValue) / 2.0f + 0.5f;
        // std::cout << greenValue << std::endl;
        float vertexColorLocation = glGetUniformLocation(shaderProgram, "our_color");
        glUniform4f(vertexColorLocation, redValue, greenValue, 0.0f, 1.0f);

        glBindVertexArray(VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 3); т.к. индексированный рендеринг
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glfwTerminate();
    return 0;
}
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

GLuint createShaderProgram(const char* vert_path, const char* frag_path) {
    std::string vert_shader_source = loadShaderSource(vert_path);
    std::string frag_shader_source = loadShaderSource(frag_path);
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