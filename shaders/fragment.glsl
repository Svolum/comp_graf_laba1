#version 460 core
out vec4 frag_color;

in vec3 Normal; // от вершинного шейдера
in vec3 FragPos;

uniform vec3 lightDir;        // направление света (например, vec3(-1, -1, -1))
uniform vec3 lightColor;      // цвет света (обычно белый)
uniform vec3 objectColor;     // цвет материала/объекта

void main() {
    // Нормализация входных данных
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(-lightDir); // инвертируем, т.к. свет "падает" в эту сторону

    // Рассеянное освещение (Diffuse)
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor;

    // Фоновое освещение (Ambient)
    vec3 ambient = 0.1 * lightColor;

    // Итоговый цвет
    vec3 result = (ambient + diffuse) * objectColor;
    frag_color = vec4(result, 1.0);
}
