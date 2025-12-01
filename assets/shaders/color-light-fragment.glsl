#version 330 core
out vec4 frag_result;

uniform vec3 color;

void main() {
    frag_result = vec4(color, 1.0);
}
