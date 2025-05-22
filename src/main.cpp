#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>
#include <iostream>

#include "shader_load.hpp"
#include "my_viev.hpp"

const unsigned int SRC_WIDTH = 1024;
const unsigned int SRC_HEIGHT = 768;

struct MeshInfo {
    unsigned int indexCount;
    unsigned int indexOffset; // в элементах, не в байтах
    glm::vec3 color;
};
std::vector<MeshInfo> meshParts;

int main() {
    // Инициализация GLFW
    if (!glfwInit()) {
        std::cerr << "ОШИБКА: не удалось инициализировать GLFW3." << std::endl;
        return 1;
    }

    // Установка версии OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Создание окна
    GLFWwindow* window = glfwCreateWindow(SRC_WIDTH, SRC_HEIGHT, "Pashalka", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        std::cerr << "ОШИБКА: не удалось создать окно." << std::endl;
        return 1;
    }

    glfwMakeContextCurrent(window);

    // Инициализация GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "ОШИБКА: не удалось инициализировать GLEW." << std::endl;
        glfwTerminate();
        return 1;
    }

    std::cout << "Версия OpenGL: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Рендерер: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Версия GLEW: " << glewGetString(GLEW_VERSION) << std::endl;

    // Включение теста глубины
    glEnable(GL_DEPTH_TEST);

    // Настройка камеры
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    const float cameraSpeed = 0.005f;

    // Проекционная матрица
    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),
        static_cast<float>(SRC_WIDTH) / static_cast<float>(SRC_HEIGHT),
        0.1f,
        100.0f
    );
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    // Загрузка модели
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile("models/var.obj", aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ОШИБКА: " << importer.GetErrorString() << std::endl;
        glfwTerminate();
        return 1;
    }
    if (!scene->HasMeshes()) {
        std::cerr << "ОШИБКА: в модели нет мешей." << std::endl;
        glfwTerminate();
        return 1;
    }

    // Загрузка вершин, нормалей и индексов
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    unsigned int currentIndexOffset = 0;
    
    for (unsigned int m = 0; m < scene->mNumMeshes; ++m) {
        aiMesh* mesh = scene->mMeshes[m];
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        aiColor3D kd(0.8f, 0.8f, 0.8f); // Цвет по умолчанию
        material->Get(AI_MATKEY_COLOR_DIFFUSE, kd);

        unsigned int vertexOffset = vertices.size() / 6;
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            // Вершины
            vertices.push_back(mesh->mVertices[i].x);
            vertices.push_back(mesh->mVertices[i].y);
            vertices.push_back(mesh->mVertices[i].z);
            // Нормали
            vertices.push_back(mesh->mNormals[i].x);
            vertices.push_back(mesh->mNormals[i].y);
            vertices.push_back(mesh->mNormals[i].z);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; ++j) {
                indices.push_back(face.mIndices[j] + vertexOffset);
            }
        }

        // Добавление информации о цвете и индексе
        MeshInfo info;
        info.indexCount = mesh->mNumFaces * 3;
        info.indexOffset = currentIndexOffset;
        info.color = glm::vec3(kd.r, kd.g, kd.b);
        meshParts.push_back(info);
        currentIndexOffset += info.indexCount;
    }

    // Настройка VBO, VAO, EBO
    GLuint VBO, VAO, EBO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Позиции вершин
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Нормали
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Создание шейдерной программы
    GLuint shaderProgram = createShaderProgram("shaders/vertex.glsl", "shaders/fragment.glsl");
    if (shaderProgram == 0) {
        std::cerr << "ОШИБКА: не удалось создать шейдерную программу." << std::endl;
        glfwTerminate();
        return 1;
    }

    // Настройка курсора
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    // Основной цикл рендеринга
    while (!glfwWindowShouldClose(window)) {
        // Обработка ввода
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cameraPos += cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cameraPos -= cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            cameraPos += cameraUp * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            cameraPos -= cameraUp * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            roll += sensetivity;
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            roll -= sensetivity;

        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(direction);

        cameraUp = glm::normalize(glm::vec3(cos(glm::radians(roll)), sin(glm::radians(roll)), 0.0f));

        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // Очистка буфера
        glClearColor(1.0f, 0.4f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // Параметры освещения
        glm::vec3 lightDir = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f)); // Свет сверху-спереди-слева
        glm::vec3 lightColor(1.0f, 1.0f, 1.0f); // Белый свет
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightDir"), 1, glm::value_ptr(lightDir));
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, glm::value_ptr(lightColor));

        // Установка матриц
        glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)); // Масштаб 1.0
        GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
        GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
        GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Отрисовка модели
        glBindVertexArray(VAO);
        for (const auto& meshPart : meshParts) {
            GLint colorLoc = glGetUniformLocation(shaderProgram, "objectColor");
            glUniform3f(colorLoc, meshPart.color.r, meshPart.color.g, meshPart.color.b);

            glDrawElements(
                GL_TRIANGLES,
                meshPart.indexCount,
                GL_UNSIGNED_INT,
                (void*)(meshPart.indexOffset * sizeof(unsigned int))
            );
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Очистка ресурсов
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}