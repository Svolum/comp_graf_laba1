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

    // Shader vertex program
    const char* vert_shader_source = R"(
        #version 410 core
        layout (location = 0) in vec3 vp;
        void main() {
            gl_Position = vec4(vp, 1.0);
        }
    )";
    // Shader fragment program
    const char* frag_shader_source = R"(
        #version 410 core
        uniform vec4 our_color;
        out vec4 frag_color;
        void main() {
            frag_color = our_color;
        }
    )";
    // frag_color = vec4(0.2, 0.7, 0.5, 1.0);
    // Компиляция шейдеров
    // вершинный шейдер
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vert_shader_source, NULL);
    glCompileShader(vertexShader);
    // фрагментный шейдер
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &frag_shader_source, NULL);
    glCompileShader(fragmentShader);
    // шейдерная программа
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);  // <--- Объединение

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