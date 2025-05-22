#version 460 core
out vec4 frag_color;

in vec3 Normal; // от вершинного шейдера
in vec3 FragPos;

uniform vec3 lightDir;        // направление света
uniform vec3 lightColor;      // цвет света
uniform vec3 objectColor;     // цвет объекта

void main() {
    // Нормализация входных данных
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(-lightDir); // инвертируем направление света

    // Диффузное освещение
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor * objectColor;

    // Итоговый цвет
    frag_color = vec4(diffuse, 1.0);
}