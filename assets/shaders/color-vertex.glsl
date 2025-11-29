#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

out vec3 pos0;
out vec3 normal0;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(pos, 1.0);
    pos0 = pos;
    normal0 = normal;
}
