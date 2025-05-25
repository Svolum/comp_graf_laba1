#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <iostream>
#include "shader_load.hpp"
#include "my_viev.hpp"

#include <windows.h>


const unsigned int SRC_WIDTH = 1024;
const unsigned int SRC_HEIGHT = 768;


struct MeshInfo {
    unsigned int indexCount;
    unsigned int indexOffset; // в элементах, не в байтах
    glm::vec3 color;
};
std::vector<MeshInfo> meshParts;
glm::vec3 compareTwoVectors(aiVector3D v1, glm::vec3 v2, bool way)
{
    if (way) {
        if (v1.x > v2.x) v2.x = v1.x;
        if (v1.y > v2.y) v2.y = v1.y;
        if (v1.z > v2.z) v2.z = v1.z;
    } else {
        if (v1.x < v2.x) v2.x = v1.x;
        if (v1.y < v2.y) v2.y = v1.y;
        if (v1.z < v2.z) v2.z = v1.z;
    }
    return v2;
}
auto rotAroundPoint(float rad, const glm::vec3& point, const glm::vec3& axis)
{
    
    auto t1 = glm::translate(glm::mat4(1),-point);
    auto r = glm::rotate(glm::mat4(1),rad,axis);
    auto t2 = glm::translate(glm::mat4(1),point);
    return t2 * r * t1;
}
int main() {
    SetConsoleOutputCP(65001); // UTF-8
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
    GLFWwindow* window = glfwCreateWindow(SRC_WIDTH, SRC_HEIGHT, "Model Viewer", nullptr, nullptr);
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
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 10.0f); // Ближе к модели
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    const float cameraSpeed = 0.003f;

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
    const aiScene* scene = importer.ReadFile("models/3.obj", aiProcess_Triangulate | aiProcess_FlipUVs);
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
    glm::vec3 min_hotizontal_on_start_CO(FLT_MAX), max_hotizontal_on_start_CO(FLT_MIN);
    glm::vec3 min_horizontal_on_end_CO(FLT_MAX), max_horizontal_on_end_CO(FLT_MIN);
    for (unsigned int m = 0; m < scene->mNumMeshes; ++m) {
        aiMesh* mesh = scene->mMeshes[m];
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        aiColor3D kd(0.8f, 0.8f, 0.8f); // Цвет по умолчанию
        material->Get(AI_MATKEY_COLOR_DIFFUSE, kd);

        std::cout << "Загрузка меша " << m << ": " << mesh->mName.C_Str() << std::endl;
        unsigned int vertexOffset = vertices.size() / 6;
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            vertices.push_back(mesh->mVertices[i].x);
            vertices.push_back(mesh->mVertices[i].y);
            vertices.push_back(mesh->mVertices[i].z);
            if (m == 5) {
                // if (mesh->mVertices[i].x < min_hotizontal_on_start_CO.x) min_hotizontal_on_start_CO.x = mesh->mVertices[i].x;
                // if (mesh->mVertices[i].y < min_hotizontal_on_start_CO.y) min_hotizontal_on_start_CO.y = mesh->mVertices[i].y;
                // if (mesh->mVertices[i].z < min_hotizontal_on_start_CO.z) min_hotizontal_on_start_CO.z = mesh->mVertices[i].z;

                // if (mesh->mVertices[i].x > max_hotizontal_on_start_CO.x) max_hotizontal_on_start_CO.x = mesh->mVertices[i].x;
                // if (mesh->mVertices[i].y > max_hotizontal_on_start_CO.y) max_hotizontal_on_start_CO.y = mesh->mVertices[i].y;
                // if (mesh->mVertices[i].z > max_hotizontal_on_start_CO.z) max_hotizontal_on_start_CO.z = mesh->mVertices[i].z;
                max_hotizontal_on_start_CO = compareTwoVectors(mesh->mVertices[i], max_hotizontal_on_start_CO, true);
                min_hotizontal_on_start_CO = compareTwoVectors(mesh->mVertices[i], min_hotizontal_on_start_CO, false);
            }
            if (m == 6) {
                max_horizontal_on_end_CO = compareTwoVectors(mesh->mVertices[i], max_horizontal_on_end_CO, true);
                min_horizontal_on_end_CO = compareTwoVectors(mesh->mVertices[i], min_horizontal_on_end_CO, false);
            }

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

        MeshInfo info;
        info.indexCount = mesh->mNumFaces * 3;
        info.indexOffset = currentIndexOffset;
        info.color = glm::vec3(kd.r, kd.g, kd.b);
        meshParts.push_back(info);
        currentIndexOffset += info.indexCount;
    }
    // std::cout << "Загружено " << meshParts.size() << " мешей." << std::endl;
    // printf("Минимальная координата: (%.2f, %.2f, %.2f)\n", min_hotizontal_on_start_CO.x, min_hotizontal_on_start_CO.y, min_hotizontal_on_start_CO.z);
    // printf("Максимальная координата: (%.2f, %.2f, %.2f)\n", max_hotizontal_on_start_CO.x, max_hotizontal_on_start_CO.y, max_hotizontal_on_start_CO.z);
    // printf("Минимальная координата: (%.2f, %.2f, %.2f)\n", min_horizontal_on_end_CO.x, min_horizontal_on_end_CO.y, min_horizontal_on_end_CO.z);
    // printf("Максимальная координата: (%.2f, %.2f, %.2f)\n", max_horizontal_on_end_CO.x, max_horizontal_on_end_CO.y, max_horizontal_on_end_CO.z);
    // Вычисление центра модели
    glm::vec3 centerOf_hotizontal_on_start_Model = (min_hotizontal_on_start_CO + max_hotizontal_on_start_CO) / 2.0f;
    glm::vec3 centerOf_hotizontal_on_end_Model = (min_horizontal_on_end_CO + max_horizontal_on_end_CO) / 2.0f;
    // printf("(%.2f, %.2f, %.2f)\n", centerOf_hotizontal_on_start_Model.x, centerOf_hotizontal_on_start_Model.y, centerOf_hotizontal_on_start_Model.z);
    // printf("(%.2f, %.2f, %.2f)\n", centerOf_hotizontal_on_end_Model.x, centerOf_hotizontal_on_end_Model.y, centerOf_hotizontal_on_end_Model.z);
    

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


    glm::vec3 lightPos = glm::vec3(-3.0f, 3.0f, 3.0f); // Свет сверху-спереди-справа

    glm::vec3 modelsUp(0.0f, 1.0f, 0.0f); // Вверх для моделей
    float models_speed = 0.05f; // Скорость вращения моделей
    float models_move_speed = 0.001f; // Скорость движения моделей
    float vertical_big = 0.0f;
    float hotizontal_on_start = 0.0f;
    float horizontal_on_end = 0.0f;
    float vertical_mini = 0.0f;
    // Основной цикл рендеринга
    while (!glfwWindowShouldClose(window)) {
        // Обработка ввода для света
        if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
            lightPos += glm::vec3(0.0f, 0.0f, -1.0f) * cameraSpeed; // forward
        if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
            lightPos += glm::vec3(0.0f, 0.0f, 1.0f) * cameraSpeed; // backward
        if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
            lightPos += glm::vec3(-1.0f, 0.0f, 0.0f) * cameraSpeed; // left
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
            lightPos += glm::vec3(1.0f, 0.0f, 0.0f) * cameraSpeed; // right
        if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
            lightPos += glm::vec3(0.0f, 1.0f, 0.0f) * cameraSpeed; // up
        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
            lightPos += glm::vec3(0.0f, -1.0f, 0.0f) * cameraSpeed; // down
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
        // Обработка models
        // Управление вертикальным положением big model
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
            vertical_big += models_move_speed;
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
            vertical_big -= models_move_speed;
        if (vertical_big < -0.2f) vertical_big = -0.2f; // Ограничение вниз
        if (vertical_big > 0.8f) vertical_big = 0.8f; // Ограничение вверх
        // Управление вращением моделей
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
            hotizontal_on_start += models_speed;
        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
            hotizontal_on_start -= models_speed;
        float hotizontal_on_start_ogr = 70.0f;
        if (hotizontal_on_start > hotizontal_on_start_ogr ) hotizontal_on_start = hotizontal_on_start_ogr ;
        if (hotizontal_on_start < -hotizontal_on_start_ogr ) hotizontal_on_start = -hotizontal_on_start_ogr ;
        // Управление вращением моделей
        if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
            horizontal_on_end += models_speed;
        if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
            horizontal_on_end -= models_speed;
        float horizontal_on_end_ogr = 100.0f;
        if (horizontal_on_end > horizontal_on_end_ogr) horizontal_on_end = horizontal_on_end_ogr;
        if (horizontal_on_end < -horizontal_on_end_ogr) horizontal_on_end = -horizontal_on_end_ogr; 
        // Управление вертикальным положением мини-модели
        if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
            vertical_mini += models_move_speed;
        if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
            vertical_mini -= models_move_speed;
        if (vertical_mini < -0.26f) vertical_mini = -0.26f; // Ограничение вниз
        if (vertical_mini > 0.0f) vertical_mini = 0.0f; // Ограничение вверх

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

        // Параметры точечного источника света
        glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f); // Белый свет
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, glm::value_ptr(lightColor));

        // Параметры затухания
        float constant = 0.3f;
        float linear = 0.09f;
        float quadratic = 0.032f;
        glUniform1f(glGetUniformLocation(shaderProgram, "constant"), constant);
        glUniform1f(glGetUniformLocation(shaderProgram, "linear"), linear);
        glUniform1f(glGetUniformLocation(shaderProgram, "quadratic"), quadratic);

        // Установка матриц
        glm::mat4 model = glm::mat4(1.0f); // Масштаб 0.5
        GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
        GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
        GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
        
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Отрисовка модели
        glBindVertexArray(VAO);
        int m = 0;
        for (const auto& meshPart : meshParts) {
            GLint colorLoc = glGetUniformLocation(shaderProgram, "objectColor");
            glUniform3f(colorLoc, meshPart.color.r, meshPart.color.g, meshPart.color.b);

            if (m == 0) {
                
            } else if (m == 1) {
                model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, vertical_big, 0.0f));
            } else if (m == 2) {
                model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, vertical_big, 0.0f));
                model *= rotAroundPoint(glm::radians(hotizontal_on_start), centerOf_hotizontal_on_start_Model, glm::vec3(0.0f, 1.0f, 0.0f));
            } else if (m == 3) {
                model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, vertical_big, 0.0f));
                model *= rotAroundPoint(glm::radians(hotizontal_on_start), centerOf_hotizontal_on_start_Model, glm::vec3(0.0f, 1.0f, 0.0f));
                model *= rotAroundPoint(glm::radians(horizontal_on_end), centerOf_hotizontal_on_end_Model, glm::vec3(0.0f, 1.0f, 0.0f));
            } else if (m == 4) {
                model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, vertical_big, 0.0f));
                model *= rotAroundPoint(glm::radians(hotizontal_on_start), centerOf_hotizontal_on_start_Model, glm::vec3(0.0f, 1.0f, 0.0f));
                model *= rotAroundPoint(glm::radians(horizontal_on_end), centerOf_hotizontal_on_end_Model, glm::vec3(0.0f, 1.0f, 0.0f));
                model *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, vertical_mini, 0.0f));
            } else if (m == 5) {
                model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, vertical_big, 0.0f));
            } else if (m == 6) {
                model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, vertical_big, 0.0f));
                model *= rotAroundPoint(glm::radians(hotizontal_on_start), centerOf_hotizontal_on_start_Model, glm::vec3(0.0f, 1.0f, 0.0f));
                // glm::vec3 center = centerOf_hotizontal_on_end_Model; 
                // glm::mat4 rotMat = rotAroundPoint(hotizontal_on_start, centerOf_hotizontal_on_start_Model, glm::vec3(0.0f, 1.0f, 0.0f));

                // glm::vec4 transformed = rotMat * glm::vec4(center, 1.0f);
                // glm::vec3 newPos = glm::vec3(transformed);

                // glm::vec3 offset = newPos - center;
                // centerOf_hotizontal_on_end_Model += offset;
                // model = rotMat;
            } else {
                model = glm::mat4(1.0f); // Сброс модели для остальных мешей
            }
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawElements(
                GL_TRIANGLES,
                meshPart.indexCount,
                GL_UNSIGNED_INT,
                (void*)(meshPart.indexOffset * sizeof(unsigned int))
            );
            m++;
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
