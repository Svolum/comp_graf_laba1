#version 460 core
out vec4 frag_color;
uniform vec3 materialColor;

void main() {
    frag_color = vec4(materialColor, 1.0);
}
