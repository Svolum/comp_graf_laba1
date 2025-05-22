#version 460 core
out vec4 frag_color;

in vec3 Normal; // от вершинного шейдера
in vec3 FragPos;

uniform vec3 lightPos;        // позиция точечного источника света
uniform vec3 lightColor;      // цвет света
uniform vec3 objectColor;     // цвет объекта

// Параметры затухания
uniform float constant;       // постоянный коэффициент
uniform float linear;         // линейный коэффициент
uniform float quadratic;      // квадратичный коэффициент

void main() {
    // Нормализация нормали
    vec3 norm = normalize(Normal);

    // Вычисление направления света от фрагмента к источнику
    vec3 lightDir = normalize(lightPos - FragPos);

    // Диффузное освещение
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * objectColor;

    // Вычисление расстояния до источника света
    float distance = length(lightPos - FragPos);

    // Затухание
    float attenuation = 1.0 / (constant + linear * distance + quadratic * distance * distance);

    // Итоговый цвет с учетом затухания
    vec3 result = diffuse * attenuation;
    frag_color = vec4(result, 1.0);
}