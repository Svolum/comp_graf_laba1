#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "shader_load.hpp"


const unsigned int SRC_WIDTH = 1024;
const unsigned int SRC_HEIGHT = 768;

float lastX = SRC_WIDTH / 2, lastY = SRC_HEIGHT / 2;
float yaw = -90.0f, pitch = 0.0f, roll = 90.0f;
float sensetivity = 0.01f;
bool firstMouse = true;

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
    // printf("%.1f\t", yaw);
    // printf("%.1f\n", pitch);
    // нельзя смотреть прямо вверх или вниз
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;
}
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

    // Camera
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    const float cameraSpeed = 0.001f;

    // Projection
    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f), // fovy vertical
        1.0f, // aspect // or ratio
        0.1f, //zNear
        100.0f  //zFar
    );
    // Viev
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    
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

    // Shader
    GLuint shaderProgram = createShaderProgram("shaders/vertex.glsl", "shaders/fragment.glsl");

    // Cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);


    while (!glfwWindowShouldClose(window)) {
        // обработка ввода
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true); // Закрыть окно
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cameraPos -= cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cameraPos += cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            cameraPos += cameraUp * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            cameraPos -= cameraUp * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            roll += sensetivity;
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            roll -= sensetivity;
        printf("%.3f\t%.3f\t%.3f\n", roll, yaw, pitch);

        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(direction);

        cameraUp = glm::normalize(glm::vec3(cos(glm::radians(roll)), sin(glm::radians(roll)), 0.0f));

        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

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


        // Get locations
        GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
        GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view)); 
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


        glBindVertexArray(VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 3); т.к. индексированный рендеринг
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // printf("%.2f\t%.2f\t%.2f\t\t%.2f\t%.2f\t%.2f\t\t%.2f\t%.2f\t%.2f\n",
        //     cameraPos.x, cameraPos.y, cameraPos.z,
        //     cameraFront.x, cameraFront.y, cameraFront.z,
        //     cameraUp.x, cameraUp.y, cameraUp.z
        // );

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glfwTerminate();
    return 0;
}