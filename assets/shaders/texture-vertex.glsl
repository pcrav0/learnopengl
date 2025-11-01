#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texture_coord;

out vec3 fragment_color;
out vec2 fragment_texture_coord;

void main()
{
    gl_Position = vec4(pos, 1.0);
    fragment_color = color;
    fragment_texture_coord = texture_coord;
}
