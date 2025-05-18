#version 460 core

layout (location = 0) in vec3 vp;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 transform;

void main() {
    gl_Position = vec4(vp, 1.0);
}
