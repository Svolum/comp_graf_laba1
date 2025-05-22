#version 460 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
out vec4 frag_color;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform sampler2D texture1;

void main() {
    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Ambient
    vec3 ambient = 0.1 * lightColor;

    // Texture
    vec4 texColor = texture(texture1, TexCoord);

    // Combine
    vec3 result = (ambient + diffuse) * texColor.rgb;
    frag_color = vec4(1.0, 0.0, 1.0, texColor.a);
}