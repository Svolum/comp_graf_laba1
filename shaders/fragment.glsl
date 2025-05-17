#version 460 core
out vec4 frag_color;

uniform vec4 our_color;

void main() {
    frag_color = our_color;
}
