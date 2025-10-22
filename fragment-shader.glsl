#version 330 core
out vec4 frag_color;
in vec4 vertex_color;

uniform vec4 global_color;

void main() {
    frag_color = global_color * vertex_color;
    //frag_color = vertex_color;
}
